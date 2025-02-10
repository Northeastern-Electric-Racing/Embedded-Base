/**
 * @file
 * @brief A ringbuffer implementation on the stack.
 *
 * @copyright Copyright Northeastern Electric Racing 2025. All rights reserved.
 *
 */

#include "ringbuffer.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

static void *get_nth_element(const struct ringbuf_t *rb, size_t n) {
  assert(rb);
  assert(n < rb->curr_elements);

  size_t index = (rb->head_idx + rb->size - rb->curr_elements + n) % rb->size;

  return (uint8_t *)rb->buf + (index * rb->element_size);
}

void rb_init(struct ringbuf_t *rb, void **buffer, size_t size) {
  rb->buf = buffer;
  rb->size = size;
  rb->element_size = element_size;
  rb->head_idx = 0;
  rb->curr_elements = 0;
}

void *rb_get_head(const struct ringbuf_t *rb) {
  assert(rb);
  assert(rb->curr_elements > 0);

  size_t last_entry_idx = (rb->head_idx + rb->size - 1) % rb->size;

  return (uint8_t *)rb->buf + (last_entry_idx * rb->element_size);
}

void *rb_dequeue(struct ringbuf_t *rb) {
  assert(rb);

  if (rb->curr_elements == 0) {
    return NULL;
  }

  size_t tail_idx = (rb->head_idx + rb->size - rb->curr_elements) % rb->size;
  void *ptr = (uint8_t *)rb->buf + (tail_idx * rb->element_size);

  rb->curr_elements--;

  return ptr;
}

void *rb_get_tail(const struct ringbuf_t *rb) {
  assert(rb);
  assert(rb->curr_elements > 0);

  size_t tail_idx = (rb->head_idx + rb->size - rb->curr_elements) % rb->size;
  return (uint8_t *)rb->buf + (tail_idx * rb->element_size);
}

void rb_get_last_n(const struct ringbuf_t *rb, void *buf, size_t n) {
  assert(rb);
  assert(buf);

  if (n > rb->curr_elements) {
    n = rb->curr_elements;
  }

  size_t start_idx = rb->curr_elements - n;

  for (size_t i = 0; i < n; i++) {
    void *src = get_nth_element(rb, start_idx + i);
    void *dest = (uint8_t *)buf + (i * rb->element_size);
    memcpy(dest, src, rb->element_size);
  }
}

void rb_insert(struct ringbuf_t *rb, void *data) {
  assert(rb);
  assert(data);

  void *dest = (uint8_t *)rb->buf + (rb->head_idx * rb->element_size);

  memcpy(dest, data, rb->element_size);

  rb->head_idx = (rb->head_idx + 1) % rb->size;

  if (rb->curr_elements < rb->size) {
    rb->curr_elements++;
  }
}