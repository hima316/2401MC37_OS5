# Q2: Producer-Consumer (Bounded Buffer)

## Design Choices & Mechanisms
1. **Lightweight Semaphores:**
   - I implemented custom kernel-level semaphores (`SYS_sem_init`, `SYS_sem_wait`, `SYS_sem_post`).
   - The semaphores are backed by an internal array `struct sem_t sems[10]` mapped inside the kernel.
   - Synchronization is handled natively by xv6's `spinlock`, `sleep()`, and `wakeup()`. Using `sleep(chan, lock)` atomically releases the semaphore lock while putting the process to sleep, preventing race conditions.
2. **Shared Memory Array:**
   - Instead of pipes, I used actual shared memory (`shm_get` implemented in Q1) to strictly reflect classic textbook Bounded Buffer architectures. The producer and consumer manipulate the exact same struct (containing the `buf` array, `in` index, and `out` index).

## How to Build and Run
1. Recompile the OS and boot QEMU:
   ```bash
   make clean && make qemu