/*
 * findAddress.c
 *
 *  Created on: May 29, 2016
 *      Author: theo
 */

#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "printSocketInfo.h"

/** findAddress  Convert a host name and port to a sockaddr structure.
 *  Input parameters:
 *     hostName: a string representation of a host name, such as localhost, or www.google.com.
 *     portName: The symbolic name or a string representation of the number. E.g. "http" or "80"
 *  Output:
 *     *serverAddressP: This structure will be filled with the appropriate socket address information
 *     *lengthP: This integer will be filled with the length of the socket address information.
 */
void findAddress( struct sockaddr * serverAddressP, socklen_t *lengthP, char *hostName, char *portName) {
	struct addrinfo *listP ; // A linked list of addrinfo records
	// This is where the DNS lookup happens.
	// We convert the string hostname to a linked list of records.
	// Each record describes a way of contacting that host.
	struct addrinfo hint ;
	memset( &hint, 0, sizeof(hint) ) ;
	hint.ai_socktype = SOCK_STREAM ;
	hint.ai_canonname = NULL, hint.ai_addr = NULL, hint.ai_next = NULL ;
	int error = getaddrinfo( hostName, portName, &hint, &listP ) ;
	if( error != 0 ) { printf( "getaddrinfo failed: %s", gai_strerror(error) ) ; exit(1) ; }
	struct addrinfo *p = listP ;
	printAddrInfoList( listP ) ;
	/* Search for the first addrinfo record that works for TCP over IPv4 or IPv6.*/
	int i = 0 ;
	for(  ; p != NULL ; p = p->ai_next ) {
		printf("%d\n", i++) ; printAddrInfo(p) ;
		if( (p->ai_family == AF_INET || p->ai_family == AF_INET6)
         && p->ai_socktype == SOCK_STREAM
		 && (p->ai_protocol == 0 || p->ai_protocol == IPPROTO_TCP ) ) {
			break ;
		}
	}
	if( p == NULL ) { printf( "No TCP service found for %s\n", hostName ) ; exit(1) ; }
	printf("A\n") ;
	printSocketAddressInfo(p->ai_addr,  p->ai_addrlen) ;
	memcpy(serverAddressP, p->ai_addr, p->ai_addrlen ) ; // Copy the record.
	*lengthP = p->ai_addrlen ;  // Return the length of the address record.
	printf("B\n") ;
	printSocketAddressInfo(serverAddressP, *lengthP) ;
	freeaddrinfo( listP ) ;
	printf("C\n") ;
	printSocketAddressInfo(serverAddressP, *lengthP) ;
}
