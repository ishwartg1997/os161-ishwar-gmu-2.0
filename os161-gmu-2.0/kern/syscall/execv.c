#include <types.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <kern/unistd.h>
#include <lib.h>
#include <proc.h>
#include <current.h>
#include <addrspace.h>
#include <vm.h>
#include <vfs.h>
#include <openfile.h>
#include <filetable.h>
#include <syscall.h>
#include <test.h>
#include<synch.h>

int sys_execv(char *progname,char **args)
{
	(void)args;
	struct addrspace *new_as,*old_as;
	struct vnode *v;
	char *progn;
	vaddr_t entrypoint,stackptr;
	int result;
	as_deactivate();
	new_as=as_create();
	//curproc->as=new_as;
	progn=kstrdup(progname);
	result=vfs_open(progn,O_RDONLY,0,&v);
	kfree(progn);
	if(result)
		return result;
	old_as=proc_setas(new_as);
	as_destroy(old_as);
	as_activate();
	result=load_elf(v,&entrypoint);
	if(result)
	{
		vfs_close(v);
		return result;
	}
	vfs_close(v);
	result = as_define_stack(new_as, &stackptr);
	if(result)
	{
	return result;
	}
	//kprintf("\n Curproc ID %d",curproc->pid);
	
	enter_new_process(0 /*argc*/, NULL /*userspace addr of argv*/,
                          NULL /*userspace addr of environment*/,
                          stackptr, entrypoint);
	kprintf("\n Curproc ID %d",curproc->pid);
	panic("enter_new_process returned\n");
        return EINVAL;
		
}
