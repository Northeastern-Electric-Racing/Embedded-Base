#include "ringbuffer.h"
#include <stdlib.h>
#include <string.h>

ringbuffer_t *ringbuffer_create(size_t capacity, size_t element_size) {
  ringbuffer_t *rb = (ringbuffer_t *)malloc(sizeof(ringbuffer_t));
  if (!rb) {
    // Handle memory allocation failure
    return NULL;
  }

  rb->buffer = calloc(capacity, element_size);
  if (!rb->buffer) {
    free(rb);
    return NULL;
  }

  rb->capacity = capacity;
  rb->element_size = element_size;

  return rb;
}

void ringbuffer_destroy(ringbuffer_t *rb) {
  // Free each dynamically allocated element
  for (size_t i = 0; i < rb->count; i++) {
    if (rb->buffer[(rb->front + i) % rb->capacity] != NULL)
      free(rb->buffer[(rb->front + i) % rb->capacity]);
  }

  // Free the buffer of pointers
  free(rb->buffer);

  // Free the ringbuffer_t structure
  free(rb);
}

int ringbuffer_is_empty(ringbuffer_t *rb) { return rb->count == 0; }

int ringbuffer_is_full(ringbuffer_t *rb) { return rb->count == rb->capacity; }

int ringbuffer_enqueue(ringbuffer_t *rb, void *data) {
  if (ringbuffer_is_full(rb)) {
    // Buffer is full, cannot enqueue
    return -1;
  }

  // Allocate memory for the new element
  rb->buffer[rb->rear] = malloc(rb->element_size);
  if (rb->buffer[rb->rear] == NULL) {
    // Handle memory allocation failure
    return -1;
  }

  // Copy the data to the newly allocated memory
  memcpy(rb->buffer[rb->rear], data, rb->element_size);

  rb->rear = (rb->rear + 1) % rb->capacity;
  rb->count++;

  return 0; // Successful enqueue
}

void ringbuffer_dequeue(ringbuffer_t *rb, void *data) {
  if (ringbuffer_is_empty(rb)) {
    // Buffer is empty, cannot dequeue
    data = NULL;
    return;
  }

  // Copy the data from the buffer
  memcpy(data, rb->buffer[rb->front], rb->element_size);

  // Free the memory of the dequeued element
  free(rb->buffer[rb->front]);

  rb->front = (rb->front + 1) % rb->capacity;
  rb->count--;
}