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

uint32_t bitstream_read(bitstream_t *bitstream, uint32_t start_bit,
			size_t num_bits)
{
	uint32_t result = 0;

	if (start_bit + num_bits > bitstream->total_bits) {
		return -1; // Error: trying to read beyond bit length
	}

	for (int i = 0; i < num_bits; ++i) {
		if (bitstream->data[(start_bit + i) / 8] &
		    (1 << (7 - ((start_bit + i) % 8)))) {
			result |= (1 << (num_bits - 1 - i));
		}
	}

	return result;
}