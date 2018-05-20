#include <stdio.h>
#include "web_server.h"
#include "main.h"

/************************************
*  Main
************************************/

#ifndef TEST
int main ()
{
  printf("start daemon\n\n");
  start_daemon();
}
#endif
