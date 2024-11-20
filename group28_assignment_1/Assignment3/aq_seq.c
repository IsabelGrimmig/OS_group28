/**
 * @file   aq_seq.c
 * @Author 02335 team
 * @date   November, 2024
 * @brief  Sequential implementation of the alarm queue
 */

#include "aq.h"
#include <stdlib.h>

//Structure for each message
typedef struct MsgNode {
    void *msg;
    MsgKind kind;
    struct MsgNode *next;
} MsgNode;

//Structure for the alarm queue
typedef struct {
    MsgNode *Msg_head;   // Head of the message queue
    MsgNode *Msg_tail;   // Tail of the message queue
    void *Msg_alarm;     // Single slot for an alarm message
    int num_msg;         // Number of normal messages
    int has_alarm;       // 1 if an alarm message is present, 0 otherwise
} MsgQueueStruct;

//Creation of the alarm queue
AlarmQueue aq_create() {
    MsgQueueStruct *queue = (MsgQueueStruct *)malloc(sizeof(MsgQueueStruct));
    if (!queue) return NULL;

    //Initialize the queue structure
    queue->Msg_head = NULL;
    queue->Msg_tail = NULL;
    queue->Msg_alarm = NULL;
    queue->num_msg = 0;
    queue->has_alarm = 0;

    return (AlarmQueue)queue;
}

//puts a message to the queue
int aq_send(AlarmQueue aq, void *msg, MsgKind kind) {
    if (!aq) return AQ_UNINIT;
    if (!msg) return AQ_NULL_MSG;

    MsgQueueStruct *queue = (MsgQueueStruct *)aq;

    if (kind == AQ_ALARM) {
        //If an alarm is already present, return error
        if (queue->has_alarm) {
            return AQ_NO_ROOM;
        }
        queue->Msg_alarm = msg;
        queue->has_alarm = 1;
    } else if (kind == AQ_NORMAL) {
        //Adds a normal message to the queue
        MsgNode *new_node = (MsgNode *)malloc(sizeof(MsgNode));
        if (!new_node) return AQ_NO_ROOM;

        new_node->msg = msg;
        new_node->kind = AQ_NORMAL;
        new_node->next = NULL;

        if (queue->Msg_tail) {
            queue->Msg_tail->next = new_node;
        } else {
            queue->Msg_head = new_node;
        }
        queue->Msg_tail = new_node;
        queue->num_msg++;
    } else {
        return AQ_NOT_IMPL; //Unsupported message kind
    }

    return 0; //Success
}

//Receive a message from the queue
int aq_recv(AlarmQueue aq, void **msg) {
    if (!aq) return AQ_UNINIT;
    if (!msg) return AQ_NULL_MSG;

    MsgQueueStruct *queue = (MsgQueueStruct *)aq;

    //If the queue is empty, return error
    if (!queue->Msg_head && !queue->has_alarm) {
        return AQ_NO_MSG;
    }

    MsgKind kind;
    if (queue->has_alarm) {
        //Retrieve the alarm message
        *msg = queue->Msg_alarm;
        queue->Msg_alarm = NULL;
        queue->has_alarm = 0;
        kind = AQ_ALARM;
    } else {
        //Retrieve a normal message
        MsgNode *node = queue->Msg_head;
        *msg = node->msg;
        queue->Msg_head = node->next;

        if (!queue->Msg_head) {
            queue->Msg_tail = NULL;
        }
        free(node);
        queue->num_msg--;
        kind = AQ_NORMAL;
    }

    return kind;
}

//Get the number of messages in the queue 
int aq_size(AlarmQueue aq) {
    if (!aq) return AQ_UNINIT;

    MsgQueueStruct *queue = (MsgQueueStruct *)aq;
    return queue->num_msg + queue->has_alarm;
}

//Get the number of alarm messages in the queue
int aq_alarms(AlarmQueue aq) {
    if (!aq) return AQ_UNINIT;

    MsgQueueStruct *queue = (MsgQueueStruct *)aq;
    return queue->has_alarm ? 1 : 0;
}

//Destroy the alarm queue
void aq_destroy(AlarmQueue aq) {
    if (!aq) return;

    MsgQueueStruct *queue = (MsgQueueStruct *)aq;

    //Free all non-alarm messages in the queue
    MsgNode *current = queue->Msg_head;
    while (current) {
        MsgNode *next = current->next;
        free(current->msg); // Free the message payload
        free(current);      // Free the node
        current = next;
    }

    //Free the alarm message if ther is one
    if (queue->has_alarm) {
        free(queue->Msg_alarm);
    }

    free(queue);// Free the queue 
}

