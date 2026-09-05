# xv6 Synchronization & Concurrency Assignment

This repository contains xv6-riscv kernel and user-space implementations of three classic synchronization problems:

1. **Peterson's Algorithm** — Software-only mutual exclusion over shared memory
2. **Producer-Consumer Problem** — Bounded buffer using counting semaphores
3. **Readers-Writers Problem** — Fair/starvation-free reader-writer synchronization

---

## 1. Project Directory Structure

```text
xv6-riscv/
├── Makefile                 # Updated UPROGS to include _peterson, _prodcons, and _readwrite
├── README.md                # Documentation of design, mechanics, and run instructions
│
├── kernel/
│   ├── defs.h               # Kernel prototypes for shm and semaphore syscalls
│   ├── proc.c               # Shared-memory page inheritance in fork() & cleanup in freeproc()
│   ├── proc.h               # Extended struct proc with shm_page physical address tracker
│   ├── syscall.c            # System call lookup table & dispatch handlers
│   ├── syscall.h            # Syscall numbers: SYS_shm_get, SYS_sem_init,
│   │                         # SYS_sem_wait, SYS_sem_post
│   └── sysproc.c             # Implementations of shm_get and semaphore kernel primitives
│
└── user/
    ├── peterson.c           # Question 1: Peterson's algorithm across 2 processes
    ├── prodcons.c           # Question 2: Bounded buffer with empty, full, and mutex semaphores
    ├── readwrite.c          # Question 3: 3 readers and 2 writers with starvation prevention
    ├── user.h               # User-level function prototypes for all added syscalls
    └── usys.pl              # Syscall stub generators for entry points
```

---

## 2. Kernel Primitives (Shared Across Tasks)

### Shared Memory (`shm_get`)

Maps an allocated physical page (`kalloc()`) into user virtual memory at:

```text
(TRAPFRAME - PGSIZE)
```

with user read/write permissions:

```text
PTE_R | PTE_W | PTE_U
```

During `fork()`, the child inherits the identical page mapping, providing true shared memory without file/pipe emulation.

### Kernel Semaphores (`sem_init`, `sem_wait`, `sem_post`)

Semaphores are maintained in an internal kernel table protected by xv6 spinlocks.

Blocking and waking are managed atomically using xv6's:

```c
sleep(chan, &lock);
wakeup(chan);
```

---

## 3. Question Overviews & Design

### Question 1: Peterson's Algorithm (`peterson.c`)

**Objective:**  
Implement mutual exclusion between two processes using only shared software flags.

**Design:**

- Uses `volatile int flag[2]` and `volatile int turn` inside the shared memory region.
- RISC-V memory barriers are inserted at entry and exit sections to prevent CPU and compiler reordering across the critical-section boundary:

```c
asm volatile("fence rw,rw" ::: "memory");
```

---

### Question 2: Producer-Consumer Problem (`prodcons.c`)

**Objective:**  
Coordinate production and consumption across a fixed circular buffer of size **5**.

### Design

- Synchronized using three semaphores:
  - `empty` — initialized to `5`
  - `full` — initialized to `0`
  - `mutex` — initialized to `1`
- The producer blocks on `sem_wait(empty)` when all 5 buffer slots are full.
- The consumer blocks on `sem_wait(full)` when the buffer is empty.
- Guarantees **in-order delivery** and **zero data loss** for 20 sequential items.

---

### Question 3: Readers-Writers Problem (`readwrite.c`)

**Objective:**  
Allow concurrent reading with exclusive writing while preventing **writer starvation**.

### Design

- Implements a fair **turnstile semaphore** (`order_mutex`) alongside `read_mutex` and `write_mutex`.
- **Starvation Prevention:** Arriving writers acquire the turnstile, preventing newly arriving readers from continuously entering and starving waiting writers.
- Spawns **3 readers** and **2 writers** using `fork()`.
- Verified through interleaved logs showing:
  - Simultaneous read concurrency.
  - Exclusive writer access.
  - Fair access for waiting writers.

---

## 4. Build & Run Instructions

To compile and launch xv6 in QEMU:

```bash
make clean
make qemu
```

Once inside the xv6 shell, run each program individually.

### Question 1: Peterson's Algorithm

```text
peterson
```

### Question 2: Producer-Consumer

```text
prodcons
```

### Question 3: Readers-Writers

```text
readwrite
```

To exit QEMU:

```text
Ctrl-A, then X
```
