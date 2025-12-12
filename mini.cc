#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <poll.h>
#include "rbcp_operator.h"
#include "papillon.h"

//
void do_daq(const std::string& ip_str, int port, int nevent, const std::string& ofname, bool display_mode) {
    std::cout << "[### do_daq() ###]" << std::endl;
    //
    bool with_storage = false;
    std::ofstream ofile;
    if(ofname != std::string("")) {
	ofile.open(ofname.c_str(), std::ios::out|std::ios::binary);
	if(!ofile.good()) {
	    std::cout << "Can't open " << ofname.c_str() << std::endl;
	    return;
	}
	  with_storage = true;
    }

    //
    time_t timer;
    papillon ro_data(ip_str.c_str(), "24"); // ip, port

    fprintf(stderr, "now we start read data\n");
    for(int ievent=0; ievent<nevent; ievent++) {
	  /*try {
	    ro_data.Read();
	  } catch (char *msg) {
	    fputs(msg, stdout);
	  }*/
      ro_data.Read();
	  time(&timer);

	  //
	  if(with_storage) {
	    papillon_event & e = ro_data.Last();
	    ofile.write((char*) &(e.header()), sizeof(e.header()));
		
	    int len = e.len();
	    unsigned char* p = e.body();
		
	    ofile.write((char*)p, len);
	    if(!display_mode) {
		  printf("%s : ",ctime(&timer));
		  ro_data.Last().header().show();
	    }
	  }

	  //
	  if(display_mode) {
	    ro_data.Last().raw_show();
	  }
    }

    if(ofile.good()) {
	  ofile.close();
    }


}


void show_usage(int argc, char* argv[]) {
    std::cout << "Usage : " << argv[0] << " -i <ip addr> -p <port> -n <nevent> -o <output fname> [-d (display mode)]"
	      << std::endl;
}


//
int main( int argc, char* argv[] ) {
    //
    std::string ip_str("192.168.10.16");
    std::string ofname("");

    int c=0;
    int port=24;
    int nevent=1e6;
    bool display_mode = false;
    while( (c=getopt(argc,argv,"hi:p:do:n:")) != -1 ) {
        switch(c) {
        case 'i':
            ip_str = std::string(optarg);
            break;
        case 'o':
            ofname = std::string(optarg);
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 'n':
            nevent = atoi(optarg);
            break;
        case 'd':
	        display_mode = true;
            break;
	    case 'h':
        default:
            show_usage(argc,argv);
            return -1;
            break;
        }
    }

    do_daq(ip_str,port,nevent,ofname,display_mode);
    

    return 0;
}
