// Inside allocproc() around line 125:
  p->shm_page = 0;
  return p;

// Inside fork() around line 280:
  // If parent has a shared page, share it with the child
  if (p->shm_page != 0) {
    np->shm_page = p->shm_page;
    if (mappinpages(np->pagetable, TRAPFRAME - PGSIZE, PGSIZE, np->shm_page, PTE_R | PTE_W | PTE_U) < 0) {
      freeproc(np);
      release(&np->lock);
      return -1;
    }
  }

// Inside freeproc() around line 150:
  if (p->shm_page != 0) {
    // Unmap the user-level page
    uvmunmap(p->pagetable, TRAPFRAME - PGSIZE, 1, 0);
    // Free the physical page if this process is the parent (or if both processes finish)
    if (p->parent == 0 || p->parent->shm_page != p->shm_page) {
      kfree((void *)p->shm_page);
    }
    p->shm_page = 0;
  }