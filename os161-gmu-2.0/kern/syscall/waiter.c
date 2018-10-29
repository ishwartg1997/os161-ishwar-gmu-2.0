#include<types.h>
#include <kern/unistd.h>
#include <lib.h>
#include <proc.h>
#include <current.h>
#include <addrspace.h>
#include <vm.h>
#include <vfs.h>
#include <kern/errno.h>
#include<thread.h>
#include<mips/trapframe.h>
#include<syscall.h>
#include<synch.h>

int sys_waitpid(pid_t pid,int *status,int options)
{
struct proc* t;	
bool my_child_not_yours=false;
PROCLIST_FORALL(t,curproc->proc_list)
{
if(t->pid==pid)
my_child_not_yours=true;
}
//if(my_child_not_yours==false)
//{
//return ECHILD;
//}
//P(t->wait_sem);
return 0;
}
