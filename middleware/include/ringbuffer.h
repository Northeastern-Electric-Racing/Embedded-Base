#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stddef.h>
#include <stdint.h>

#endif // RINGBUFFER_H

typedef struct {
    void** buffer;
    size_t capacity;
    size_t element_size;
    size_t front;
    size_t rear;
    size_t count;
    
} ringbuffer_t;

ringbuffer_t* ringbuffer_create(size_t capacity, size_t element_size);

int ringbuffer_is_empty(ringbuffer_t* rb);

int ringbuffer_is_full(ringbuffer_t* rb);

int ringbuffer_enqueue(ringbuffer_t* rb, void* data);

void* ringbuffer_dequeue(ringbuffer_t* rb, void* data);