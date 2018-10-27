/*
 * Copyright (c) 2001, 2002, 2009
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
#include <lib.h>
#include <clock.h>
#include <thread.h>
#include <test.h>
#include<synch.h>
#define NPEOPLE 20
//struct lock *boy_lock;
//struct lock *girl_lock;
int i,j;
static struct semaphore *boy_sem;
static struct semaphore *girl_sem;
int people=0;

static
void
shower()
{
	// The thread enjoys a refreshing shower!
        clocksleep(1);
}

static
void
boy(void *p, unsigned long which)
{
	//kprintf("Boy sem count: %d",boy_sem->sem_count);
	(void)p;
	//if(girl_sem->count<3||boy_sem_count==0)
	//{
	
	kprintf("boy #%ld starting\n", which);
	P(boy_sem);

	// Implement this function
	// use bathroom
	kprintf("boy #%ld entering bathroom...\n", which);
	shower();
	people++;
	kprintf("boy #%ld leaving bathroom\n", which);
	if(boy_sem->sem_count==0)
	{
	if(people!=NPEOPLE)
	{
	int m=girl_sem->sem_count;
	for(i=m;i<3;i++)
	V(girl_sem);
	}
	}
}

static
void
girl(void *p, unsigned long which)
{
	(void)p;
	
	//P(girl_sem);
	kprintf("girl #%ld starting\n", which);
	P(girl_sem);
	// Implement this function
	
	// use bathroom
	kprintf("girl #%ld entering bathroom\n", which);
	shower();
	people++;
	kprintf("girl #%ld leaving bathroom\n", which);
	if(girl_sem->sem_count==0)
	{
	if(people!=NPEOPLE)
	{
	int n=boy_sem->sem_count;
	for(j=n;j<3;j++)
	V(boy_sem);
	}
	}
	
}

// Change this function as necessary
int
bathroom(int nargs, char **args)
{
	boy_sem=sem_create("Boy Sem",3);
	girl_sem=sem_create("Girl Sem",0);
	int i, err=0;

	(void)nargs;
	(void)args;


	for (i = 0; i < NPEOPLE; i++) {
		switch(i % 2) {
			case 0:
			err = thread_fork("Boy Thread", NULL,
					  boy, NULL, i);
			break;
			case 1:
			err = thread_fork("Girl Thread", NULL,
					  girl, NULL, i);
			break;
		}
		if (err) {
			panic("bathroom: thread_fork failed: %s)\n",
				strerror(err));
		}
	}
	kprintf("\n");

	return 0;
}

