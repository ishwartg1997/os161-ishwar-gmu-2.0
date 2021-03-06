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

int sys_fork(struct trapframe *tf,pid_t *retval)
{

	int process_error,thread_error,address_error;
	struct trapframe *newtf=kmalloc(sizeof(*tf));
	if(newtf==NULL)
		return ENOMEM;
	memcpy(newtf,tf,sizeof(*tf));
	struct addrspace *as=proc_getas();
	struct addrspace **new_as=kmalloc(sizeof(struct addrspace*));
	address_error=as_copy(as,new_as);
	if(address_error)
	{
		kfree(newtf);
		return address_error;
	}
	struct proc **new_proc=kmalloc(sizeof(struct proc*));
	process_error=proc_fork(new_proc);
	if((*new_proc)->pid==-1)
		return EAGAIN;
	if(process_error)
	{
		kfree(new_as);
		kfree(newtf);
		kfree(new_proc);
		return process_error;
	}
	(*new_proc)->p_addrspace= *new_as;
	proclist_addtail((&curproc->proc_list),(*new_proc));
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


































