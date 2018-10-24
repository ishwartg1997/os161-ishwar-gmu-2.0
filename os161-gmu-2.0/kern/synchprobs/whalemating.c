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

/*
 * Driver code for whale mating problem
 */
#include <types.h>
#include <lib.h>
#include <thread.h>
#include <test.h>
#include<synch.h>

#define NMATING 10
static struct cv *male_cv,*female_cv;
static struct lock *dating_lock;
//static bool gender_male=true;
static struct semaphore *matchmaker_male,*matchmaker_female;
static
void
male(void *p, unsigned long which)
{
	(void)p;
	kprintf("male whale #%ld starting\n", which);
	V(matchmaker_male);
        lock_acquire(dating_lock);
        cv_wait(male_cv,dating_lock);
	kprintf("male whale #%ld mated\n", which);
	lock_release(dating_lock);
	// Implement this function
}

static
void
female(void *p, unsigned long which)
{
	(void)p;
	kprintf("female whale #%ld starting\n", which);
	V(matchmaker_female);
        lock_acquire(dating_lock);
        cv_wait(female_cv,dating_lock);
	kprintf("female whale #%ld mated\n", which);
	lock_release(dating_lock);
	//lock_release(dating_lock);
	// Implement this function
}

static
void
matchmaker(void *p, unsigned long which)
{
	(void)p;

	kprintf("matchmaker whale #%ld starting\n", which);
	//lock_acquire(dating_lock);
	P(matchmaker_male);
        P(matchmaker_female);
        lock_acquire(dating_lock);
	cv_signal(male_cv,dating_lock);
	
	//lock_acquire(dating_lock);
	cv_signal(female_cv,dating_lock);
	
	kprintf("matchmaker whale #%ld helped mate\n", which);
	lock_release(dating_lock);
	// Implement this function
}


// Change this function as necessary
int
whalemating(int nargs, char **args)
{
	male_cv=cv_create("male");
	female_cv=cv_create("female");
	matchmaker_male=sem_create("Male",0);
	matchmaker_female=sem_create("Female",0);	
	dating_lock=lock_create("Dating");
	int i, j, err=0;

	(void)nargs;
	(void)args;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < NMATING; j++) {
			switch(i) {
			    case 0:
				err = thread_fork("Male Whale Thread",
						  NULL, male, NULL, j);
				break;
			    case 1:
				err = thread_fork("Female Whale Thread",
						  NULL, female, NULL, j);
				break;
			    case 2:
				err = thread_fork("Matchmaker Whale Thread",
						  NULL, matchmaker, NULL, j);
				break;
			}
			if (err) {
				panic("whalemating: thread_fork failed: %s)\n",
				      strerror(err));
			}
		}
	}
	return 0;
}
