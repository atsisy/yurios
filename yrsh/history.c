#include "../include/kernel.h"
#include "../include/sh.h"
#include "../include/yrfs.h"
#include "../include/sysc.h"

i32_t history_fd;

/*
 *=======================================================================================
 *
 *=======================================================================================
 */
i32_t history_init(void){
	if((history_fd = do_open("history", __O_CREAT__ | __O_RDONLY__)) != -1)
		return history_fd;
	else
		return -1;
}

void add_history(char *inputed_command){
	if((history_fd = do_open("history", __O_CREAT__ | __O_RDONLY__)) != -1){
		fadd(history_fd, inputed_command);
		fadd(history_fd, "\n");
	}
}
