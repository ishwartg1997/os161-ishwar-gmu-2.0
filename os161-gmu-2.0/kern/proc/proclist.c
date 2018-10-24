#include<types.h>
#include<proc.h>
/*Proclist node basically*/
/*Proclist*/


//proclist_node=kmalloc(sizeof(struct proclist*));
int proclist_insert(struct proc_list *proclist,struct proc *process)
{
struct proclist_node *node,*temp_node;
temp_node=kmalloc(sizeof(struct proclist_node*));
if(proclist->root==NULL)
{
temp_node->pid= process->pid;
temp_node->process=process;
temp_node->next=NULL;
proclist->root=temp_node;
return 0;
}
for(node=proclist->root;node->next!=NULL;node=node->next);
pid_t pid= process->pid;
temp_node->process=process;
temp_node->pid=pid;
node->next=temp_node;
temp_node->next=NULL;
proclist->count=proclist->count+1;
return 0;
}
int proclist_search(struct proc_list *proclist,struct proc* process)
{
bool ispresent=false;
struct proclist_node *node,*temp,*temp1;
if(proclist->root->pid==process->pid&&proclist->count==1)
{
proclist->root=NULL;
proclist->count=proclist->count-1;
return 0;
}
if(proclist->root->pid==process->pid&&proclist->count>1)
{
temp=proclist->root;
proclist->root=proclist->root->next;
kfree(&temp);
}
for(node=proclist->root;node!=NULL;node=node->next)
{
temp1=node->next;
if(temp1->pid==process->pid)
{
temp=node->next;
node->next=node->next->next;
kfree(&temp);
proclist->count=proclist->count-1;
return 0;
}
return -1;
}
if(!ispresent)
return -1;
return 0;
}
int proclist_empty(struct proc_list *proclist)
{
struct proclist_node *node;
proclist->count=0;
for(node=proclist->root;node!=NULL;node=node->next)
{

kfree(&node);
}
kfree(&proclist);
return 0;
}
