
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#include "aq.h"

#include "aux.h"

/** 
 * Concurrent program that sends and receives a few integer messages 
 * in order to demonstrate the basic use of the thread-safe Alarm Queue Library
 *
 * By using sleeps we (try to) control the scheduling of the threads in
 * order to obtain specific execution scenarios.  But there is no guarantee.
 *
 */

static AlarmQueue q;

void * producer (void * arg) {
  msleep(500);
  put_normal(q, 1);
  msleep(500);
  put_normal(q, 2);
  put_normal(q, 3);
  
  return 0;
}

void * consumer(void * arg) {
  get(q);
  get(q);
  get(q);

  return 0;
}

//New Test 1: Blocking Alarm Message Test
void * send_alarm1(void *queue) {
  printf("Thread A: Sending first alarm message...\n");
  if (aq_send(queue, (void *)"Alarm1", AQ_ALARM)!=0){
    fprintf(stderr, "Failed to send alarm message from Thread A\n")
  }
  printf("Thread A: First alarm message sent.\n");
  return NULL;
}
void * send_alarm2(void *queue){
  sleep(1);
  printf("Thread B: Attempting to send second alarm message...\n");
  if (aq_send(queue, (void *)"Alarm2", AQ_ALARM)==0){
    printf("Thread B: Successfully sent second alarm message (unblocked).\n");

  } else {
    fprintf(stderr, "Failed to send alarm message from Thread B\n");
  }
  return NULL;
}

void * receive_alarm(void *queue){
  void *msg;
  sleep(2);
  printf("Thread C: Receiving message...\n");
  int kind = aq_recv(queue, &msg);
  if(kind==AQ_ALARM){
    printf("Thread C: Received alarm message: %s\n", (char *)msg);
  }else{
    printf("Thread C: Received normal message: %s\n", (char *)msg);
  }
  return NULL;
}
void test_blocking_alarm(AlarmQueue queue){
  pthread_t threadA, threadB, threadC;
  pthread_create(&threadA, NULL, send_alarm1, queue);
  pthread_create(&threadB, NULL, send_alarm2, queue);
  pthread_create(&threadC, NULL, receive_alarm, queue);
  pthread_join(threadA, NULL);
  pthread_join(threadB, NULL);
  pthread_join(threadC, NULL);
}
// New Test 2: FIFO Order of Normal Messages
void *send_normal_messages(void *queue)
{
  printf("Thread X: Sending normal messages in order...\n");
  aq_send(queue, (void *)"Message 1", AQ_NORMAL);
  aq_send(queue, (void *)"Message 2", AQ_NORMAL);
  aq_send(queue, (void *)"Message 3", AQ_NORMAL);
  printf("Thread X: Normal messages sent.\n");
  return NULL;
}

void *receive_normal_messages(void *queue){
  void *msg;
  int kind;
  printf("Thread Y: Receiving messages...\n");
  for (int i = 0; i < 3; i++){
    kind = aq_recv(queue, &msg);
    if (kind == AQ_NORMAL){
      printf("Thread Y: Received normal message: %s\n", (char *)msg);
    }else {
      printf("Thread Y: Received alarm message (unexpected): %s\n", (char *)msg);
    }
  }
  return NULL;
}

void test_fifo_order(AlarmQueue queue)
{
  pthread_t threadX, threadY;
  pthread_create(&threadX, NULL, send_normal_messages, queue);
  sleep(1);
  pthread_create(&threadY, NULL, receive_normal_messages, queue);
  pthread_join(threadX, NULL);
  pthread_join(threadY, NULL);
}

int main(int argc, char ** argv) {
    int ret;

  q = aq_create();

  if (q == NULL) {
    printf("Alarm queue could not be created\n");
    exit(1);
  }
  
  pthread_t t1;
  pthread_t t2;

  void * res1;
  void * res2;

  printf("----------------\n");

  // Run original producer-consumer test
  printf("Running Original Producer-Consumer Test\n");
 
  /* Fork threads */
  pthread_create(&t1, NULL, producer, NULL);
  pthread_create(&t2, NULL, consumer, NULL);
  
  /* Join with all threads */
  pthread_join(t1, &res1);
  pthread_join(t2, &res2);

  printf("Threads terminated with %ld, %ld\n", (uintptr_t)res1, (uintptr_t)res2);
  print_sizes(q);

  // Run new test for blocking alarm message
  printf("----------------\n");
  printf("Running Test 1: Blocking Alarm Message\n");
  test_blocking_alarm(q);

  // Run new test for FIFO order of normal messages
  printf("\n----------------\n");
  printf("Running Test 2: FIFO Order of Normal Messages\n");
  test_fifo_order(q);

  // Clean up
  aq_destroy(q);
  return 0;
}
