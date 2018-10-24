#include<types.h>
#include<syscall.h>
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
#include<synch.h>
/*Each child process needs a copy of the current trapframe, process structure and runnable threads*/
//static struct semaphore *lock_sem;
//int counter=0;
//lock_sem=sem_create("Lock",1)
int sys_fork(struct trapframe *tf,pid_t *retval)
{
//lock_sem=sem_create("Lock",1);
int process_error,thread_error,address_error;
struct trapframe *newtf;
newtf=kmalloc(sizeof(struct trapframe));
memcpy(newtf,tf,sizeof(struct trapframe ));
struct addrspace *as=proc_getas();
struct addrspace **new_as;
new_as=kmalloc(sizeof(struct addrspace*));
address_error=as_copy(as,new_as);
if(address_error)
{
kfree(newtf);
return address_error;
}
struct proc **new_proc=kmalloc(sizeof(struct proc*));
process_error=proc_fork(new_proc);
if(process_error)
{
kfree(new_as);
kfree(newtf);
return process_error;
}
//KASSERT(proc_getas() == NULL);
(*new_proc)->p_addrspace= *new_as;
thread_error=thread_fork("thread",*new_proc,enter_forked_process ,newtf,0);
if(thread_error)
{
kfree(newtf);
kfree(new_proc);
kfree(new_as);
return thread_error;
}
*retval=(*new_proc)->pid;
return 0;
}

int sys_getpid(int *retval)
{

*retval= curproc->pid;
return 0;
}


































