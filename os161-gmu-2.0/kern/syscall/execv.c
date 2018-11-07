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
#include<copyinout.h>
#include <test.h>
#include<synch.h>

int sys_execv(char *progname,char **args)
{
	struct addrspace *new_as,*old_as;
	struct vnode *v;
	char *progn=NULL;
	int align_value;
	int argc=0,i;
	vaddr_t entrypoint,stackptr;
	if(args!=NULL)
		while(1)
			
			{
				if(args[argc]==NULL)
					break;
				else
					argc++;
			}
	char **args_copy = (char **) kmalloc(sizeof(char *) * (argc+1));
	int initial_size;
	int result;
	as_deactivate();
	new_as=as_create();
	progn=kstrdup(progname);
	result=vfs_open(progn,O_RDONLY,0,&v);
	kfree(progn);
	if(result)
		return result;
	for(i=0;i<argc;i++)
	{
		initial_size=strlen(args[i])+1;
		args_copy[i]=(char*)kmalloc(initial_size*sizeof(char));
		copyinstr((userptr_t)args[i],args_copy[i],(strlen(args[i])+1),NULL);
	}
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
	char **arg_pointers=kmalloc(sizeof(char *)*(argc+1));
	for(i=argc-1;i>=0;i--)
        {
		size_t len=strlen(args_copy[i])+1;
                stackptr=stackptr-len;
		size_t actual;
		arg_pointers[i]=(char *)stackptr;
		result=copyoutstr(args_copy[i],(userptr_t)stackptr,len,NULL);
		if(result)
			return result;
        }
	arg_pointers[argc]= NULL;

	stackptr=stackptr-((argc+1)*sizeof(char *));
	
	align_value=(vaddr_t)stackptr%8;
        if (align_value) {
                stackptr=stackptr-align_value;
        }
	copyout(arg_pointers,(userptr_t)stackptr,((argc+1)*sizeof(char *)));

	enter_new_process(argc, (userptr_t)stackptr,
                          (userptr_t)stackptr,stackptr,entrypoint);
	kprintf("\n Curproc ID %d",curproc->pid);
	panic("enter_new_process returned\n");
        return EINVAL;
		
}	
