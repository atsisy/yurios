#include "../../include/kernel.h"

void kernel_send(struct Process *proc, u32_t msg){
	queue_push(proc->irq, msg);
}
