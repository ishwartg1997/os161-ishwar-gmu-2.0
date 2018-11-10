#include<types.h>
#include<synch.h>
#include<limits.h>
#include<array.h>
#include<pid.h>

static struct lock *pid_lock;
bool pid_allocator[PID_MAX];
int start_pid;
int proc_counter;
void pid_initialize()
{
	proc_counter=0;
	pid_lock=lock_create("Process ID");
	for(int i=0;i<PID_MAX;i++)
	pid_allocator[i]=false;
	start_pid=PID_MIN;
}

int pid_get(pid_t *ret)
{
	lock_acquire(pid_lock);
	if(proc_counter<(PID_MAX-2))
	{
	int i=start_pid;
	while(pid_allocator[i]==true)
	{
		i=i+1;
		if(i==PID_MAX)
			i=PID_MIN;
	}
	pid_allocator[i]=true;
	*ret=i;
	proc_counter++;
	//start_pid=i;
	}
	else
	{
	*ret=-1;
	}
	lock_release(pid_lock);
	return 0;
}

int pid_free(pid_t pid)
{
	lock_acquire(pid_lock);
	pid_allocator[pid]=false;
	proc_counter--;
	lock_release(pid_lock);
	return 0;
}
