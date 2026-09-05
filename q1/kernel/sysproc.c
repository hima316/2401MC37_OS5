#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

// Define a fixed virtual address for the shared page:
// TRAPFRAME is at (TRAMPOLINE - PGSIZE). We place SHM right beneath it.
#define SHM_VA (TRAPFRAME - PGSIZE)

uint64
sys_shm_get(void)
{
  struct proc *p = myproc();

  // If already mapped for this process, return existing virtual address
  if (p->shm_page != 0) {
    return SHM_VA;
  }

  // Allocate a single physical memory page
  char *pa = kalloc();
  if (pa == 0)
    return 0;

  memset(pa, 0, PGSIZE);

  // Map into current user page table with Read, Write, and User permissions
  if (mappinpages(p->pagetable, SHM_VA, PGSIZE, (uint64)pa, PTE_R | PTE_W | PTE_U) < 0) {
    kfree(pa);
    return 0;
  }

  p->shm_page = (uint64)pa;
  return SHM_VA;
}