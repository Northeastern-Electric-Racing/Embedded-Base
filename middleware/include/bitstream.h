#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	uint8_t *data; // The bitstream data, stored as bytes
	size_t bytes; // The number of bytes in the bitstream
	size_t total_bits; // Total number of bits in the bitstream
} bitstream_t;

/**
 * @brief Initializes a bitstream with the specified length.
 * @param *bitstream The bitstream to initialize.
 * @param *data The buffer to store the bitstream data.
 * @param bytes The length of the bitstream in bytes.
 */
void bitstream_init(bitstream_t *bitstream, uint8_t *data, size_t bytes);

/**
 * @brief Adds data to a bitstream. The data is added to the end of the bitstream.
 * @param *bitstream The bitstream to add data to.
 * @param value The data to add to the bitstream.
 * @param num_bits The length of the data in bits.
 * @return Returns 1 if failed, and 0 if successful.
 */
int bitstream_add(bitstream_t *bitstream, uint32_t value, size_t num_bits);

/**
 * @brief Reads info from the bitstream.
 * @param *bitstream The bitstream to read from.
 * @param start_bit The bit to start reading from.
 * @param num_bits The amount of bits to read. The maximum is 32 bits, and (start_bit + num_bits) must be less than the total number of bits in the bitstream.
 * @return Returns 1 if failed, and 0 if successful.
 */
uint32_t bitstream_read(bitstream_t *bitstream, uint32_t start_bit,
			size_t num_bits);

#endif // BITSTREAM_H