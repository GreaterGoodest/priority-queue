/*
    Creates a priority queue based on a heap data structure which allows for
    adding, popping, and peeking Messages.

    The heap data structure allows for logarithmic time operations O(logn)
    This is better than a linked list, which would be linear O(n)
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MSG_SIZE 1024

typedef struct _message {
    uint32_t id; //used to determine if message is for a specific function
    uint8_t command; //DATA, GET, etc...
    uint8_t priority;
    uint8_t error_code;
    size_t  data_size;
    uint8_t data[MSG_SIZE];
} message_t;

typedef struct _message_queue {
    message_t **messages;
    size_t queue_size; //expandable if we want
    size_t last_element;  //where to place the next message before bubble up
} message_queue_t;

message_queue_t *create_queue(size_t queue_size)
{
    message_queue_t *new_queue = calloc(1, sizeof(message_queue_t));

    new_queue->messages = calloc(queue_size, sizeof(uint64_t));
    new_queue->queue_size = queue_size;
    new_queue->last_element = 0;

    return new_queue;
}

uint8_t free_queue(message_queue_t *message_queue)
{
    ssize_t last_element = message_queue->last_element; //point to last actual piece of data

    last_element -= 1;
    while (last_element >= 0)
    {
        if(message_queue->messages[last_element]){
            printf("freeing: %s\n", message_queue->messages[last_element]->data);
            free(message_queue->messages[last_element]);
        }else{ puts("empty message slot"); }

        last_element -= 1;
    }

    free(message_queue);
}

uint8_t add_message(message_queue_t *queue, uint32_t id, uint8_t command, uint8_t priority, uint8_t error_code, uint32_t data_size, const uint8_t *data)
{
    message_t *new_message = calloc(sizeof(message_t), 1);
    uint32_t curr_index = queue->last_element;

    if (data_size > MSG_SIZE)
    {
        puts("Message data is too large");
        return -1;
    }

    new_message->id = id;
    new_message->command = command;
    new_message->priority = priority;
    new_message->error_code = error_code;
    new_message->data_size = data_size;
    memcpy(new_message->data, data, data_size);
    queue->messages[curr_index] = new_message;

    uint32_t parent_index = queue->last_element / 2;
    while(queue->messages[curr_index]->priority < queue->messages[parent_index]->priority)
    {
        message_t *parent_message = queue->messages[parent_index];
        
        /* swap child and parent */
        queue->messages[parent_index] = new_message;
        queue->messages[curr_index] = parent_message;

        curr_index = parent_index;
        parent_index /= 2;
    } 

    queue->last_element++;
    if (queue->last_element >= queue->queue_size)
    {
        //expand queue
    }
    return 0;
}

message_t* peek_queue(message_queue_t *queue)
{
    return queue->messages[0];
}

uint8_t pop_queue(message_queue_t *queue, uint8_t *data)
{
    message_t *top_message = queue->messages[0]; 

    memcpy(data, top_message->data, top_message->data_size);

    /*move last message to top of queue*/
    queue->last_element -= 1;
    message_t *last_message = queue->messages[queue->last_element];
    queue->messages[0] = last_message;

    /*bubble down to appropriate position*/
    size_t curr_index = 0;
    size_t left_child_i = 1;
    size_t right_child_i = 2;
    while (
        queue->messages[curr_index] > queue->messages[left_child_i] ||
        queue->messages[curr_index] > queue->messages[right_child_i]
        )
    {
        left_child_i = curr_index * 2 + 1;
        right_child_i = curr_index * 2 + 2;
    }

    queue->messages[queue->last_element] = NULL;
}

int main()
{
    int retval = 0;

    message_queue_t *message_queue = create_queue(100);

    retval = add_message(message_queue,0,1,1,0,5,"hello");
    if (retval != 0)
    {
        puts("Failed to add message");
        return 1;
    }
    retval = add_message(message_queue,0,1,0,0,4,"test");
    
    puts("Priority check:");
    printf("data: %s, priority %d\n", message_queue->messages[0]->data, message_queue->messages[0]->priority);
    printf("data: %s, priority %d\n", message_queue->messages[1]->data, message_queue->messages[1]->priority);

    puts("Peek test:");
    message_t *tmp_message = peek_queue(message_queue);
    printf("data: %s, priority %d\n", tmp_message->data, tmp_message->priority);

    puts("Pop test:");
    char data[1024] = {0};
    pop_queue(message_queue, data);
    printf("data: %s\n", data);

    free_queue(message_queue);
}