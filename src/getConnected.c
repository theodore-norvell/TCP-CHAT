/*
 * getConnected.c
 *
 *  Created on: May 29, 2016
 *      Author: theo
 */

#include <sys/socket.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "getConnected.h"
#include "printSocketInfo.h"
#include "errorExit.h"

int getConnected( int family, struct sockaddr *sockaddrP, socklen_t length ) {
    // Create a new socket.
    int sockFD = socket( family, SOCK_STREAM, 0 ) ;
    check( sockFD >= 0, "socket failed" ) ;
    // TODO implement an exponential back off -- see Stevens
    printf("Connecting to ") ; printSocketAddressInfo( sockaddrP, length ) ;
    int err = connect( sockFD, sockaddrP, length ) ;
    check( err==0, "connect failed." ) ;
    printf("Connection made.\n") ;
    printf("Connected to ") ; printSocketAddressInfo( sockaddrP, length ) ;

    length = sizeof( *sockaddrP ) ;
    getsockname( sockFD, sockaddrP, &length ) ;
    printf("Connected to ") ; printSocketAddressInfo( sockaddrP, length ) ;

    length = sizeof( *sockaddrP ) ;
    getpeername( sockFD, sockaddrP, &length ) ;
    printf("Connected to ") ; printSocketAddressInfo( sockaddrP, length ) ;

    return sockFD ;
}
