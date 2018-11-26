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
int coremap_curr;
int used;
int coremap_size;
paddr_t ram_size;
paddr_t coremap_addr;
void coremap_initialize()
{
        paddr_t ram_size=ram_getsize();
        memory_size=ram_size/PAGE_SIZE;
	int i;
        coremap_size=(((memory_size*sizeof(struct coremap_node)) + PAGE_SIZE)/PAGE_SIZE);
	if((((memory_size*sizeof(struct coremap_node)) + PAGE_SIZE)%PAGE_SIZE)>0)
        {
            coremap_size++;
        }
        coremap_addr=ram_stealmem(coremap_size);
        coremap=PADDR_TO_KVADDR(coremap_addr);
	used=coremap_addr/PAGE_SIZE;
	if((coremap_addr%PAGE_SIZE)>0)
		used++;
	//coremap_size=memory_size;
        for(i=0;i<used;i++)
        {
                (*(coremap+i)).is_reserved=true;
                (*(coremap+i)).is_free=false;
        }

	for(i=used;i<(memory_size-coremap_size);i++)
        {
                (*(coremap+i)).is_free=true;
                (*(coremap+i)).is_reserved=false;
        }
        //int first=ram_getfirstfree()/PAGE_SIZE;
	/*for(i=0;i<used;i++)
        {
                (*(coremap+i)).is_reserved=true;
                (*(coremap+i)).is_free=false;
        }*/
	coremap_curr=used;
}

paddr_t
coremap_allocate(unsigned long npages)
{
        //size_t size;
        //paddr_t paddr;
	int alloc_count=0,i;
	//struct addrspace *cur_as=proc_getas();
	//struct node *page_table=cur_as->page_table;
	//struct node *traverse=cur_as->page_table;
	//int return_value=((coremap_curr)*PAGE_SIZE);
	//coremap_curr=coremap_curr+npages;
	//if(coremap_curr>=+memory_size)
	//	coremap_curr=used;
	//return ((paddr_t)(return_value));
       	 
	for(i=used;i<memory_size;i++)
	{
		
		if(alloc_count>=npages)
		{
			for(int j=(i-npages);j<i;j++)
				coremap[j].is_free=false;
			 //used=i;
			 return ((paddr_t)((i-npages)*PAGE_SIZE));
			
			//return i;
		}
		if(coremap[i].is_free==false)
                {
                        alloc_count=0;
                }

		if(coremap[i].is_free==true)
		{
			/*struct node *temp;
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
			}*/
			alloc_count+=1;
			coremap[i].is_free=false;
		}
		//if(coremap[i].is_free==false)
		//{
		//	alloc_count=0;
		//}
	}

        //paddr = firstpaddr;
        //firstpaddr += size;
	//curproc->p_addrspace->page_table=page_table;
	//return ((i-npages)*PAGE_SIZE);
	return 0;
}



