// system calls
int fork(void);
int exit(int) __attribute__((noreturn));
...
void* shm_get(void);