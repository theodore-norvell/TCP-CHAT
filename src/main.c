/*
 * main.c
 *
 *  Created on: Jun 11, 2016
 *      Author: theo
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "client.h"
#include "server.h"

int main( int argc, char **argv ) {

	bool isClient = argc > 1 && 0==strcmp(argv[1], "client" ) ;
	bool isServer = argc > 1 && 0==strcmp( argv[1], "server" ) ;
	if( isClient ) return clientMain( argc, argv ) ;
	else if( isServer ) return serverMain( argc, argv ) ;
	else {
		printf( "Usage: chat client name [host [port]]\n" ) ;
		printf( "       chat server [host [port]] \n" ) ;
	    exit(1) ; }
}
