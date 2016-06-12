/*
 * printSocketInfo.h
 *
 *  Created on: May 29, 2016
 *      Author: theo
 */

#ifndef PRINTSOCKETINFO_H_
#define PRINTSOCKETINFO_H_

#include <netdb.h>
#include <sys/socket.h>

void printSocketAddressInfo( struct sockaddr *sap, socklen_t len ) ;
void printAddrInfo( struct addrinfo *p ) ;
void printAddrInfoList( struct addrinfo *listP ) ;

#endif /* PRINTSOCKETINFO_H_ */
