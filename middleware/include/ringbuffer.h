#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stddef.h>
#include <stdint.h>

#endif // RINGBUFFER_H

#define BUFFER_SIZE 10

typedef struct {
    uint8_t* buffer[BUFFER_SIZE];
    size_t front;
    size_t rear;
    size_t count;
} ring_buffer_t;

void initialize_ring_buffer(ring_buffer_t* rb);

int is_buffer_empty(ring_buffer_t* rb);

int is_buffer_full(ring_buffer_t* rb);

int enqueue(ring_buffer_t* rb, void* data);

uint8_t* dequeue(ring_buffer_t* rb);