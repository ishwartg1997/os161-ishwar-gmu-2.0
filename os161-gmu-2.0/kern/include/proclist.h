#include<types.h>
#include<proc.h>

struct proclist_node
{
struct proclist_node *next;
pid_t pid;
struct proc *process;
};

struct proc_list
{
int count;
struct proclist_node *root;
};


//proclist_node=kmalloc(sizeof(struct proclist*));
int proclist_insert(struct proc_list *proclist,struct proc *process);
int proclist_search(struct proc_list *proclist,struct proc* pid);
int proclist_empty(struct proc_list *proclist);
