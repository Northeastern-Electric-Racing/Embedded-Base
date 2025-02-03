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

typedef struct ringbuf_t ringbuf_t;

/**
 * @brief Initialize a ringbuffer.
 * 
 * @param rb Pointer to ringbuffer.
 * @param buffer Pointer to stack allocated buffer.
 * @param size The number of elements in the ringbuffer.
 */
void ringbuffer_init(struct ringbuf_t *rb, void **buffer, size_t size);

/**
 * @brief Get the last element to be inserted in the ringbuffer.
 * 
 * @param rb Pointer to ringbuffer.
 * @param ptr Pointer to read data to.
 */
void *ringbuffer_get_head(const struct ringbuf_t *rb);

/**
 * @brief Get the element at the tail of the ringbuffer.
 * 
 * @param rb Pointer to ringbuffer.
 * @param ptr Pointer to read data to.
 */
void *ringbuffer_get_tail(const struct ringbuf_t *rb);

/**
 * @brief Get the last N elements from the head of the ringbuffer.
 * 
 * @param rb Pointer to ringbuffer.
 * @param buf Buffer that data will be written to.
 * @param n Number of elements, starting at the head, to read into the buffer.
 */
void ringbuffer_get_last_n(const struct ringbuf_t *rb, void *buf, size_t n);

/**
 * @brief Insert an element at the head of the ringbuffer.
 * 
 * @param rb Pointer to ringbuffer.
 * @param data The data to insert at the head of the ringbuffer.
 */
void ringbuffer_insert(const struct ringbuf_t *rb, void *data);

#endif