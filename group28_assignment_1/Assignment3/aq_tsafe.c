#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "aq.h"

// Structure to represent each message
typedef struct MsgNode {
    void *msg;
    MsgKind kind;
    struct MsgNode *next;
} MsgNode;

// Structure to represent the alarm queue
typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    MsgNode *head;          // Points to the head of the queue
    MsgNode *tail;          // Points to the tail of the queue
    int alarm_present;      // 1 if an alarm message is in the queue, 0 otherwise
} AlarmQueueImpl;

// Create the alarm queue
AlarmQueue aq_create() {
    AlarmQueueImpl *queue = (AlarmQueueImpl *)malloc(sizeof(AlarmQueueImpl));
    if (!queue) return NULL;
    
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->cond, NULL);
    queue->head = NULL;
    queue->tail = NULL;
    queue->alarm_present = 0;

    return (AlarmQueue)queue;
}

// Send a message to the queue
int aq_send(AlarmQueue aq, void *msg, MsgKind k) {
    AlarmQueueImpl *queue = (AlarmQueueImpl *)aq;
    pthread_mutex_lock(&queue->lock);
    
    // If message is an alarm and an alarm is already present, block
    if (k == AQ_ALARM) {
        while (queue->alarm_present) {
            pthread_cond_wait(&queue->cond, &queue->lock);
        }
        queue->alarm_present = 1; // Mark that an alarm is now present
    }

    // Create a new node for the message
    MsgNode *new_node = (MsgNode *)malloc(sizeof(MsgNode));
    new_node->msg = msg;
    new_node->kind = k;
    new_node->next = NULL;

    // Add to queue
    if (queue->tail) {
        queue->tail->next = new_node;
    } else {
        queue->head = new_node;
    }
    queue->tail = new_node;

    pthread_cond_signal(&queue->cond); // Notify waiting threads
    pthread_mutex_unlock(&queue->lock);
    return 0;
}

// Receive a message from the queue
int aq_recv(AlarmQueue aq, void **msg) {
    AlarmQueueImpl *queue = (AlarmQueueImpl *)aq;
    pthread_mutex_lock(&queue->lock);

    // Wait until a message is available
    while (!queue->head) {
        pthread_cond_wait(&queue->cond, &queue->lock);
    }

    // Get the message at the head
    MsgNode *node = queue->head;
    *msg = node->msg;
    int kind = node->kind;

    // Update head and alarm presence
    queue->head = node->next;
    if (!queue->head) {
        queue->tail = NULL;
    }
    if (kind == AQ_ALARM) {
        queue->alarm_present = 0; // Allow new alarm messages
    }

    free(node);
    pthread_cond_signal(&queue->cond); // Notify any blocked senders
    pthread_mutex_unlock(&queue->lock);

    return kind;
}

// Destroy the alarm queue
void aq_destroy(AlarmQueue aq) {
    AlarmQueueImpl *queue = (AlarmQueueImpl *)aq;
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->cond);

    // Free remaining messages in the queue
    MsgNode *current = queue->head;
    while (current) {
        MsgNode *next = current->next;
        free(current->msg); 
        free(current);
        current = next;
    }
    
    free(queue);
}

// Get the size of the queue (number of messages)
int aq_size(AlarmQueue aq) {
    AlarmQueueImpl *queue = (AlarmQueueImpl *)aq;
    pthread_mutex_lock(&queue->lock);
    int size = 0;
    MsgNode *current = queue->head;
    while (current) {
        size++;
        current = current->next;
    }
    pthread_mutex_unlock(&queue->lock);
    return size;
}

// Get the number of alarm messages in the queue (0 or 1)
int aq_alarms(AlarmQueue aq) {
    AlarmQueueImpl *queue = (AlarmQueueImpl *)aq;
    pthread_mutex_lock(&queue->lock);
    int alarms = queue->alarm_present ? 1 : 0;
    pthread_mutex_unlock(&queue->lock);
    return alarms;
}
