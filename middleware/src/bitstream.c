#include "bitstream.h"

void bitstream_init(bitstream_t *bitstream, uint8_t *data, size_t bytes)
{
	bitstream->data = data;
	bitstream->total_bits = 0;
	bitstream->bytes = bytes;
	bitstream->overflow = false;
	memset(bitstream->data, 0, bytes); // Zero out the buffer
}

int bitstream_add(bitstream_t *bitstream, uint32_t value, size_t num_bits)
{
	if (bitstream->total_bits + num_bits > (bitstream->bytes * 8)) {
		return -1; // Error: not enough space in the bitstream
	}

	bool overflow = false;
	if (value >= pow(2, num_bits)) {
		overflow =
			true; // Error: value is too large for the number of bits, so set overflow to true.
		value = pow(2, num_bits) -
			1; // Cap value to the maximum value that can be stored in the number of bits.
	}

	for (int i = 0; i < num_bits; ++i) {
		if (value & (1 << (num_bits - 1 - i))) {
			bitstream->data[(bitstream->total_bits + i) / 8] |=
				(1 << (7 - ((bitstream->total_bits + i) % 8)));
		}
	}

	bitstream->total_bits += num_bits;

	if (overflow) {
		bitstream->overflow = true;
		return 1; // Overflow occurred
	}

	return 0; // Success
}

int bitstream_remove(bitstream_t *bitstream, size_t num_bits)
{
    if (num_bits > bitstream->total_bits) {
        return -1;
    }

    uint32_t value = 0;
    size_t start_bit = bitstream->total_bits - num_bits;

    for (size_t i = 0; i < num_bits; ++i) {
        size_t bit_index = start_bit + i;
        size_t byte_index = bit_index / 8;
        size_t bit_in_byte = 7 - (bit_index % 8);

        uint8_t bit = (bitstream->data[byte_index] >> bit_in_byte) & 1;

        value = (value << 1) | bit;

        // Clear the bit from the buffer
        bitstream->data[byte_index] &= ~(1 << bit_in_byte);
    }

    bitstream->total_bits -= num_bits;

    return (int)value;
}

int bitstream_add_signed(bitstream_t *bitstream, int32_t value, size_t num_bits)
{
	if (bitstream->total_bits + num_bits > (bitstream->bytes * 8)) {
		return -1; // Error: not enough space in the bitstream
	}

	bool overflow = false;
	/* For a signed int, 'value' must be in between -2^(num_bits - 1) and 2^(num_bits-1) - 1. */
	/* For example, an 6-bit input value must be in-between -32 and 31. */
	int32_t max_value = (1LL << (num_bits - 1)) - 1;
	int32_t min_value = -(1LL << (num_bits - 1));

	if (value > max_value || value < min_value) {
		overflow = true; // Error: value is too large or too small

		/* Cap value to maximum or minimum */
		if (value > max_value) {
			value = max_value;
		} else {
			value = min_value;
		}
	}

	/* Create a mask for the num_bits we want to extract */
	uint32_t mask = (1u << num_bits) - 1;
	/* Extract the bits we want, including the sign bit */
	uint32_t bits = (uint32_t)value & mask;

	for (int i = 0; i < num_bits; ++i) {
		if (bits & (1u << (num_bits - 1 - i))) {
			bitstream->data[(bitstream->total_bits + i) / 8] |=
				(1 << (7 - ((bitstream->total_bits + i) % 8)));
		}
	}

	bitstream->total_bits += num_bits;

	if (overflow) {
		bitstream->overflow = true;
		return 1; // Error: Overflow occurred
	}

	return 0; // Success
}