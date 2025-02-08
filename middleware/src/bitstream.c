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
	if (bitstream->last_bit + input_length > bitstream->length) {
		return 1; // If the bitstream does not have enough space, return 1.
	}

	/* Iterate over each bit in the input data and add it to the bitstream. */
	for (int i = 0; i < input_length; i++) {
		size_t input_byte_index = i / 8;
		size_t input_bit_index = 7 - (i % 8);
		uint8_t input_bit = NER_GET_BIT(
			input_data[input_byte_index],
			input_bit_index); // Extract the bit from the input data.

		size_t bitstream_byte_index = bitstream->last_bit / 8;
		size_t bitstream_bit_index = 7 - (bitstream->last_bit % 8);

		/* If the input bit is 1, set the corresponding bitstream bit (Make it 1). Otherwise, clear the corresponding bitstream bit (Make it 0). */
		if (input_bit) {
			NER_SET_BIT(
				bitstream->data[bitstream_byte_index],
				bitstream_bit_index); // Set the bit (Make it 1).
		} else {
			NER_CLEAR_BIT(
				bitstream->data[bitstream_byte_index],
				bitstream_bit_index); // Clear the bit (Make it 0).
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
		   void *output_buffer, size_t output_length)
{
	size_t range_length = end_index - start_index + 1;
	uint8_t *output = (uint8_t *)output_buffer;

	/* Handle Erros */
	if (range_length > output_length) {
		return 1; // If the output buffer is not large enough to hold the requested bits, return 1.
	}

	if (start_index > end_index) {
		return 1; // If the start bit is invalid, return 1.
	}

	if (end_index > bitstream->last_bit) {
		return 1; // If the end bit is invalid, return 1.
	}

	/* Iterate over each bit in the bitstream and copy it to the output data. */
	for (int i = 0; i < range_length; i++) {
		size_t current_bit = start_index + i;
		size_t bitstream_byte_index = current_bit / 8;
		size_t bitstream_bit_index = 7 - (current_bit % 8);
		uint8_t bitstream_bit = NER_GET_BIT(
			bitstream->data[bitstream_byte_index],
			bitstream_bit_index); // Extract the bit from the bitstream.

		size_t output_byte_index = i / 8;
		size_t output_bit_index = 7 - (i % 8);

		if (bitstream_bit) {
			NER_SET_BIT(
				output[output_byte_index],
				output_bit_index); // Set the bit in the output data.
		} else {
			NER_CLEAR_BIT(
				output[output_byte_index],
				output_bit_index); // Clear the bit in the output data.
		}
	}

	return 0;
}

int bitstream_modify(bitstream_t *bitstream, size_t start_index,
		     size_t end_index, void *input_buffer, size_t input_length)
{
	size_t range_length = end_index - start_index + 1;
	uint8_t *input = (uint8_t *)input_buffer;

	/* Handle Erros */
	if (range_length > input_length) {
		return 1; // If the range is larger than the input buffer (meaning there is not enough information to modify all requested bits), return 1.
	}

	if (start_index > end_index) {
		return 1; // If the start bit is invalid, return 1.
	}

	if (end_index > bitstream->last_bit) {
		return 1; // If the end bit is invalid, return 1.
	}

	/* Iterate over each bit in the input and copy it to the bitstream. */
	for (int i = 0; i < range_length; i++) {
		size_t input_byte_index = i / 8;
		size_t input_bit_index = 7 - (i % 8);
		uint8_t input_bit = NER_GET_BIT(
			input[input_byte_index],
			input_bit_index); // Extract the bit from the input data.

		size_t current_bit = start_index + i;
		size_t bitstream_byte_index = current_bit / 8;
		size_t bitstream_bit_index = 7 - (current_bit % 8);

		if (input_bit) {
			NER_SET_BIT(
				bitstream->data[bitstream_byte_index],
				bitstream_bit_index); // Set the bit in the bitstream.
		} else {
			NER_CLEAR_BIT(
				bitstream->data[bitstream_byte_index],
				bitstream_bit_index); // Clear the bit in the bitstream.
		}
	}

	return 0;
}