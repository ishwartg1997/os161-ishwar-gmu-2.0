#include<types.h>
#include<proc.h>

#include<current.h>
#include<syscall.h>
void sys_exit(int exit_code)
{
(void)exit_code;
//spinlock_acquire(&curproc->p_lock);
if(curproc->proclist->count>0)
{
while(curproc->proclist->count>0)
{
struct proclist_node *node=curproc->proclist->root;
if(node->process->is_zombie==true)
proc_destroy(node->process);
node=node->next;
curproc->proclist->count=curproc->proclist->count-1;
}
}
curproc->is_zombie=true;
//spinlock_release(&curproc->p_lock);
thread_exit();
}
