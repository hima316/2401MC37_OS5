#include "spinlock.h"

#define MAX_SEMS 16

struct kernel_sem {
  struct spinlock lock;
  int count;
  int allocated;
};

static struct kernel_sem sem_pool[MAX_SEMS];

uint64
sys_sem_init(void)
{
  int sem_id, init_val;
  argint(0, &sem_id);
  argint(1, &init_val);

  if (sem_id < 0 || sem_id >= MAX_SEMS)
    return -1;

  if (!sem_pool[sem_id].allocated) {
    initlock(&sem_pool[sem_id].lock, "ksem");
    sem_pool[sem_id].allocated = 1;
  }

  acquire(&sem_pool[sem_id].lock);
  sem_pool[sem_id].count = init_val;
  release(&sem_pool[sem_id].lock);

  return 0;
}

uint64
sys_sem_wait(void)
{
  int sem_id;
  argint(0, &sem_id);

  if (sem_id < 0 || sem_id >= MAX_SEMS)
    return -1;

  acquire(&sem_pool[sem_id].lock);
  while (sem_pool[sem_id].count == 0) {
    // Atomically releases the spinlock and sleeps on the channel
    sleep(&sem_pool[sem_id], &sem_pool[sem_id].lock);
  }
  sem_pool[sem_id].count--;
  release(&sem_pool[sem_id].lock);

  return 0;
}

uint64
sys_sem_post(void)
{
  int sem_id;
  argint(0, &sem_id);

  if (sem_id < 0 || sem_id >= MAX_SEMS)
    return -1;

  acquire(&sem_pool[sem_id].lock);
  sem_pool[sem_id].count++;
  wakeup(&sem_pool[sem_id]);
  release(&sem_pool[sem_id].lock);

  return 0;
}