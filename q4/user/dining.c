#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_PHILOSOPHERS 5
#define NUM_CYCLES       5

// Semaphore index 5 is dedicated to atomic printing across processes
#define PRINT_LOCK 5

void delay(int count) {
  for (volatile int i = 0; i < count; i++) {
    // Busy delay loop
  }
}

void safe_print(int phil_id, const char *msg) {
  sem_wait(PRINT_LOCK);
  printf("[Tick %d] Philosopher %d: %s\n", uptime(), phil_id, msg);
  sem_post(PRINT_LOCK);
}

void philosopher(int id) {
  int left_fork = id;
  int right_fork = (id + 1) % NUM_PHILOSOPHERS;

  int first_fork, second_fork;

  // Asymmetric ordering strategy:
  // Odd philosophers pick right fork first, even philosophers pick left fork first.
  if (id % 2 == 1) {
    first_fork = right_fork;
    second_fork = left_fork;
  } else {
    first_fork = left_fork;
    second_fork = right_fork;
  }

  for (int cycle = 1; cycle <= NUM_CYCLES; cycle++) {
    // 1. Thinking
    safe_print(id, "THINKING");
    delay(300000);

    // 2. Hungry
    safe_print(id, "HUNGRY (attempting fork pickup)");

    // Acquire forks in asymmetric order
    sem_wait(first_fork);
    sem_wait(second_fork);

    // 3. Eating
    safe_print(id, "EATING (acquired both forks)");
    delay(400000);

    // Release forks
    sem_post(second_fork);
    sem_post(first_fork);

    safe_print(id, "FINISHED EATING (released both forks)");
    sleep(2); // Short sleep before beginning the next cycle
  }

  safe_print(id, "COMPLETED ALL CYCLES");
  exit(0);
}

int main(int argc, char *argv[]) {
  // 1. Initialize 5 binary semaphores for forks (IDs 0 to 4)
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    if (sem_init(i, 1) < 0) {
      printf("Failed to initialize fork semaphore %d\n", i);
      exit(1);
    }
  }

  // 2. Initialize print synchronization lock (ID 5)
  sem_init(PRINT_LOCK, 1);

  printf("Starting Dining Philosophers (N = %d, Cycles = %d)...\n",
         NUM_PHILOSOPHERS, NUM_CYCLES);

  // 3. Spawn 5 philosopher processes
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    int pid = fork();
    if (pid < 0) {
      printf("fork failed for philosopher %d\n", i);
      exit(1);
    }
    if (pid == 0) {
      philosopher(i);
    }
  }

  // 4. Parent process waits for all 5 child processes to terminate
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    wait(0);
  }

  printf("\nAll 5 philosophers successfully finished without deadlock.\n");
  exit(0);
}