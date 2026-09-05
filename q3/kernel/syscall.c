// Add function prototypes:
extern uint64 sys_rw_shm_get(void);
extern uint64 sys_sem_init(void);
extern uint64 sys_sem_wait(void);
extern uint64 sys_sem_post(void);

// Add to syscalls function pointer array:
[SYS_rw_shm_get] sys_rw_shm_get,
[SYS_sem_init]   sys_sem_init,
[SYS_sem_wait]   sys_sem_wait,
[SYS_sem_post]   sys_sem_post,