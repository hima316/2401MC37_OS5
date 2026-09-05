#include "memlayout.h"

#define RW_SHM_VA (TRAPFRAME - PGSIZE)

uint64
sys_rw_shm_get(void)
{
  struct proc *p = myproc();
  if (p->shm_page != 0)
    return RW_SHM_VA;

  char *pa = kalloc();
  if (pa == 0)
    return 0;

  memset(pa, 0, PGSIZE);

  if (mappinpages(p->pagetable, RW_SHM_VA, PGSIZE, (uint64)pa, PTE_R | PTE_W | PTE_U) < 0) {
    kfree(pa);
    return 0;
  }

  p->shm_page = (uint64)pa;
  return RW_SHM_VA;
}

// ---------------- Semaphore Implementation ----------------
#define NSEM 16

struct sem_t {
  struct spinlock lock;
  int count;
  int initialized;
} sems[NSEM];

uint64
sys_sem_init(void)
{
  int id, val;
  argint(0, &id);
  argint(1, &val);

  if (id < 0 || id >= NSEM)
    return -1;

  if (!sems[id].initialized) {
    initlock(&sems[id].lock, "semaphore");
    sems[id].initialized = 1;
  }

  acquire(&sems[id].lock);
  sems[id].count = val;
  release(&sems[id].lock);
  return 0;
}

uint64
sys_sem_wait(void)
{
  int id;
  argint(0, &id);
  if (id < 0 || id >= NSEM)
    return -1;

  acquire(&sems[id].lock);
  while (sems[id].count == 0) {
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
  if (id < 0 || id >= NSEM)
    return -1;

  acquire(&sems[id].lock);
  sems[id].count++;
  wakeup(&sems[id]);
  release(&sems[id].lock);
  return 0;
}