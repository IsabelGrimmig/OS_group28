/**
 * @file   aq.c
 * @Author 02335 team
 * @date   October, 2024
 * @brief  Alarm queue skeleton implementation
 */

#include "aq.h"
#include <bits/pthreadtypes.h>

typedef struct MsgNode 
{
  void *msg;
  MsgKind kind;
  struct MsgNode *next;
} MsgNode;

typedef struct
{
  MsgNode *Msg_head;            // Head of message queue
  MsgNode *Msg_tail;            // Tail of message queue
  void *Msg_alarm;              // Single slot for one alarm message
  pthread_mutex_t queue_mutex;  // Mutex for synchronizing access
  pthread_cond_t cond_not_empty;// Condition for message availability
  pthread_cond_t cond_no_alarm; // Condition for alarm slot availability
  int num_msg;                  // Number of messages, not counting alarms
  int has_alarm;                // Flag indicating existence of an alarm msg
} MsgQueueStruct;

static void cleanup_queue (MsgQueueStruct *queue);

// Create new alarm queue
AlarmQueue aq_create() {
  // Allocate memory for the structure
  MsgQueueStruct *queue = (MsgQueueStruct *)malloc(sizeof(MsgQueueStruct));
  if (!queue) return NULL; // Return NULL if allocation fails

  // Initialize queue structure
  queue->Msg_head = queue->Msg_tail = NULL;
  queue->Msg_alarm = NULL;
  queue->num_msg = 0;
  queue->has_alarm = 0;

  //Initialize mutex and cond var's
  if (pthread_mutex_init(&queue->queue_mutex, NULL) != 0 ||
      pthread_cond_init(&queue->cond_not_empty, NULL) != 0 ||
      pthread_cond_init(&queue->cond_no_alarm, NULL) != 0) {
      
      // If initialization fails, clean and return NULL
      free(queue);
      return(NULL);
    }
    return (AlarmQueue)queue;
}

int aq_send( AlarmQueue aq, void * msg, MsgKind kind){
  if (!aq) return AQ_UNINIT;
  if (!msg) return AQ_NULL_MSG;

  MsgQueueStruct *queue = (MsgQueueStruct *)aq;
  pthread_mutex_lock(&queue->queue_mutex);

  if (kind == AQ_ALARM) {
    // if there is an alarm message, wait until received
   
    while (queue->has_alarm){
      pthread_cond_wait(&queue->cond_no_alarm, &queue->queue_mutex);
    }
    // Set alarm msg and mark active
    queue->Msg_alarm = msg;
    queue->has_alarm = 1;

  } else if (kind == AQ_NORMAL) {
    // add msg to the queue
    MsgNode *new_node = (MsgNode *)malloc(sizeof(MsgNode));
    if (!new_node){
      pthread_mutex_unlock(&queue->queue_mutex);
      return AQ_NO_ROOM;
    }
    new_node->msg = msg;
    new_node->kind = AQ_NORMAL;
    new_node->next = NULL;

    if (queue->Msg_tail){
      queue->Msg_tail->next = new_node;
      
    } else {
      queue->Msg_head = new_node;
    }
    queue->Msg_tail = new_node;
    queue->num_msg++;

  } else {
    pthread_cond_signal(&queue->cond_not_empty);
    pthread_mutex_unlock(&queue->queue_mutex);
    return AQ_NOT_IMPL; // In case of a not implemented msg
  }

  pthread_cond_signal(&queue->cond_not_empty); // point that space is available
  pthread_mutex_unlock(&queue->queue_mutex);
  //return AQ_SUCCESS; //NOT IMPLEMENTED
}

int aq_recv( AlarmQueue aq, void **msg) {
  if (!aq) return AQ_UNINIT;
  if (!msg) return AQ_NULL_MSG;

  MsgQueueStruct *queue = (MsgQueueStruct *)aq;
  pthread_mutex_lock(&queue->queue_mutex);

  //wait when there is a msg
  while (!queue->has_alarm && queue->num_msg == 0){
    pthread_cond_wait(&queue->cond_not_empty, &queue->queue_mutex);
  }

  if (!queue->Msg_head && !queue->has_alarm){
    pthread_mutex_unlock(&queue->queue_mutex);
    return AQ_NO_MSG; 
  }

  MsgKind kind;
  if (queue->has_alarm) {
    //Retrieve alarm
    *msg = queue->Msg_alarm;
    queue->Msg_alarm = NULL;
    queue->has_alarm = 0;
    kind = AQ_ALARM;

    //signal alarms is free
    pthread_cond_signal(&queue->cond_no_alarm);
  } else {
    //retrieve msg
    MsgNode *node = queue->Msg_head;
    *msg = node->msg;
    queue->Msg_head = node->next

    if (!queue->Msg_head) {
      queue->Msg_tail = NULL;
    }
    queue->num_msg--;
    kind = AQ_NORMAL;

    free(node);
  }

  pthread_mutex_unlock(&queue->queue_mutex);
  return kind;
  
}

int aq_size( AlarmQueue aq) {
  if (!aq) return AQ_UNINIT;

  MsgQueueStruct *queue = (MsgQueueStruct *)aq;
  pthread_mutex_lock(&queue->queue_mutex);
  int size = queue->num_msg + queue->has_alarm;
  pthread_mutex_unlock(&queue->queue_mutex);

  return size;
}

int aq_alarms( AlarmQueue aq) {
  if (!aq) return AQ_UNINIT;

  MsgQueueStruct *queue = (MsgQueueStruct *)aq;
  pthread_mutex_lock(&queue->queue_mutex);
  int alarms = queue->has_alarm;
  pthread_mutex_unlock(&queue->queue_mutex);

  return alarms;
}

//Cleaning process
void aq_clean(AlarmQueue aq){
  if (!aq) return;

    MsgQueueStruct *queue = (MsgQueueStruct *)aq;

    pthread_mutex_lock(&queue->queue_mutex);

    MsgNode *current = queue->Msg_head;
    while (current){
      MsgNode *next = current->next; 
      free(current->msg);            
      free(current);                 
      current = next;                
    }

    queue->Msg_head = NULL;
    queue->Msg_tail = NULL;
    queue->num_msg = 0;

    // Frigør alarmbeskeden, hvis den findes
    if (queue->has_alarm)
    {
      free(queue->Msg_alarm);  // Frigør alarmdata
      queue->Msg_alarm = NULL; // Nulstil alarmpegeren
      queue->has_alarm = 0;    // Nulstil alarmflaget
    }

    pthread_mutex_unlock(&queue->queue_mutex);

    // Ødelæg mutex og condition variables
    pthread_mutex_destroy(&queue->queue_mutex);
    pthread_cond_destroy(&queue->cond_not_empty);
    pthread_cond_destroy(&queue->cond_no_alarm);

    // Frigør selve kø-strukturen
    free(queue);
  }


//internal func to free all msg's in queue
static void cleanup_queue(MsgQueueStruct *queue) {
  MsgNode *current = queue->Msg_head;
  while (current) {
    MsgNode *next = current->next;
    free(current);
    current = next;
  }

  queue->Msg_head = queue->Msg_tail = NULL;
  queue->num_msg = 0;

  if (queue->has_alarm) {
    free(queue->Msg_alarm);
    queue->Msg_alarm = NULL;
    queue->has_alarm = 0;
  }
}

