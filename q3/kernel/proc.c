// in allocproc()
p->shm_page = 0;
  return p;

//in fork()
// Map identical shared page into child process if parent allocated it
  if (p->shm_page != 0) {
    np->shm_page = p->shm_page;
    if (mappinpages(np->pagetable, TRAPFRAME - PGSIZE, PGSIZE, np->shm_page, PTE_R | PTE_W | PTE_U) < 0) {
      freeproc(np);
      release(&np->lock);
      return -1;
    }
  }

// in freeproc()
if (p->shm_page != 0) {
    uvmunmap(p->pagetable, TRAPFRAME - PGSIZE, 1, 0);
    if (p->parent == 0 || p->parent->shm_page != p->shm_page) {
      kfree((void *)p->shm_page);
    }
    p->shm_page = 0;
  }

