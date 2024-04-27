#ifndef C_UTILS
#define C_UTILS

/*
 * Will retrieve the container of a certain pointer given the container type and its pointer
 * Trick pulled from Linux Kernel
 */
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#endif /* C_UTILS */


void endian_swap(void *ptr, int size);
