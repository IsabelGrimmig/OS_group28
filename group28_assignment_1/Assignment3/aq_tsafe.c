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
    pthread_cond_t cond_not_empty;
    pthread_cond_t cond_no_alarm;
    MsgNode *head;          // Points to the head of the queue
    MsgNode *tail;          // Points to the tail of the queue
    int alarm_present;      // 1 if an alarm message is in the queue, 0 otherwise
    int num_messages;       // Number of normal messages in the queue
} AlarmQueueImpl;

// Create the alarm queue
AlarmQueue aq_create() {
    AlarmQueueImpl *queue = (AlarmQueueImpl *)malloc(sizeof(AlarmQueueImpl));
    if (!queue) return NULL;
    
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->cond_not_empty, NULL);
    pthread_cond_init(&queue->cond_no_alarm, NULL);
    queue->head = NULL;
    queue->tail = NULL;
    queue->alarm_present = 0;
    queue->num_messages = 0;

    return (AlarmQueue)queue;
}

// Send a message to the queue
int aq_send(AlarmQueue aq, void *msg, MsgKind k) {
    if (!aq || !msg) return AQ_NULL_MSG;

    AlarmQueueImpl *queue = (AlarmQueueImpl *)aq;
    pthread_mutex_lock(&queue->lock);

    // Handle alarm messages
    if (k == AQ_ALARM) {
        while (queue->alarm_present) {
            pthread_cond_wait(&queue->cond_no_alarm, &queue->lock);
        }
        queue->alarm_present = 1;
    }

    // Create a new node for the message
    MsgNode *new_node = (MsgNode *)malloc(sizeof(MsgNode));
    if (!new_node) {
        pthread_mutex_unlock(&queue->lock);
        return AQ_NO_ROOM;
    }
    new_node->msg = msg;
    new_node->kind = k;
    new_node->next = NULL;

    // Add to the queue
    if (queue->tail) {
        queue->tail->next = new_node;
    } else {
        queue->head = new_node;
    }
    queue->tail = new_node;

    if (k == AQ_NORMAL) queue->num_messages++;

    pthread_cond_signal(&queue->cond_not_empty);
    pthread_mutex_unlock(&queue->lock);
    return 0;
}

// Receive a message from the queue
int aq_recv(AlarmQueue aq, void **msg) {
    if (!aq || !msg) return AQ_NULL_MSG;

    AlarmQueueImpl *queue = (AlarmQueueImpl *)aq;
    pthread_mutex_lock(&queue->lock);

    while (!queue->head) {
        pthread_cond_wait(&queue->cond_not_empty, &queue->lock);
    }

    MsgNode *node = queue->head;
    MsgNode *prev = NULL;

    // Check for alarm messages first
    if (queue->alarm_present) {
        while (node && node->kind != AQ_ALARM) {
            prev = node;
            node = node->next;
        }
        if (node) {
            if (prev) {
                prev->next = node->next;
            } else {
                queue->head = node->next;
            }
            if (node == queue->tail) queue->tail = prev;
            queue->alarm_present = 0;
            pthread_cond_signal(&queue->cond_no_alarm);
        }
    } else {
        // No alarm present, handle normal messages
        node = queue->head;
        queue->head = node->next;
        if (!queue->head) queue->tail = NULL;
        queue->num_messages--;
    }

    if (!node) {
        pthread_mutex_unlock(&queue->lock);
        return AQ_NO_MSG;
    }

    *msg = node->msg;
    int kind = node->kind;
    free(node);

    pthread_mutex_unlock(&queue->lock);
    return kind;
}

// Destroy the alarm queue
void aq_destroy(AlarmQueue aq) {
    if (!aq) return;

    AlarmQueueImpl *queue = (AlarmQueueImpl *)aq;
    pthread_mutex_lock(&queue->lock);

    MsgNode *current = queue->head;
    while (current) {
        MsgNode *next = current->next;
        free(current->msg);
        free(current);
        current = next;
    }

    pthread_mutex_unlock(&queue->lock);
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->cond_not_empty);
    pthread_cond_destroy(&queue->cond_no_alarm);

    free(queue);
}

// Get the size of the queue (number of messages)
int aq_size(AlarmQueue aq) {
    if (!aq) return AQ_UNINIT;

    AlarmQueueImpl *queue = (AlarmQueueImpl *)aq;
    pthread_mutex_lock(&queue->lock);
    int size = queue->num_messages + queue->alarm_present;
    pthread_mutex_unlock(&queue->lock);
    return size;
}

// Get the number of alarm messages in the queue (0 or 1)
int aq_alarms(AlarmQueue aq) {
    if (!aq) return AQ_UNINIT;

    AlarmQueueImpl *queue = (AlarmQueueImpl *)aq;
    pthread_mutex_lock(&queue->lock);
    int alarms = queue->alarm_present ? 1 : 0;
    pthread_mutex_unlock(&queue->lock);
    return alarms;
}
