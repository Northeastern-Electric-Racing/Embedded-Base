#include "ringbuffer.h"

void initialize_ring_buffer(ring_buffer_t* rb)
{
    rb->front = 0;
    rb->rear = 0;
    rb->count = 0;
}

int is_buffer_empty(ring_buffer_t* rb) 
{
    return rb->count == 0;
}

int is_buffer_full(ring_buffer_t* rb) 
{
    return rb->count == BUFFER_SIZE;
}

int enqueue(ring_buffer_t* rb, void* data) 
{
    if (is_buffer_full(rb)) {

        /* Buffer is full, cannot enqueue */
        return -1;
    }

    rb->buffer[rb->rear] = data;
    rb->rear = (rb->rear + 1) % BUFFER_SIZE;
    rb->count++;

    return 0; // Successful enqueue
}

void* dequeue(ring_buffer_t* rb) 
{
    if (is_buffer_empty(rb)) {

        /* Buffer is empty, cannot dequeue */
        return NULL;
    }

    uint8_t* data = rb->buffer[rb->front];
    rb->front = (rb->front + 1) % BUFFER_SIZE;
    rb->count--;

    return data;
}