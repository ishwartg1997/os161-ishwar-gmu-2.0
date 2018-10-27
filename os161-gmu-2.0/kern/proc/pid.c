#include<types.h>
#include<synch.h>
#include<pid.h>
static struct lock *pid_lock;
int pid_counter;
void pid_initialize()
{
pid_lock=lock_create("Process ID");
pid_counter=1;
}
int pid_get()
{
int pid;
lock_acquire(pid_lock);
pid_counter=pid_counter+1;
pid=pid_counter;
lock_release(pid_lock);
return pid_counter;
}

