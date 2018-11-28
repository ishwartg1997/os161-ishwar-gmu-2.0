/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <spl.h>
#include <spinlock.h>
#include<coremap.h>
#include <proc.h>
#include <current.h>
#include <mips/tlb.h>
#include <addrspace.h>
#include <vm.h>

/*
 * Dumb MIPS-only "VM system" that is intended to only be just barely
 * enough to struggle off the ground. You should replace all of this
 * code while doing the VM assignment. In fact, starting in that
 * assignment, this file is not included in your kernel!
 *
 * NOTE: it's been found over the years that students often begin on
 * the VM assignment by copying dumbvm.c and trying to improve it.
 * This is not recommended. dumbvm is (more or less intentionally) not
 * a good design reference. The first recommendation would be: do not
 * look at dumbvm at all. The second recommendation would be: if you
 * do, be sure to review it from the perspective of comparing it to
 * what a VM system is supposed to do, and understanding what corners
 * it's cutting (there are many) and why, and more importantly, how.
 */

/* under dumbvm, always have 72k of user stack */
/* (this must be > 64K so argument blocks of size ARG_MAX will fit) */
#define DUMBVM_STACKPAGES    18

/*
 * Wrap ram_stealmem in a spinlock.
 */
static struct spinlock stealmem_lock = SPINLOCK_INITIALIZER;

void
vm_bootstrap(void)
{
	/* Do nothing. */
}

static
paddr_t
getppages(unsigned long npages)
{
	paddr_t addr;
	spinlock_acquire(&stealmem_lock);
	addr=coremap_allocate(npages);
	spinlock_release(&stealmem_lock);
	return addr;
}

/* Allocate/free some kernel-space virtual pages */
vaddr_t
alloc_kpages(unsigned npages)
{
	paddr_t pa;
	pa = getppages(npages);
	if (pa==0) {
		return 0;
	}
	return PADDR_TO_KVADDR(pa);
}

void
free_kpages(vaddr_t addr)
{
	/* nothing - leak the memory. */
	paddr_t physical_address=addr-MIPS_KSEG0;
	int page_number=physical_address/PAGE_SIZE;
	if(physical_address%PAGE_SIZE>0)
		page_number++;
	int free_limit=page_number+coremap[page_number].chunk_size;
	for(int i=page_number;i<free_limit;i++)
	{
	coremap[i].is_free=true;
	coremap[i].chunk_size=0;
	if(coremap[i].chunk_size==1)
	{
	break;
	}
	}
	
}

void
vm_tlbshootdown_all(void)
{
	panic("dumbvm tried to do tlb shootdown?!\n");
}

void
vm_tlbshootdown(const struct tlbshootdown *ts)
{
	(void)ts;
	panic("dumbvm tried to do tlb shootdown?!\n");
}

int
vm_fault(int faulttype, vaddr_t faultaddress)
{
	vaddr_t vbase, vtop,stackbase, stacktop;
	paddr_t paddr;
	int i;
	uint32_t ehi, elo;
	struct addrspace *as;
	int spl;

	faultaddress &= PAGE_FRAME;

	switch (faulttype) {
	    case VM_FAULT_READONLY:
		return EPERM;
	    case VM_FAULT_READ:
	    case VM_FAULT_WRITE:
		break;
	    default:
		return EINVAL;
	}

	if (curproc == NULL) {
		/*
		 * No process. This is probably a kernel fault early
		 * in boot. Return EFAULT so as to panic instead of
		 * getting into an infinite faulting loop.
		 */
		return EFAULT;
	}

	as = proc_getas();
	if (as == NULL) {
		/*
		 * No address space set up. This is probably also a
		 * kernel fault early in boot.
		 */
		return EFAULT;
	}

	/* Assert that the address space has been set up properly. */
	/*KASSERT(as->page_table[i]->as_vbase != 0);
	KASSERT(as->page_table[i]->as_pbase != 0);
	KASSERT(as->page_table[i]->as_npages != 0);
	KASSERT(as->page_table[i]->as_vbase != 0);
	KASSERT(as->page_table[i]->as_pbase != 0);
	KASSERT(as->page_table[i]->as_npages != 0);
	KASSERT(as->page_table[i]->as_stackpbase != 0);
	KASSERT((as->page_table[i]->as_vbase & PAGE_FRAME) == as->page_table[i]->as_vbase);
	KASSERT((as->page_table[i]->as_pbase & PAGE_FRAME) == as->page_table[i]->as_pbase);
	KASSERT((as->page_table[i]->as_vbase & PAGE_FRAME) == as->page_table[i]->as_vbase);
	KASSERT((as->page_table[i]->as_pbase & PAGE_FRAME) == as->page_table[i]->as_pbase2);
	KASSERT((as->page_table[i]->as_stackpbase & PAGE_FRAME) == as->page_table[i]->as_stackpbase);*/
	stackbase = USERSTACK - DUMBVM_STACKPAGES * PAGE_SIZE;
        stacktop = USERSTACK;
	int read_only=0;
	for(int i=0;i<as->no_of_segments;i++)
	{
	vbase = as->page_table[i].as_vbase;
	vtop = vbase + as->page_table[i].as_npages * PAGE_SIZE;
	//vbase = as->page_table[i].as_vbase;
	//vtop = vbase + as->page_table[i].as_npages * PAGE_SIZE;
	//stackbase = USERSTACK - DUMBVM_STACKPAGES * PAGE_SIZE;
	//stacktop = USERSTACK;
	if (faultaddress >= vbase && faultaddress < vtop) {
		paddr = (faultaddress - vbase) + as->page_table[i].as_pbase;
		if((as->page_table[i].final_privilege==4||as->page_table[i].final_privilege==5))
			read_only=1;
		break;
	}
	else if (faultaddress >= stackbase && faultaddress < stacktop) {
		paddr = (faultaddress - stackbase) + as->as_stackpbase;
		}
	}

	/* make sure it's page-aligned */
	KASSERT((paddr & PAGE_FRAME) == paddr);

	/* Disable interrupts on this CPU while frobbing the TLB. */
	spl = splhigh();

	for (i=0; i<NUM_TLB; i++) {
		tlb_read(&ehi, &elo, i);
		if (elo & TLBLO_VALID) {
			continue;
		}
		ehi = faultaddress;
		elo = paddr |TLBLO_VALID;
		if(!read_only)
			elo=elo|TLBLO_DIRTY;
		tlb_write(ehi, elo, i);
		splx(spl);
		return 0;
	
	}
	int val=tlb_probe(ehi,elo);
	if(val<0)
		tlb_random(ehi,elo);
	else
		tlb_write(TLBLO_INVALID(),TLBLO_INVALID(),val);

	//kprintf("dumbvm: Ran out of TLB entries - cannot handle page fault\n");
	splx(spl);
	return 0;
}

