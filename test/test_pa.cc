#include <iostream>
#include <string>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <poll.h>
#include "papillon.h"

//
int 
main( int argc, char* argv[] )
{
    //
    time_t timer;
    papillon pa;
    pa.Setup("192.168.10.16", "24"); // ip, port
    fprintf(stderr, "now we start read data\n");
    while ( 1 ) {
	while (!pa.HasEvent()) {
	    poll(NULL, 0, 1); /* hopefully 1ms */
	}
	

	try {
	    pa.Read();
	} catch (char *msg) {
	    fputs(msg, stdout);
	}
	time(&timer);

	//ca.Last().header().show();
	//ca.Last().show();

	papillon_event& o = pa.Last();

	int bytes        = o.len();  // body length in bytes
	int header_bytes = sizeof(o.header());
	int header[7];
	o.header().Fill(header);
	
	char* dst[100000];
	memcpy(dst, (char*)header, header_bytes);
	memcpy(dst+7, o.body(), bytes);

	int* t = (int*)dst;
	int* s = (int*)o.body();
	for(int i=0; i<7; i++) {
	    //for(int i=0; i<10; i++) {
	    //printf("%d %x %x\n", i, t[i],s[i]);
	    printf("%d %x\n", i, t[i]);
	}
	for(int i=0; i<5; i++) {
	    printf("%d %x %x\n", i, t[7+i],s[i]);
	}

	std::cout << "====" << std::endl;
    }

    return 0;
}
