#include "spinlock.h"

#define NSEM 10 // Max number of semaphores

struct sem_t {
  struct spinlock lock;
  int count;
  int initialized;
} sems[NSEM];

uint64
sys_sem_init(void)
{
  int id, max_val;
  argint(0, &id);
  argint(1, &max_val);

  if(id < 0 || id >= NSEM) return -1;

  if(!sems[id].initialized) {
    initlock(&sems[id].lock, "semaphore");
    sems[id].initialized = 1;
  }
  
  acquire(&sems[id].lock);
  sems[id].count = max_val;
  release(&sems[id].lock);
  return 0;
}

uint64
sys_sem_wait(void)
{
  int id;
  argint(0, &id);
  if(id < 0 || id >= NSEM) return -1;

  acquire(&sems[id].lock);
  while(sems[id].count == 0) {
    // Sleep releases the spinlock while waiting, and reacquires it when woken
    sleep(&sems[id], &sems[id].lock);
  }
  sems[id].count--;
  release(&sems[id].lock);
  return 0;
}

uint64
sys_sem_post(void)
{
  int id;
  argint(0, &id);
  if(id < 0 || id >= NSEM) return -1;

  acquire(&sems[id].lock);
  sems[id].count++;
  wakeup(&sems[id]); // Wakes up all sleeping processes on this channel
  release(&sems[id].lock);
  return 0;
}