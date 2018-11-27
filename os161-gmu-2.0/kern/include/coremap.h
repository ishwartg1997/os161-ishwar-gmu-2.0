#include<vm.h>
#include<spinlock.h>
struct coremap_node{
	bool is_reserved;
	bool is_free;
	int chunk_size;
};

struct coremap_node *coremap;
int memory_size;

struct spinlock *coremap_lock;

void coremap_initialize(void);
paddr_t coremap_allocate(unsigned long npages);

