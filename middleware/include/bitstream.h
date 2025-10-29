#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// clang-format off

typedef struct {
	uint8_t *data; // The bitstream data, stored as bytes
	size_t bytes; // The number of bytes in the bitstream
	size_t total_bits; // Total number of bits in the bitstream
	bool overflow; // False by default. If bitstream_add() is called for a value larger than its allotted bits, overflow will be set to true.
} bitstream_t;

/* Private implementation functions. Please the general bitstream_add() macro. */
int _bitstream_add_unsignedint(bitstream_t *bitstream, uint32_t value, size_t num_bits);
int _bitstream_add_signedint(bitstream_t *bitstream, int32_t value, size_t num_bits);

/**
 * @brief Adds data to a bitstream. The data is added to the end of the bitstream.
 * @param *bitstream The bitstream to add data to. (bitstream_t)
 * @param value The data to add to the bitstream. (uint8_t, uint16_t uint32_t, int8_t, int16_t, int32_t, int)
 * @param num_bits The length of the data in bits. (size_t)
 * @return Returns 0 if successful, -1 if there is insufficient space in the bitstream, and 1 if overflow occurs.
 */
#define bitstream_add(bitstream, value, num_bits) \
	_Generic((value), \
		uint8_t:  _bitstream_add_unsignedint, \
		uint16_t: _bitstream_add_unsignedint, \
		uint32_t: _bitstream_add_unsignedint, \
		int8_t:   _bitstream_add_signedint,   \
		int16_t:  _bitstream_add_signedint,   \
		int32_t:  _bitstream_add_signedint,   \
		int:      _bitstream_add_signedint    \
	)((bitstream), (value), (num_bits))

/**
 * @brief Initializes a bitstream with the specified length.
 * @param *bitstream The bitstream to initialize.
 * @param *data The buffer to store the bitstream data.
 * @param bytes The length of the bitstream in bytes.
 */
void bitstream_init(bitstream_t *bitstream, uint8_t *data, size_t bytes);

// clang-format on

#endif // BITSTREAM_H