#include<types.h>
#include <lib.h>
#include<addrspace.h>
#include<proc.h>
#include<current.h>
#include<vm.h>
#include <mainbus.h>
#include<coremap.h>

struct coremap_node *coremap;
int memory_size;
void coremap_initialize()
{
        paddr_t ram_size=ram_getsize();
        memory_size=ram_size/PAGE_SIZE;
	int i;
        int coremap_size=(memory_size*sizeof(*coremap))/PAGE_SIZE;
	if((sizeof(coremap)*memory_size)%PAGE_SIZE>0)
        {
            memory_size++;
        }
        paddr_t coremap_addr=ram_stealmem(memory_size);
        coremap=PADDR_TO_KVADDR(coremap_addr);
	int used=coremap_addr/PAGE_SIZE;
	//coremap_size=memory_size;
        for(i=0;i<coremap_size;i++)
        {
                (*(coremap+i)).is_free=true;
                (*(coremap+i)).is_reserved=false;
        }
        //int first=ram_getfirstfree()/PAGE_SIZE;
	for(i=0;i<used;i++)
        {
                (*(coremap+i)).is_reserved=true;
                (*(coremap+i)).is_free=false;
        }
}

void
coremap_allocate(unsigned long npages)
{
        size_t size;
        paddr_t paddr;
	int alloc_count=0,i;
	struct addrspace *cur_as=proc_getas();
	struct node *page_table=cur_as->page_table;
	struct node *traverse=cur_as->page_table;
        for(i=0;i<memory_size;i++)
	{
		if(alloc_count==npages)
			break;
		if(coremap[i].is_free==true&&coremap[i].is_reserved==false)
		{
			struct node *temp;
			temp->index=i;
                        temp->next=NULL;
			if(traverse==NULL)
			{
				traverse=temp;
			}
			else
			{
				traverse->next=temp;
				traverse=traverse->next;
			}
			alloc_count+=1;
			coremap[i].is_free=false;
		}	
	}

        //paddr = firstpaddr;
        //firstpaddr += size;
	curproc->p_addrspace->page_table=page_table;
}



