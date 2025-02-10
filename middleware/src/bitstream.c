#include "bitstream.h"

int bitstream_init(bitstream_t *bitstream, size_t length)
{
	bitstream->length = length;
	bitstream->last_bit = 0;

	if (!bitstream) {
		return 1;
	}

	bitstream->data = (uint8_t *)calloc(
		(length + 7) / 8,
		sizeof(uint8_t)); // Allocate memory for the bitstream data, and initialize all bits to 0.

	if (!bitstream->data) {
		free(bitstream->data);
		return 1; // If memory allocation fails, return 1.
	}

	return 0;
}

int bitstream_add(bitstream_t *bitstream, void *data, size_t input_length)
{
	uint8_t *input_data = (uint8_t *)data;
	input_data[input_length / 8] <<=
		8 -
		(input_length %
		 8); // Shift the last byte to the left to remove the unused bits.
	for (int i = 0; i < input_length; i++) {
		uint8_t input_bit = NER_GET_BIT(
			input_data[i / 8],
			7 - (i % 8)); // Extract the bit from the input data.

		if (input_bit) {
			NER_SET_BIT(bitstream->data[bitstream->last_bit / 8],
				    7 - (bitstream->last_bit %
					 8)); // Set the bit in the bitstream.
		} else {
			NER_CLEAR_BIT(
				bitstream->data[bitstream->last_bit / 8],
				7 - (bitstream->last_bit %
				     8)); // Clear the bit in the bitstream.
		}
		bitstream->last_bit++; // Increment the index of the last bit.
	}
	return 0;
}

uint8_t *bitstream_get_head(bitstream_t *bitstream)
{
	return bitstream->data +
	       (bitstream->last_bit /
		8); // Return a pointer to the last byte in the bitstream.
}

int bitstream_destroy(bitstream_t *bitstream)
{
	if (!bitstream || !bitstream->data) {
		return 1;
	}
	free(bitstream->data);
	bitstream->data = NULL;
	bitstream->length = 0;
	bitstream->last_bit = 0;
	return 0;
}

int bitstream_read(bitstream_t *bitstream, size_t start_index, size_t end_index,
		   void *input_buffer, size_t input_length)
{
	uint8_t *input = (uint8_t *)input_buffer;
	size_t input_bit_index = 0;
	for (size_t i = start_index; i <= end_index; i++) {
		/* Extract bit from bitstream */
		uint8_t byte_index = i / 8;
		uint8_t bit_position = 7 - (i % 8);
		uint8_t bit =
			NER_GET_BIT(bitstream->data[byte_index], bit_position);

		if (bit) {
			NER_SET_BIT(input[input_bit_index / 8],
				    7 - (input_bit_index % 8));
		} else {
			NER_CLEAR_BIT(input[input_bit_index / 8],
				      7 - (input_bit_index % 8));
		}
		input_bit_index++;
	}
	input[input_bit_index / 8] >>=
		8 -
		(input_bit_index %
		 8); // Shift the last byte to the right to remove the unused bits.
	return 0;
}