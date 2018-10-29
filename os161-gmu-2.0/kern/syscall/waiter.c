#include<types.h>
#include <kern/unistd.h>
#include <lib.h>
#include <proc.h>
#include <current.h>
#include <addrspace.h>
#include <vm.h>
#include <vfs.h>
#include<types.h>
#include <kern/errno.h>
#include<thread.h>
#include<mips/trapframe.h>
#include<syscall.h>
#include<synch.h>

pid_t sys_waitpid(pid_t pid,int *status,int options)
{
	if(options!=0)
		return EINVAL;

	lock_acquire(curproc->mutex);
	struct proc* t;	
	bool my_child_not_yours=false;
	PROCLIST_FORALL(t,curproc->proc_list)
	{
		if(t->pid==pid)
		{
			my_child_not_yours=true;
			break;
		}
	}
	if(my_child_not_yours==false)
	{
		lock_release(curproc->mutex);
		return ECHILD;
	}
	else
	{
		proclist_remove(&curproc->proc_list,t);
		lock_release(curproc->mutex);
	}
	P(t->wait_sem);
	*status=t->p_exitcode;
	pid_free(t->pid);
	return 0;
}
