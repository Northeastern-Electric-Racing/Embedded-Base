/**
 * @file
 * @brief A ringbuffer implementation on the stack.
 *
 * @copyright Copyright Northeastern Electric Racing 2025. All rights reserved.
 *
 */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stddef.h>

typedef struct ringbuf_t {
  void **buf;
  size_t size; /* Maximum number of elements that can be in the ringbuffer */
  size_t head_idx;
  size_t curr_elements; /* Number of elements in the RB */
  size_t element_size;
} ringbuf_t;

/**
 * @brief Initialize a ringbuffer.
 *
 * @param rb Pointer to ringbuffer.
 * @param buffer Pointer to stack allocated buffer.
 * @param size The number of elements in the ringbuffer.
 * @param element_size The size of each element in bytes.
 */
void rb_init(struct ringbuf_t *rb, void *buffer, size_t size,
             size_t element_size);

/**
 * @brief Get the last element to be inserted in the ringbuffer.
 *
 * @param rb Pointer to ringbuffer.
 * @param ptr Pointer to read data to.
 */
void *rb_get_head(const struct ringbuf_t *rb);

/**
 * @brief Pop the head off of the RB.
 *
 * @param rb Pointer to ringbuffer.
 * @return void* Data at the head of the RB.
 */
void *rb_dequeue(struct ringbuf_t *rb);

/**
 * @brief Get the element at the tail of the ringbuffer.
 *
 * @param rb Pointer to ringbuffer.
 * @param ptr Pointer to read data to.
 */
void *rb_get_tail(const struct ringbuf_t *rb);

/**
 * @brief Get the last N elements from the head of the ringbuffer.
 *
 * @param rb Pointer to ringbuffer.
 * @param buf Buffer that data will be written to.
 * @param n Number of elements, starting at the head, to read into the buffer.
 */
void rb_get_last_n(const struct ringbuf_t *rb, void *buf, size_t n);

/**
 * @brief Insert an element at the head of the ringbuffer. Overwrites data at
 * the tail if the RB is full.
 *
 * @param rb Pointer to ringbuffer.
 * @param data The data to insert at the head of the ringbuffer.
 */
void rb_insert(struct ringbuf_t *rb, void *data);

#endif