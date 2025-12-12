/*************************************************
*                                                *
* SiTCP Bus Control Protocol                     *
* Header file                                    *
*                                                *
* 2006/03/01 Tomohisa Uchida                     *
*                                                *
*************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

struct bcp_header{
  unsigned char type;
  unsigned char command;
  unsigned char id;
  unsigned char length;
  unsigned int address;
};
