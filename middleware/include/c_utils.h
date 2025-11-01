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

#endif /* C_UTILS */