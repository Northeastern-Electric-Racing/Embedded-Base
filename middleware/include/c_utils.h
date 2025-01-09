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


void endian_swap(void *ptr, int size);

/// @brief Reverses the bit order of a byte
/// @param b byte
/// @return the same byte but wuth the bits reversed
unsigned char reverse_bits(unsigned char b);

#endif /* C_UTILS */