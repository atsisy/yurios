#include "../../include/yuri.h"

int main(void){

      char str[1024];

      read(stdin, str, 10);
	sleep(1000);
      write(stdout, str, 5);

      exit();
}
