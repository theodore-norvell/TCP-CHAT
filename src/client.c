/*
 ============================================================================
 Name        : TCP-CHAT-APP.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

#include "findAddress.h"
#include "getConnected.h"
#include "send1Line.h"
#include "errorExit.h"

static const int BUFFERSIZE = 1024 ;

void chat( int sockFD, char *name ) {
	send1Line( sockFD, name ) ;
	int fromUserSize = 0 ;
	char fromUser[BUFFERSIZE] ;
	int fromServerSize = 0 ;
	char fromServer[BUFFERSIZE] ;

	// Send one line to tell the server the name of the client.
	sprintf( fromUser, "%s\n", name ) ;
	send1Line( sockFD, fromUser ) ;

	while( 1 ) {
		// Wait until there is some input or a file is closed.
		fd_set setOfFileDescriptors ;
		FD_ZERO( &setOfFileDescriptors ) ;
		FD_SET( 0, &setOfFileDescriptors ) ;
		FD_SET( sockFD, &setOfFileDescriptors ) ;
		int result = select(sockFD+1, &setOfFileDescriptors, NULL, NULL, NULL) ;
		check( result >= 0, "select failed" ) ;
		if( FD_ISSET( 0, &setOfFileDescriptors ) ) {
			// Read one byte
			result = read( 0, fromUser+fromUserSize, 1) ;
			check( result >= 0, "read failed") ;
			if( result == 0 ) { // end of user input.
				printf("Goodbye\n") ;
				break ; }
			assert( result == 1 ) ;
			fromUserSize += result ;
			// To avoid overflow we add a newline if the buffer is getting full.
			if(fromUserSize == BUFFERSIZE-1 ) { fromUser[ fromUserSize++ ] = '\n' ; }
			if( fromUser[ fromUserSize-1 ] == '\n' ) {
				send1Line( sockFD, fromUser ) ;
				fromUserSize = 0 ; }
		}
		if( FD_ISSET( sockFD, &setOfFileDescriptors ) ) {
			// Read one byte
			result = read( sockFD, fromServer+fromServerSize, 1) ;
			if( result < 0 ) { printf("read failed\n") ; exit(1) ; }
			if( result == 0 ) { // end of user input.
				printf("Server has closed its socket. Goodbye\n") ;
				break ; }
			assert( result == 1 ) ;
			fromServerSize += result ;
			// To avoid overflow we add a newline if the buffer is getting full.
			if(fromServerSize == BUFFERSIZE-1 ) { fromServer[ fromServerSize++ ] = '\n' ; }
			if( fromServer[ fromServerSize-1 ] == '\n' ) {
				fromServer[ fromServerSize-1 ] = 0 ;
				printf("%s\n", fromServer ) ;
				fromServerSize = 0 ; }
		}
	}
}

int clientMain( int argc, char **argv ) {

	char *name, *host, *port ;
	if( argc < 3 || argc > 5) { printf( "Usage: chat client name [host [port]]\n" ) ; exit( 1 ) ; }
	if( argc < 5 ) port = "49123" ; else port = argv[4] ;
	if( argc < 4 ) host = "localhost" ; else host = argv[3] ;
	name = argv[2] ;

    struct sockaddr_storage socketAddressStorage ;
    struct sockaddr *socketAddressP = (struct sockaddr*) &socketAddressStorage ;
    socklen_t length ;
    // TODO The next two lines try connecting to the first suitable address found.
    // It might be better to try connecting to each one until a suitable one is found.
    findAddress( socketAddressP, &length, host, port ) ;
    int sockFD = getConnected( socketAddressP->sa_family, socketAddressP, length ) ;

    chat( sockFD, name ) ;
	close( sockFD ) ;
    return 0 ;
}
