# Readers-Writers Problem Implementation (xv6)

## 1. Synchronization Logic & Starvation Prevention

### First Readers-Writers (Unbounded Reader Priority) Issue
In the textbook First Readers-Writers formulation:
- Readers acquire a mutex, increment `read_count`, and if `read_count == 1`, acquire `wrt`.
- As long as at least one reader remains active, subsequent readers bypass `wrt` and enter the critical section directly.
- **Problem**: If reader arrivals overlap continuously, `read_count` never drops to 0, causing **writer starvation**.

### Starvation-Free / Fair Solution (Implemented Here)
We introduce an ordering turnstile semaphore (`SEM_ORDER`):
1. **Readers**:
   - Must pass `wait(SEM_ORDER)` before incrementing `read_count`, then immediately release `post(SEM_ORDER)`.
2. **Writers**:
   - Must call `wait(SEM_ORDER)`, then `wait(SEM_WRT)`, then release `post(SEM_ORDER)`.
3. **Starvation Prevention Effect**:
   - When a writer arrives, it acquires `SEM_ORDER`.
   - New incoming readers block on `wait(SEM_ORDER)` behind the writer instead of incrementing `read_count`.
   - Active readers already inside the critical section finish and exit. The last active reader releases `SEM_WRT`, letting the queued writer execute without delay.
   - Concurrent reads remain preserved because readers release `SEM_ORDER` immediately after registering in `read_count`.

## 2. Compilation and Execution
```bash
make clean
make qemu
$ readwrite