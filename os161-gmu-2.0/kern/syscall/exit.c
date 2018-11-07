#include<types.h>
#include<proc.h>
#include<current.h>
#include<syscall.h>
void sys_exit(int exit_code)
{
	lock_acquire(curproc->mutex);
	struct proc *t;
	PROCLIST_FORALL(t,curproc->proc_list) 
	{
		if((t->pid)&&(t->is_zombie==true))
			proc_destroy(t);
	}
	curproc->is_zombie=true;
	exit_code=exit_code;
	lock_release(curproc->mutex);
	curproc->p_exitcode=exit_code;
	V(curproc->wait_sem);
	thread_exit();
}
