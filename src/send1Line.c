/*
 * send1Line.c
 *
 *  Created on: May 29, 2016
 *      Author: theo
 */


#include "send1Line.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

#include "errorExit.h"


void send1Line( int fileDesciptor, char *data ) {
	// Let len be the number of characters up to and including the first newline.
	int len = strstr( data, "\n") - data + 1 ;
	int sent = 0 ;
	// Send the first len characters from data
	while( sent < len ) {
		int result = write( fileDesciptor, data + sent, len - sent ) ;
		if( result < 0 && (errno == EINTR || errno == EAGAIN) ) {
			// 0 bytes were written, so we try again.
			// It might be better to poll in the case of EAGAIN to avoid spinning.
			// See http://stackoverflow.com/questions/10340154/is-there-a-cleaner-way-to-use-the-write-function-reliably
			// and http://pubs.opengroup.org/onlinepubs/009696699/functions/write.html.
			result = 0 ;
		}
		check( result >= 0, "Write failed\n" ) ;
		sent += result ; }
}

