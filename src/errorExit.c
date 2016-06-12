/*
 * errorExit.c
 *
 *  Created on: Jun 1, 2016
 *      Author: theo
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "errorExit.h"

void check( bool condition, char* message ) {
    if( !condition ) {
        printf( "%s: %s\n", message, strerror( errno ) ) ;
        exit( 1 ) ; }
}

void errorExit( char *message ) {
	printf( "%s\n", message ) ;
	exit( 1 ) ;
}
