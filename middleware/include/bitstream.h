#include <stdint.h>
#include <stdlib.h>

typedef struct {
	uint8_t *data; // The bitstream data, stored as bytes
	size_t length; // The length of the bitstream in bits
	size_t current_bit; // The current last bit in the bitstream
} bitstream_t;