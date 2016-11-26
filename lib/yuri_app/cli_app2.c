#include "../../include/yuri.h"

int main(void){
  char *str = malloc(1024);
  read(stdin, str, 10);
  write(stdout, str, 10);
  free(str);
  exit();
}
