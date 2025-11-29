#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
	uint8_t *data; // The bitstream data, stored as bytes
	size_t bytes; // The number of bytes in the bitstream
	size_t total_bits; // Total number of bits in the bitstream
	bool overflow; // False by default. If bitstream_add() is called for a value larger than its allotted bits, overflow will be set to true.
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
 * @param bitstream The bitstream to add data to.
 * @param value The data to add to the bitstream.
 * @param num_bits The length of the data in bits.
 * @return Returns 0 if successful, -1 if there is insufficient space in the bitstream, and 1 if overflow occurs.
 */
int bitstream_add(bitstream_t *bitstream, uint32_t value, size_t num_bits);

/**
 * @brief Removes from the bitstream. The data is popped from the stream
 * @param bitstream The bitstream to add data to
 * @param num_bits number of bits to remove from the bitstream
 * @return bits popped from the stream decoded as an integer, -1 if unsuccessful
 */
int bitstream_remove(bitstream_t *bitstream, size_t num_bits)

/**
 * @brief Adds a signed int to a bitstream. The data is added to the end of the bitstream.
 * @param *bitstream The bitstream to add data to.
 * @param value The data to add to the bitstream.
 * @param num_bits The length of the data in bits.
 * @return Returns 0 if successful, -1 if there is insufficient space in the bitstream, and 1 if overflow occurs.
 */
int bitstream_add_signed(bitstream_t *bitstream, int32_t value,
			 size_t num_bits);

#endif // BITSTREAM_H