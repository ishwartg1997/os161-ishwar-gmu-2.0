#include<types.h>
#include<proc.h>
#include<current.h>
#include<syscall.h>
void sys_exit(int exit_code)
{
	lock_acquire(curproc->mutex);
	struct proc *t;
	curproc->is_zombie=true;
	exit_code=exit_code;
	lock_release(curproc->mutex);
	curproc->p_exitcode=exit_code;
	V(curproc->wait_sem);
	thread_exit();
}
