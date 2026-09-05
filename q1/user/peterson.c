#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Memory layout of the shared page
struct shared_data {
  volatile int flag[2];
  volatile int turn;
  volatile int counter;
};

// Memory fence macro to prevent out-of-order execution across the barrier
#define mb() asm volatile("fence rw,rw" ::: "memory")

void delay(int cycles) {
  for (volatile int i = 0; i < cycles; i++) {
    // Busy wait remainder loop
  }
}

int
main(int argc, char *argv[])
{
  struct shared_data *shm = (struct shared_data *)shm_get();
  if ((uint64)shm == 0) {
    printf("peterson: shm_get failed\n");
    exit(1);
  }

  // Initialize shared variables
  shm->flag[0] = 0;
  shm->flag[1] = 0;
  shm->turn = 0;
  shm->counter = 0;

  int pid = fork();
  if (pid < 0) {
    printf("peterson: fork failed\n");
    exit(1);
  }

  int id = (pid == 0) ? 1 : 0; // 0 for parent, 1 for child
  int other = 1 - id;
  const int ITERATIONS = 10;

  for (int i = 0; i < ITERATIONS; i++) {
    /* ---------------- Entry Section ---------------- */
    shm->flag[id] = 1;
    shm->turn = other;
    mb(); // Ensure stores are committed before condition test

    while (shm->flag[other] == 1 && shm->turn == other) {
      // Busy wait
    }
    mb(); // Acquire barrier

    /* ---------------- Critical Section ---------------- */
    int current = shm->counter;
    printf("Process %d in CS, counter = %d\n", id, current);
    delay(500000); // Artificial delay to expose potential race conditions
    shm->counter = current + 1;

    /* ---------------- Exit Section ---------------- */
    mb(); // Release barrier
    shm->flag[id] = 0;

    /* ---------------- Remainder Section ---------------- */
    delay(300000);
  }

  if (pid == 0) {
    exit(0);
  } else {
    wait(0);
    printf("Final Counter Value: %d (Expected: %d)\n", shm->counter, 2 * ITERATIONS);
    if (shm->counter == 2 * ITERATIONS) {
      printf("SUCCESS: Mutual exclusion preserved, updates match.\n");
    } else {
      printf("FAILURE: Race condition encountered.\n");
    }
    exit(0);
  }
}