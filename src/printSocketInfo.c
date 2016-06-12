/*
 * printSocketInfo.c
 *
 *  Created on: May 29, 2016
 *      Author: theo
 */

#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>
#include <stdio.h>

#include "printSocketInfo.h"


struct pair {
	int number ;
	char *name ;
};

struct pair family[] = { { AF_INET, "AF_INET"}, {AF_INET6, "AF_INET6"},
		                 {AF_UNIX, "AF_UNIX"}, {AF_UNSPEC, "AF_UNSPEC"} } ;
struct pair sockType[] = { {SOCK_STREAM, "SOCK_STREAM"}, {SOCK_DGRAM, "SOCK_DGRAM"},
		                   {SOCK_SEQPACKET, "SOCK_SEQPACKET}"}, {SOCK_RAW, "SOCK_RAW"} } ;
struct pair protocol[] = { {0, "default"}, {IPPROTO_TCP, "IPPROTO_TCP"},
		                   {IPPROTO_UDP, "IPPROTO_UDP"}, {IPPROTO_RAW, "IPPROTO_RAW" } };

char* numberToString( int number, struct pair *table, int tableLen ) {
	for( int i=0 ; i < tableLen; ++i ) if( number == table[i].number ) return table[i].name ;
	return "UNKNOWN" ;
}

#define lookup( number, table) (numberToString( number, table, sizeof(table)/sizeof(struct pair)))

void printSocketAddressInfo( struct sockaddr *sap, socklen_t len ) {
	printf("Socket address information:\n") ;
	char name[NI_MAXHOST], service[NI_MAXSERV] ;
	int error=getnameinfo(sap, len, name, sizeof(name),
			service, sizeof(service), NI_NUMERICHOST);
	if( error != 0 ) { printf( "getnameinfo failed. %s\n", gai_strerror( error ) ) ; exit(1) ; }
	printf( "   ip address is %s, service is %s.\n", name, service ) ;
	if( sap->sa_family == AF_INET ) {
		struct sockaddr_in *sap4 = (struct sockaddr_in*) sap ;
		in_addr_t ipAddress = sap4->sin_addr.s_addr ;
		unsigned char* p = (unsigned char*) & ipAddress ;
		printf( "    Address is %d.%d.%d.%d, family: %s, port: %d\n",
				p[0], p[1], p[2], p[3],
				lookup(sap4->sin_family, family),
				ntohs( sap4->sin_port ) ) ;
	}
	else if( sap->sa_family == AF_INET6 ) {
		struct sockaddr_in6 *sap6 = (struct sockaddr_in6*) sap ;
		printf( "    Address is " ) ;
		for(int i=15; i >= 0 ; --i) printf( ":%x", sap6->sin6_addr.s6_addr[i] ) ;
		printf( " family: %s port: %d\n", lookup(sap6->sin6_family, family), ntohs( sap6->sin6_port ) ) ;
	}
	else assert(0) ;
}

void printAddrInfo( struct addrinfo *p ) {
	printf("Address info is ai_flags: 0x%x ai_family: %s ai_socktype %s, ai_protocol: %s ai_addrlen %d canonical ame: %s\n",
			p->ai_flags, lookup(p-> ai_family, family), lookup(p->ai_socktype, sockType),
			lookup(p-> ai_protocol, protocol), p->ai_addrlen, p->ai_canonname ) ;
	printSocketAddressInfo( p->ai_addr, p->ai_addrlen ) ;
}

void printAddrInfoList( struct addrinfo *listP ) {
	for( struct addrinfo *p = listP ; p != NULL ; p = p->ai_next ) printAddrInfo(p) ;
}
