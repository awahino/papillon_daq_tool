#include "include.h"

void test() {

  std::ifstream fin;
  fin.open("./hoge.dat");

  display my_display;
  
  /*std::string line;
  int nct = 0;
  while (getline(std::cin, line)) {
    unsigned int lnum=0;
	unsigned int x[4]={0};

	// assuming the format as "od -H" output
	std::sscanf(line.c_str(),"%d %x %x %x %x",&lnum,&(x[0]),&(x[1]),&(x[2]),&(x[3]));

	for (int i=0; i<4; i++) {
      if (nct==N_MAX_LEN) {
      	my_display.ShowPlot();

      	nct = 0;
      }

      my_display.m_data[nct++] = x[i];
	}
  }

  if(nct==N_MAX_LEN) my_display.ShowPlot();*/

  dislpay.do_it(0);

}

