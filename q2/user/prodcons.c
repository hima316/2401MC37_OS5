#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUF_SIZE 5

// Semaphore IDs
#define MUTEX_ID 0
#define EMPTY_ID 1
#define FULL_ID  2

struct shared_buffer {
  int buf[BUF_SIZE];
  int in;
  int out;
};

int main(int argc, char *argv[])
{
  // 1. Allocate shared memory (reusing Q1 shm_get)
  struct shared_buffer *q = (struct shared_buffer *)shm_get();
  if ((uint64)q == 0) {
    printf("prodcons: shm_get failed\n");
    exit(1);
  }
  q->in = 0;
  q->out = 0;

  // 2. Initialize Semaphores
  sem_init(MUTEX_ID, 1);         // Mutex lock for buffer array
  sem_init(EMPTY_ID, BUF_SIZE);  // Counting sem for empty slots
  sem_init(FULL_ID, 0);          // Counting sem for filled slots

  int pid = fork();
  if (pid < 0) {
    printf("prodcons: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // ---------------- CONSUMER ----------------
    for (int i = 1; i <= 20; i++) {
      printf("[Consumer] Waiting for items... (Block if empty)\n");
      sem_wait(FULL_ID);
      sem_wait(MUTEX_ID);

      // Critical Section
      int item = q->buf[q->out];
      printf("           -> [Consumer] Removed: %d (Slot %d)\n", item, q->out);
      q->out = (q->out + 1) % BUF_SIZE;

      sem_post(MUTEX_ID);
      sem_post(EMPTY_ID);

      // Consumer is slower initially, making Producer fill the buffer & block
      sleep(10); 
    }
    exit(0);
  } else {
    // ---------------- PRODUCER ----------------
    for (int i = 1; i <= 20; i++) {
      printf("[Producer] Trying to produce %d... (Block if full)\n", i);
      sem_wait(EMPTY_ID);
      sem_wait(MUTEX_ID);

      // Critical Section
      q->buf[q->in] = i;
      printf("           <- [Producer] Inserted: %d (Slot %d)\n", i, q->in);
      q->in = (q->in + 1) % BUF_SIZE;

      sem_post(MUTEX_ID);
      sem_post(FULL_ID);

      // Producer becomes slower later, allowing Consumer to empty buffer
      if (i > 10) sleep(15); 
      else sleep(1);
    }
    wait(0);
    printf("Producer-Consumer completed successfully.\n");
    exit(0);
  }
}