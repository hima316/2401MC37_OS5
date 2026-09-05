# Peterson's Algorithm on xv6-riscv

## 1. Design & Approach
- **Shared Memory Primitive (`shm_get`)**: 
  Processes in xv6 operate with isolated page tables. A new system call, `shm_get()`, allocates one physical page via `kalloc()` and maps it with read/write/user access (`PTE_R | PTE_W | PTE_U`) directly below the process's `TRAPFRAME` (`TRAPFRAME - PGSIZE`).
- **Inheritance via `fork()`**: 
  During `fork()`, if the parent contains an active shared page, the kernel maps the identical physical frame (`shm_page`) into the child process's page table at the same virtual address.
- **Mutual Exclusion via Peterson's Algorithm**:
  - Mutual exclusion logic is held strictly in `volatile int flag[2]` and `volatile int turn`.
  - RISC-V memory barriers (`asm volatile("fence rw,rw" ::: "memory")`) prevent compiler optimizations and hardware reordering around the critical section entry and exit.

## 2. How to Build and Run
1. Build and run in QEMU:
   ```bash
   make clean
   make qemu