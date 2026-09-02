#include "c_utils.h"
#include <assert.h>
#include <math.h>

/* Epsilon for float comparisons */
#define FLOAT_EPSILON (0.0001f)

void endian_swap(void *ptr, int size)
{
	char *p = (char *)ptr;
	char tmp;
	for (int i = 0; i < size / 2; i++) {
		tmp = p[i];
		p[i] = p[size - i - 1];
		p[size - i - 1] = tmp;
	}
}

unsigned char reverse_bits(unsigned char b)
{
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

float linear_interpolate(float x, float x1, float x2, float y1, float y2)
{
	assert(fabsf(x2 - x1) > FLOAT_EPSILON);

	return y1 + ((x - x1) * (y2 - y1) / (x2 - x1));
}

bool update_active_high_hysteresis(float value, float high_threshold,
				   float low_threshold, bool *active)
{
	assert(active != NULL);
	assert(high_threshold > low_threshold);

	if (*active) {
		*active = value > low_threshold;
	} else {
		*active = value > high_threshold;
	}

	return *active;
}

bool update_active_low_hysteresis(float value, float high_threshold,
				  float low_threshold, bool *active)
{
	assert(active != NULL);
	assert(high_threshold > low_threshold);

	if (*active) {
		*active = value < high_threshold;
	} else {
		*active = value < low_threshold;
	}

	return *active;
}

uint16_t uint8_to_uint16(uint8_t msb, uint8_t lsb)
{
	return ((uint16_t)msb << 8) | ((uint16_t)lsb);
}

void uint16_to_uint8(uint16_t value, uint8_t arr[2])
{
	arr[1] = (uint8_t)value;
	arr[0] = (uint8_t)(value >> 8);
}
