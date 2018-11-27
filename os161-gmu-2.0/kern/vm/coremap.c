#include<types.h>
#include <lib.h>
#include<addrspace.h>
#include<proc.h>
#include<current.h>
#include<vm.h>
#include <mainbus.h>
#include<coremap.h>

int coremap_curr;
int used;
int coremap_size;
paddr_t ram_size;
paddr_t coremap_addr;
//struct spinlock *coremap_lock;

void coremap_initialize()
{
        paddr_t ram_size=ram_getsize();
        memory_size=ram_size/PAGE_SIZE;
	int i;
        coremap_size=(((memory_size*sizeof(struct coremap_node)) + PAGE_SIZE)/PAGE_SIZE);
        coremap_addr=ram_stealmem(coremap_size);
        coremap=PADDR_TO_KVADDR(coremap_addr);
	used=coremap_addr/PAGE_SIZE;
	if((coremap_addr%PAGE_SIZE)>0)
		used++;
        for(i=0;i<used;i++)
        {
                (*(coremap+i)).is_reserved=true;
                (*(coremap+i)).is_free=false;
		(*(coremap+i)).chunk_size=0;
        }

	for(i=used;i<(memory_size-coremap_size);i++)
        {
                (*(coremap+i)).is_free=true;
                (*(coremap+i)).is_reserved=false;
		(*(coremap+i)).chunk_size=0;
        }
	coremap_curr=used;
}

paddr_t
coremap_allocate(unsigned long npages)
{
	int alloc_count=0,i;
	
	for(i=used;i<memory_size;i++)
	{
		
		if(alloc_count>=npages)
		{
		 	int chunk_allocation=npages;
			for(int j=(i-npages);j<i;j++)
			{	
				coremap[j].chunk_size=chunk_allocation;
				coremap[j].is_free=false;
				chunk_allocation-=1;
			}
			 return ((paddr_t)((i-npages+1)*PAGE_SIZE));
			
		}
		if(coremap[i].is_free==false)
                {
                        alloc_count=0;
                }

		if(coremap[i].is_free==true)
		{
			
			alloc_count++;
		}
	}

	return 0;
}



