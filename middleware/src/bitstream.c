#include "bitstream.h"

uint8_t bitstream_create(bitstream_t *bitstream, size_t length)
{
	bitstream->length = length;
	bitstream->current_bit = 0;

	if (!bitstream) {
		return 1;
	}

	bitstream->data = (uint8_t *)calloc((length + 7) / 8, sizeof(uint8_t));

	if (!bitstream->data) {
		return 1;
	}

	return 0;
}

uint8_t bitstream_add(bitstream_t *bitstream, void *data, size_t input_length)
{
	uint8_t *input_data = (uint8_t *)data;
	if (bitstream->current_bit + input_length > bitstream->length) {
		return 1; // If the bitstream does not have enough space, return 1.
	}

	for (int i = 0; i < input_length; i++) {
		size_t input_byte_index = i / 8;
		size_t input_bit_index = 7 - (i % 8);
		uint8_t input_bit =
			(input_data[input_byte_index] >> input_bit_index) & 1;

		size_t bitstream_byte_index = bitstream->current_bit / 8;
		size_t bitstream_bit_index = 7 - (bitstream->current_bit % 8);

		/* If the input bit is 1, set the corresponding bit in the bitstream to 1. Otherwise, set it to 0. */
		if (input_bit) {
			bitstream->data[bitstream_byte_index] |=
				1 << bitstream_bit_index;
		} else {
			bitstream->data[bitstream_byte_index] &=
				~(1 << bitstream_bit_index);
		}

		bitstream->current_bit++; // Increment the current bit index.
	}

	return 0;
}