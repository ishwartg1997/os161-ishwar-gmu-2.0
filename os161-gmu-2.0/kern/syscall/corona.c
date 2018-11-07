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
	//(void)args;
	struct addrspace *new_as,*old_as;
	struct vnode *v;
	char *progn=NULL	;
	//char **args_copy;
	int argc=0,i;
	vaddr_t entrypoint,stackptr;
	if(args!=NULL)
		while(args[argc]!=NULL)
			argc++;
	//kprintf("Argument %d",argc);
	char **args_copy = (char **) kmalloc(sizeof(char *) * (argc+1));
	int initial_size;
	int result;
	as_deactivate();
	new_as=as_create();
	//curproc->as=new_as;
	progn=kstrdup(progname);
	result=vfs_open(progn,O_RDONLY,0,&v);
	kfree(progn);
	if(result)
		return result;
	//bzero(args_copy,sizeof(args_copy));
	for(i=0;i<argc;i++)
	{
		initial_size=strlen(args[i])+1;
		args_copy[i]=(char*)kmalloc(initial_size*sizeof(char));
		//initial_size=strlen(args[i])+1
		copyinstr((userptr_t)args[i],args_copy[i],(strlen(args[i])+1),NULL);
		//copyin(argv,args,sizeof(char*));
	}
	args_copy[i]='\0';
	

	//for(i=0;i<argc;i++)
	//{
	//kprintf("%s",args_copy[i]);
	//}
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
	//kprintf("Stackptr %x here",stackptr);
	result = as_define_stack(new_as, &stackptr);
	if(result)
	{
		return result;
	}
	char **arg_pointers=kmalloc(sizeof(char *)*(argc+1));
	for(i=0;i<(argc);i++)
        {
		size_t len=strlen(args_copy[i])+1;
		//kprintf("%x",stackptr);
		//arg_pointers[i]=(char *)stackptr;	
                stackptr=stackptr-len;
		size_t actual;
		//bzero(*stackptr,(strlen(args_copy[i])+1));
		arg_pointers[i]=(char *)stackptr;
                //args_copy[i][strlen(args_copy)]='\0';
		result=copyoutstr(args_copy[i],(userptr_t)stackptr,len,NULL);
		if(result)
			return result;
        }
	//arg_pointers[i]='\0';

	int mod = ((vaddr_t)stackptr) % 8;
        if (mod) 
	{
              stackptr =stackptr-mod;
        }
	for(i=0;i<argc;i++)
	{	
		stackptr=stackptr-sizeof(char*);
		result=copyout(arg_pointers[i],(userptr_t)stackptr,sizeof(char *));
		if(result)
		return result;
	}
	mod=((vaddr_t)stackptr)%8;
	if (mod)  
        {
              stackptr =stackptr-mod;
        }
;	
	//copyout(argv,kernel_argv,sizeof(kernel_argv));
	//kprintf("\n Curproc ID %d",curproc->pid);
	enter_new_process(argc, (userptr_t)stackptr,
                          (userptr_t)stackptr,stackptr,entrypoint);
	kprintf("\n Curproc ID %d",curproc->pid);
	panic("enter_new_process returned\n");
        return EINVAL;
		
}