struct addrspace *
as_create(void)
{
	struct addrspace *as = kmalloc(sizeof(struct addrspace));
	if (as==NULL) {
		return NULL;
	}

	
	as->as_stackpbase = 0;
	return as;
}

void
as_destroy(struct addrspace *as)
{
	kfree(as);
}

void
as_activate(void)
{
	int i, spl;
	struct addrspace *as;

	as = proc_getas();
	if (as == NULL) {
		return;
	}

	/* Disable interrupts on this CPU while frobbing the TLB. */
	spl = splhigh();

	for (i=0; i<NUM_TLB; i++) {
		tlb_write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
	}

	splx(spl);
}

void
as_deactivate(void)
{
	/* nothing */
}

int
as_define_region(struct addrspace *as, vaddr_t vaddr, size_t sz,
		 int readable, int writeable, int executable)
{
	size_t npages;

	/* Align the region. First, the base... */
	sz += vaddr & ~(vaddr_t)PAGE_FRAME;
	vaddr &= PAGE_FRAME;

	/* ...and now the length. */
	sz = (sz + PAGE_SIZE - 1) & PAGE_FRAME;

	npages = sz / PAGE_SIZE;

	/* We don't use these - all pages are read-write */
	int privilege=readable|writeable|executable;
	for(int i=0;i<as->no_of_segments;i++)
	{
	if (as->page_table[i].as_vbase == 0) {
		as->page_table[i].as_vbase = vaddr;
		as->page_table[i].as_npages = npages;
		as->page_table[i].temp_privilege=privilege;
		return 0;
	}

	}

	/*
	 * Support for more than two regions is not available.
	 */
	return 0;
}

static
void
as_zero_region(paddr_t paddr, unsigned npages)
{
	bzero((void *)PADDR_TO_KVADDR(paddr), npages * PAGE_SIZE);
}

int
as_prepare_load(struct addrspace *as)
{
	/*KASSERT(as->page_table[i]->as_pbase == 0);
	KASSERT(as->page_table[i]->as_pbase2 == 0);
	KASSERT(as->page_table[i]->as_stackpbase == 0);*/
	for(int i=0;i<as->no_of_segments;i++)
	{
	as->page_table[i].as_pbase = getppages(as->page_table[i].as_npages);
	if (as->page_table[i].as_pbase == 0) {
		return ENOMEM;
	as_zero_region(as->page_table[i].as_pbase, as->page_table[i].as_npages);
	}
	}


	as->as_stackpbase = getppages(DUMBVM_STACKPAGES);
	if (as->as_stackpbase == 0) {
		return ENOMEM;
	}

	as_zero_region(as->as_stackpbase, DUMBVM_STACKPAGES);

	return 0;
}

int
as_complete_load(struct addrspace *as)
{
	//(void)as;
	for(int i=0;i<as->no_of_segments;i++)
	{
	as->page_table[i].final_privilege=as->page_table[i].temp_privilege;
	}
	return 0;
}

int
as_define_stack(struct addrspace *as, vaddr_t *stackptr)
{
	KASSERT(as->as_stackpbase != 0);

	*stackptr = USERSTACK;
	return 0;
}

int
as_copy(struct addrspace *old, struct addrspace **ret)
{
	struct addrspace *new;

	new = as_create();
	if (new==NULL) {
		return ENOMEM;
	}
	for(int i=0;i<old->no_of_segments;i++)
	{
	new->page_table[i].as_vbase = old->page_table[i].as_vbase;
	new->page_table[i].as_npages = old->page_table[i].as_npages;
	}
	 if (as_prepare_load(new)) {
                as_destroy(new);
                return ENOMEM;
        }
	 for(int i=0;i<old->no_of_segments;i++)
        {

	   memmove((void *)PADDR_TO_KVADDR(new->page_table[i].as_pbase),
                (const void *)PADDR_TO_KVADDR(old->page_table[i].as_pbase),
                old->page_table[i].as_npages*PAGE_SIZE);
	}
        memmove((void *)PADDR_TO_KVADDR(new->as_stackpbase),
                (const void *)PADDR_TO_KVADDR(old->as_stackpbase),
                DUMBVM_STACKPAGES*PAGE_SIZE);

	

	/* (Mis)use as_prepare_load to allocate some physical memory. */

	/*KASSERT(new->as_pbase != 0);
	KASSERT(new->as_pbase2 != 0);
	KASSERT(new->as_stackpbase != 0);
	*/

	*ret = new;
	return 0;
}
