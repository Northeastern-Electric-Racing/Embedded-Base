#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <stdlib.h>
#include "c_utils.h"

typedef struct {
	uint8_t *data; // The bitstream data, stored as bytes
	size_t length; // The length of the bitstream in bits
	size_t last_bit; // The current last bit in the bitstream
} bitstream_t;

/**
 * @brief Initializes a bitstream with the specified length.
 * @param {bitstream_t*} The bitstream to initialize.
 * @param {size_t} The length of the bitstream in bits.
 * @return Returns 1 if failed, and 0 if successful.
 */
int bitstream_init(bitstream_t *bitstream, size_t length);

/**
 * @brief Adds data to a bitstream. The data is added to the end of the bitstream.
 * @param {bitstream_t*} The bitstream to add data to.
 * @param {void*} The data to add to the bitstream.
 * @param {size_t} The length of the data in bits.
 * @return Returns 1 if failed, and 0 if successful.
 */
int bitstream_add(bitstream_t *bitstream, void *data, size_t input_length);

/**
 * @brief Gets a pointer to the last byte in the bitstream.
 * @param {bitstream_t*} The bitstream to get the head of.
 * @return Returns a pointer to the last byte in the bitstream.
 */
uint8_t *bitstream_get_head(bitstream_t *bitstream);

/**
 * @brief Reads a range of bits from a bitstream. The minimum index is 0, and the maximum index is bitstream->last_bit.
 * @param {bitstream_t*} The bitstream to read from.
 * @param {size_t} The index of the first bit in the range. (Inclusive)
 * @param {size_t} The index of the last bit in the range. (Inclusive)
 * @param {void*} The output buffer to write the bits to.
 * @param {size_t} The length of the output buffer in bits.
 * @return Returns 1 if failed, and 0 if successful.
 */
int bitstream_read(bitstream_t *bitstream, size_t start_bit, size_t end_bit,
		   void *output, size_t output_length);

#endif // BITSTREAM_H