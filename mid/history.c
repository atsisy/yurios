#include "../include/kernel.h"
#include "../include/sh.h"
#include "../include/string.h"

char **history;
int put_index;
char is_fulled;

void history_init(void){
	int i;
	put_index = 0;
	is_fulled = 0;
	*history = (char *)memory_alloc(memman, 128 << 3);
	for(i = 0;i < 128;i++){
		history[i] = (char *)memory_alloc(memman, 100);
	}
}

void add_history(char *inputed_command){
	int i, length;
	if(put_index == 127){
		put_index = 0;
		is_fulled = 1;
	}
	length = strlen(inputed_command);

	for(i = 0;i < length;i++){
		history[put_index][i] = inputed_command[i];
	}
	history[put_index][i+1] = '\0';
	put_index++;
}

char *get_history(int get_index){
  return history[get_index];
}
