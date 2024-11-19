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
        int message1 = 1;
        put_normal(q, message1);

        msleep(500);
        int message2 = 2;
        put_normal(q, message2);

        int message3 = 3;
        put_normal(q, message3);
        return NULL;
    }

void *consumer(void *arg){
    int msg;
    msg = get(q);
    printf("Consumer received: %d\n", msg);

    msg = get(q);
    printf("Consumer received: %d\n", msg);

    msg = get(q);
    printf("Consumer received: %d\n", msg);
    return NULL;
}
// Test 1: Blocking Alarm Message Test
void *send_alarm(void *queue){
    char *message = malloc(20);
    sprintf(message, "Alarm Message");

    int result = aq_send((AlarmQueue)queue, message, AQ_ALARM);
    if (result == AQ_NO_ROOM){
        fprintf(stderr, "Thread A: Alarm message blocked as expected.\n");
        free(message); // Free memory if it wasn't sent
    } else {
        printf("Thread A: Alarm message sent.\n");
    }
    return NULL;
}

void *send_second_alarm(void *queue){
    msleep(1000); // Allow some time for blocking to occur
    char *message = malloc(20);
    sprintf(message, "Second Alarm");
    printf("Thread B: Attempting to send second alarm message...\n");
    
    int result = aq_send((AlarmQueue)queue, message, AQ_ALARM);
    if (result == 0){
        printf("Thread B: Successfully sent second alarm message.\n");
    } else {
        fprintf(stderr, "Thread B: Failed to send second alarm message.\n");
        free(message); // Free memory if it wasn't sent
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
        free(msg); // Free allocated memory
    } else {
        printf("Thread C: Unexpectedly received normal message: %s\n", (char *)msg);
    }
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
    for (int i = 1; i <= 3; i++){
        char *message = malloc(20);
        sprintf(message, "Message %d", i);
        aq_send(queue, message, AQ_NORMAL);
    }
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
        } else if (kind == AQ_ALARM){
            printf("Thread Y: Unexpectedly received alarm message: %s\n", (char *)msg);
            free(msg); // Avoid memory leaks
        } else {
            printf("Thread Y: Received unknown message type.\n");
        }
    }
    return NULL;
}

void test_fifo_order(AlarmQueue queue){
    printf("\nRunning Test 2: FIFO Order of Normal Messages...\n");

    //aq_clean(queue);
    queue = aq_create();
    if (!queue)
    {
        fprintf(stderr, "Failed to reinitialize alarm queue.\n");
        exit(1);
    }

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
