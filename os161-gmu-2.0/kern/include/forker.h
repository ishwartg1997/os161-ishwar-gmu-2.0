#include<forker.h>
int sys_fork(struct trapframe *tf,pid_t *retval);
int sys_getpid(int *retval);
