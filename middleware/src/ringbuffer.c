/**
 * @file
 * @brief A ringbuffer implementation on the stack.
 *
 * @copyright Copyright Northeastern Electric Racing 2025. All rights reserved.
 * 
 */

#include "ringbuffer.h"

#include <assert.h>

struct ringbuf_t {
	void **buf;
	size_t size; /* Maximum number of elements that can be in the ringbuffer */
	size_t head_idx;
	size_t curr_elements; /* Number of elements in the RB */
};

/**
 * @brief Get the nth element from the head. Checks that you are getting 
 * a valid value with asserts. n=0 gets the head of the RB.
 * 
 * @param rb Pointer to ringbuffer.
 * @param n nth element to get.
 * @return void* The value at that location
 */
static void *get_nth_element(const struct ringbuf_t *rb, size_t n)
{
	assert(rb);
	assert(n < rb->size);
	assert(rb->curr_elements > ((rb->head_idx - n) % rb->size));

	return rb->buf[(rb->head_idx - n) % rb->size];
}

void ringbuffer_init(struct ringbuf_t *rb, void **buffer, size_t size)
{
	rb->buf = buffer;
	rb->size = size;
	rb->head_idx = 0;
}

void *ringbuffer_get_head(const struct ringbuf_t *rb)
{
	assert(rb);

	return rb->buf[rb->head_idx];
}

void *ringbuffer_get_tail(const struct ringbuf_t *rb)
{
	assert(rb);

	void *ptr;

	/*  
     * If the RB has not wrapped around yet, then the 
     * tail is at 0 
     */
	if (rb->curr_elements < rb->size) {
		ptr = rb->buf[0];
	} else {
		ptr = get_nth_element(rb, rb->head_idx + 1);
	}

	return ptr;
}

void ringbuffer_get_last_n(const struct ringbuf_t *rb, void *buf, size_t n)
{
	assert(rb);
	assert(buf);

	for (size_t i = 0; i < n; i++) {
		rb->buf[i] = get_nth_element(rb, i);
	}
}

void ringbuffer_insert(const struct ringbuf_t *rb, void *data)
{
	assert(rb);
	assert(data);
	rb->buf[(rb->head_idx + 1) % rb->size] = data;
}