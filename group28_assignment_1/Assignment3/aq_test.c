#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "aq.h"
#include "aux_new.h"

/**
 * Concurrent program that demonstrates the use of the thread-safe Alarm Queue Library.
 * Includes tests for blocking behavior and FIFO ordering of normal messages.
 */

static AlarmQueue q;

// Producer and Consumer Test
void *producer(void *arg){
    msleep(500);
    put_normal(q, 1);
    msleep(500);
    put_normal(q, 2);
    put_normal(q, 3);
    return NULL;
}

void *consumer(void *arg){
    get(q);
    get(q);
    get(q);
    return NULL;
}

// Test 1: Blocking Alarm Message Test
void *send_alarm(void *queue){
    int result = aq_send((AlarmQueue)queue, "Alarm Message", AQ_ALARM);
    if (result == AQ_NO_ROOM){
        fprintf(stderr, "Thread A: Alarm message blocked as expected.\n");
    } else {
        printf("Thread A: Alarm message sent.\n");
    }
    return NULL;
}

void *send_second_alarm(void *queue){
    msleep(1000); // Allow some time for blocking to occur
    printf("Thread B: Attempting to send second alarm message...\n");
    int result = aq_send((AlarmQueue)queue, "Second Alarm", AQ_ALARM);
    if (result == 0){
        printf("Thread B: Successfully sent second alarm message.\n");
    } else {
        fprintf(stderr, "Thread B: Failed to send second alarm message.\n");
    }
    return NULL;
}

void *receive_alarm_message(void *queue){
    void *msg;
    msleep(2000); // Simulate delay in processing
    printf("Thread C: Receiving message...\n");
    int kind = aq_recv(queue, &msg);
    if (kind == AQ_ALARM){
        printf("Thread C: Received alarm message: %s\n", (char *)msg);
    } else {
        printf("Thread C: Unexpectedly received normal message: %s\n", (char *)msg);
    }
    free(msg); // Free allocated memory
    return NULL;
}

void test_blocking_alarm(AlarmQueue queue){
    printf("\nRunning Test 1: Blocking Alarm Message...\n");
    pthread_t threadA, threadB, threadC;

    pthread_create(&threadA, NULL, send_alarm, queue);
    pthread_create(&threadB, NULL, send_second_alarm, queue);
    pthread_create(&threadC, NULL, receive_alarm_message, queue);

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);
}

// Test 2: FIFO Order of Normal Messages
void *send_normal_messages(void *queue){
    printf("Thread X: Sending normal messages...\n");
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
            free(msg); // Free allocated memory
        } else {
            fprintf(stderr, "Thread Y: Received unexpected alarm message: %s\n", (char *)msg);
        }
    }
    return NULL;
}

void test_fifo_order(AlarmQueue queue){
    printf("\nRunning Test 2: FIFO Order of Normal Messages...\n");
    pthread_t threadX, threadY;

    pthread_create(&threadX, NULL, send_normal_messages, queue);
    msleep(500); // Introduce delay for realistic producer-consumer simulation
    pthread_create(&threadY, NULL, receive_normal_messages, queue);

    pthread_join(threadX, NULL);
    pthread_join(threadY, NULL);
}

int main(int argc, char **argv){
    q = aq_create();
    if (q == NULL){
        fprintf(stderr, "Failed to create alarm queue.\n");
        return 1;
    }

    printf("----------------\nRunning Producer-Consumer Test...\n");
    pthread_t producer_thread, consumer_thread;

    // Original producer-consumer test
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    print_sizes(q);

    // Test 1: Blocking Alarm Message
    test_blocking_alarm(q);

    // Test 2: FIFO Order of Normal Messages
    test_fifo_order(q);

    aq_destroy(q);
    return 0;
}
