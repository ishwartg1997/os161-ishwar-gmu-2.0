#include<vm.h>
struct coremap_node{
	bool is_reserved;
	bool is_free;
};
void coremap_initialize(void);
