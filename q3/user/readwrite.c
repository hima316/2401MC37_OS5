#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Semaphore IDs
#define SEM_MUTEX      0   // Protects read_count
#define SEM_WRT        1   // Guarantees exclusive write access
#define SEM_ORDER      2   // Turnstile: prevents writer starvation

struct shared_state {
  int shared_data;
  int read_count;
};

void delay(int cycles) {
  for (volatile int i = 0; i < cycles; i++) {}
}

void reader_process(int id, struct shared_state *state) {
  for (int iter = 0; iter < 3; iter++) {
    // Turnstile: wait here if a writer is already queued
    sem_wait(SEM_ORDER);
    sem_wait(SEM_MUTEX);
    
    state->read_count++;
    if (state->read_count == 1) {
      // First reader locks writing access
      sem_wait(SEM_WRT);
    }
    
    sem_post(SEM_MUTEX);
    sem_post(SEM_ORDER); // Release turnstile for the next arrival

    /* ---------------- Critical Section: Reading ---------------- */
    printf("[Reader %d | PID %d | Tick %d] Reading shared_data = %d (Active Readers = %d)\n",
           id, getpid(), uptime(), state->shared_data, state->read_count);
    delay(400000); // Simulate reading time

    /* ---------------- Exit Section ---------------- */
    sem_wait(SEM_MUTEX);
    state->read_count--;
    printf("[Reader %d | PID %d | Tick %d] Finished reading (Remaining Readers = %d)\n",
           id, getpid(), uptime(), state->read_count);
    if (state->read_count == 0) {
      // Last reader releases writing access
      sem_post(SEM_WRT);
    }
    sem_post(SEM_MUTEX);

    sleep(3); // Wait before attempting next read
  }
  exit(0);
}

void writer_process(int id, struct shared_state *state) {
  for (int iter = 0; iter < 3; iter++) {
    // Turnstile: stops subsequent readers from jumping ahead
    sem_wait(SEM_ORDER);
    sem_wait(SEM_WRT);
    sem_post(SEM_ORDER);

    /* ---------------- Critical Section: Writing ---------------- */
    printf(">>> [Writer %d | PID %d | Tick %d] ACQUIRED LOCK. Old value = %d\n",
           id, getpid(), uptime(), state->shared_data);
    state->shared_data += 10;
    delay(600000); // Simulate write duration
    printf("<<< [Writer %d | PID %d | Tick %d] COMMITTED New value = %d. RELEASING LOCK\n",
           id, getpid(), uptime(), state->shared_data);

    sem_post(SEM_WRT);

    sleep(5); // Remainder section
  }
  exit(0);
}

int main(int argc, char *argv[]) {
  struct shared_state *state = (struct shared_state *)rw_shm_get();
  if ((uint64)state == 0) {
    printf("readwrite: failed to get shared memory\n");
    exit(1);
  }

  state->shared_data = 100;
  state->read_count = 0;

  // Initialize semaphores
  sem_init(SEM_MUTEX, 1);
  sem_init(SEM_WRT, 1);
  sem_init(SEM_ORDER, 1);

  printf("Starting Readers-Writers with 3 Readers and 2 Writers...\n\n");

  // Spawn 3 Readers
  for (int r = 1; r <= 3; r++) {
    int pid = fork();
    if (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }
    if (pid == 0) {
      reader_process(r, state);
    }
  }

  // Spawn 2 Writers
  for (int w = 1; w <= 2; w++) {
    int pid = fork();
    if (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }
    if (pid == 0) {
      writer_process(w, state);
    }
  }

  // Parent waits for all 5 children to complete
  for (int i = 0; i < 5; i++) {
    wait(0);
  }

  printf("\nAll readers and writers finished. Final shared_data = %d\n", state->shared_data);
  exit(0);
}