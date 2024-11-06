#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "aq.h"
#include "aux.h"

int main(int argc, char **argv) {
    AlarmQueue q = aq_create();
    if (q == NULL) {
        printf("Alarm queue could not be created\n");
        exit(1);
    }

    printf("Queue created successfully.\n");

    put_normal(q, 1);
    put_normal(q, 2);
    put_alarm(q, 3); // First alarm message

    /* Retrieve the first alarm message to unblock any further alarm sends */
    printf("Retrieving first alarm message.\n");
    assert(get(q) == 3);

    /* Now it's safe to attempt sending another alarm */
    put_alarm(q, 4); 

    put_normal(q, 5);
    printf("Inserted messages. Now checking queue size.\n");

    /* Now there should be 4 messages in the queue */
    assert(print_sizes(q) == 4);

    /* Retrieve messages in the correct order */
    printf("Retrieving remaining messages.\n");
    assert(get(q) == 4);
    assert(get(q) == 1);
    assert(get(q) == 2);
    assert(get(q) == 5);

    /* Now the queue should be empty */
    assert(print_sizes(q) == 0);

    /* Next get should give error */
    printf("Attempting to get from empty queue.\n");
    assert(get(q) == AQ_NO_MSG);

    aq_destroy(q);
    printf("Queue destroyed. Program completed successfully.\n");

    return 0;
}
