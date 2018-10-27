#include<stdio.h>
int sys_waitpid(retval)
{
bool my_child_not_yours=false;
PROCLIST_FORALL(t,curproc->proc_list)
{
if(t->pid==pid)
my_child_not_yours=true;
}
if(my_child_not_yours==false)
{
retval=;
return 0;
}
if(my_child_not_yours->is_zombie==false)
wchan_sleep(wc,proc);
}
