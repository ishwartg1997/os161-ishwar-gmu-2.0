#include<types.h>
#include<proc.h>
#include<current.h>
#include<syscall.h>
void sys_exit(int exit_code)
{
struct proc *t;
//spinlock_acquire(&curproc->p_lock);
PROCLIST_FORALL(t,curproc->proc_list) {
if(t->is_zombie==true)
proc_destroy(t);
}
curproc->is_zombie=true;
//V(curproc->wait_sem);
//spinlock_release(&curproc->p_lock);
//exit_code=exit_code;
//thread_exit();
}
