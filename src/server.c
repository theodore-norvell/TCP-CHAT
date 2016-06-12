/*
 * chatServer.c
 *
 *  Created on: May 29, 2016
 *      Author: theo
 */


#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>

#include "findAddress.h"
#include "errorExit.h"
#include "send1Line.h"
#include "printSocketInfo.h"

const static int BUFFERSIZE = 1024 ;

typedef struct Client {
	char clientName[ BUFFERSIZE ] ;
	int state ; // 0 -- inactive, 1 -- reading name, 2 -- in the conversation.
	int clientFD ; // File descriptor for the client
	char buffer[ BUFFERSIZE ] ;
	int count ;  // How many characters are in the buffer.
} Client ;

const static int CLIENTMAX = 20 ;
Client client[ CLIENTMAX ] ;

void addNewClient( newClientFD ) {
	int i ;
	for( i = 0 ; i < CLIENTMAX ; ++i ) if( client[i].state == 0 ) break ;
	if( i < CLIENTMAX ) {
		client[i].state = 1 ;
		client[i].clientFD = newClientFD ;
		client[i].count = 0 ;
	} else {
		send1Line( newClientFD, "Too many users\n" ) ;
		int err = close( newClientFD ) ;
		check( err == 0, "close failed" ) ;
	}
}

void broadcast( char * message ) {
	for( int i = 0 ; i < CLIENTMAX ; ++i ) {
		if( client[i].state == 2 ) {
			send1Line( client[i].clientFD, message ) ;
		}
	}
}

void broadcastFrom(int clientNo ) {
	char message[2*BUFFERSIZE+10] ;
	sprintf( message, "[%s] %s\n", client[clientNo].clientName, client[clientNo].buffer ) ;
	broadcast( message ) ;
}

void broadcastJoin(int clientNo ) {
	char message[BUFFERSIZE+35] ;
	sprintf( message, "[%s has joined the conversation]\n", client[clientNo].clientName ) ;
	broadcast( message ) ;
}

void broadcastLeave(int clientNo ) {
	char message[BUFFERSIZE+35] ;
	sprintf( message, "[%s has left the conversation]\n", client[clientNo].clientName ) ;
	broadcast( message ) ;
}

void chatServer( int listeningFD ) {
	while( 1 ) {
		// Make a set representing the listeningFD and all active clients
		fd_set fileDescriptors ;
		FD_ZERO( &fileDescriptors ) ;
		int maxFD = listeningFD ;
		FD_SET( listeningFD, &fileDescriptors ) ;
		for( int i = 0 ; i < CLIENTMAX ; ++i ) {
			if( client[i].state == 1 || client[i].state == 2 ) {
				FD_SET( client[i].clientFD, &fileDescriptors ) ;
				if( client[i].clientFD > maxFD ) maxFD = client[i].clientFD ; } }

		printf( "About to select\n") ;
		// Wait until there is input to deal with
		int err = select( maxFD+1, &fileDescriptors, NULL, NULL, NULL ) ;
		check( err >= 0, "select has failed" ) ;
		check( err > 0, "select has timed out" ) ;
		printf( "Back from select\n") ;
		// Is there a new connection request?
		if( FD_ISSET( listeningFD, &fileDescriptors ) ) {
			struct sockaddr socketAddress ;
			socklen_t length ;
			int newClientFD = accept(listeningFD, &socketAddress, &length ) ;
			check( newClientFD >= 0, "accept failed" ) ;
			printf("Accepting new client\n") ;
			addNewClient( newClientFD ) ; }
		// Process input from all clients that have sent data or closed the socket.
		for( int i = 0 ; i < CLIENTMAX ; ++i ) {
			if( FD_ISSET( client[i].clientFD, &fileDescriptors ) ) {
				int count = read( client[i].clientFD, client[i].buffer+client[i].count, 1 ) ;
				check( count >= 0, "read failed" ) ;
				// Did the client close the connection?
				if( count == 0 ) {
					broadcastLeave( i ) ;
					client[i].state = 0 ;
					err = close( client[i].clientFD ) ;
					check( err == 0, "close failed" ) ;
					printf( "Closed client %d\n", i ) ; }
				else {
					assert( count == 1 ) ;
					printf( "Read a %c from client %d.\n", client[i].buffer[client[i].count], i) ;
					client[i].count += 1 ;
					if(client[i].count == BUFFERSIZE-1 ) {
						client[i].buffer[ client[i].count++ ] = '\n' ; }
					// Check for newline
					if( client[i].buffer[ client[i].count-1 ] == '\n' ) {
						// Replace newline with a 0.
						client[i].buffer[ client[i].count-1 ] = 0 ;
						if( client[i].state == 1 ) {
							strcpy( client[i].clientName, client[i].buffer ) ;
							client[i].state = 2 ;
							printf( "Client %d is named %s\n.", i, client[i].clientName ) ;
							broadcastJoin( i ) ;
						} else {
							printf( "Client %d has completed a line: %s\n.", i, client[i].buffer ) ;
							broadcastFrom( i ) ; }
						client[i].count = 0 ; }
				}
			}
		}
	}
}


int serverMain( int argc, char **argv ) {
    printf( "Running server\n" ) ;
	char *port, *host ;
	if( argc > 4) { printf( "Usage: chat server [host [port]]\n" ) ; exit( 1 ) ; }
	char hostName[256] ;
	if( argc < 3 ) {
		gethostname( hostName, 256) ;
		host = hostName ; }
	else host = argv[2] ;
	if( argc < 4 ) port = "49123" ; else port = argv[3] ;

    printf( "Host is %s, port is %s\n", host, port) ;

	struct sockaddr_storage socketAddressStorage ;
	struct sockaddr *socketAddressP = (struct sockaddr*) &socketAddressStorage ;
	socklen_t length ;
    findAddress( socketAddressP, &length, host, port ) ;

    printf( "Socket address is\n" ) ;
    printSocketAddressInfo( socketAddressP, length ) ;

    int sockFD = socket( socketAddressP->sa_family, SOCK_STREAM, 0 ) ;
    check( sockFD >= 0, "socket failed" ) ;
    int err = bind( sockFD, socketAddressP, length ) ;
    check( err == 0,  "bind failed" ) ;
    printf( "bind ok\n" ) ;
    err = listen( sockFD, 10 ) ;
    check( err >= 0, "listen failed" ) ;
    printf( "listen ok\n" ) ;
    chatServer( sockFD ) ;
	close( sockFD ) ;
    return 0 ;
}
