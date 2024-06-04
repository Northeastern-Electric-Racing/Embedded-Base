#include "c_utils.h"


void endian_swap(void *ptr, int size)
{
    char *p = (char *)ptr;
    char tmp;
    for (int i = 0; i < size/2; i++)
    {
        tmp = p[i];
        p[i] = p[size-i-1];
        p[size-i-1] = tmp;
    }
}

unsigned char reverse_bits(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}