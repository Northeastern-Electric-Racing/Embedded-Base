#ifndef C_UTILS
#define C_UTILS

/*
 * Will retrieve the container of a certain pointer given the container type and
 * its pointer Trick pulled from Linux Kernel
 */
#define CONTAINER_OF(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))

/*
 * Gets a bit of a binary number at desired location
*/
#define NER_GET_BIT(num, bit) ((num >> bit) & 1)

/*
 * Sets a bit of a binary number at desired location
*/
#define NER_SET_BIT(num, bit) \
	bit < (sizeof(num) * 8) ? (num |= (1UL << bit)) : num

/*
 * Clears a bit of a binary number at desired location
*/
#define NER_CLEAR_BIT(num, bit) \
	bit < (sizeof(num) * 8) ? (num &= ~(1UL << bit)) : num

/*
 * Extracts a bit from a byte.
 * @param byte Byte to extract the bit from
 * @param bit_index Index of the bit to extract
 * @return The extracted bit (either 1 or 0)
*/
#define EXTRACT_BIT(byte, bit_index) ((byte >> (bit_index)) & 0x01)

/**
 * Macro for returning the min of two numbers
 * @param a first number
 * @param b second number
 */
#define min(a, b)                       \
	({                              \
		__typeof__(a) _a = (a); \
		__typeof__(b) _b = (b); \
		_a < _b ? _a : _b;      \
	})

void endian_swap(void *ptr, int size);

/// @brief Reverses the bit order of a byte
/// @param b byte
/// @return the same byte but wuth the bits reversed
unsigned char reverse_bits(unsigned char b);

/**
 * @brief Performs linear interpolation between two points.
 *
 * Computes the interpolated y-value corresponding to x, given two
 * reference points (x1, y1) and (x2, y2).
 *
 * @param x   Input value at which to interpolate.
 * @param x1  First reference x-coordinate.
 * @param x2  Second reference x-coordinate.
 * @param y1  y-value at x1.
 * @param y2  y-value at x2.
 *
 * @return Interpolated y-value.
 */
float linear_interpolate(float x, float x1, float x2, float y1, float y2);

#endif /* C_UTILS */