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