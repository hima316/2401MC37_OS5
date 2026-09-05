// Add extern declarations
extern uint64 sys_sem_init(void);
extern uint64 sys_sem_wait(void);
extern uint64 sys_sem_post(void);

// Add to the syscalls array:
[SYS_sem_init] sys_sem_init,
[SYS_sem_wait] sys_sem_wait,
[SYS_sem_post] sys_sem_post,