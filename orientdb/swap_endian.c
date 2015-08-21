#include <stdio.h>
#include "swap_endian.h"

void swapEndian(void *num, int type) {
    switch (type) {
        case SHORT: ;
            short *_short = num;
            *_short = (*_short<<8 & 0xff00) |(*_short>>8 & 0x00ff);
            break;
        case INT: ;
            int *_int = num;
            *_int = (*_int<<8 & 0xff00ff00) | (*_int>>8 & 0x00ff00ff);
            *_int = (*_int<<16 & 0xffff0000) | (*_int>>16 & 0x0000ffff);
            break;
        case LONG: ;
            long *_long = num;
            *_long = (*_long<<8 & 0xff00ff00ff00ff00) | (*_long>>8 & 0x00ff00ff00ff00ff);
            *_long = (*_long<<16 & 0xffff0000ffff0000) | (*_long>>16 & 0x0000ffff0000ffff);
            *_long = (*_long<<32 & 0xffffffff00000000) | (*_long>>32 & 0x00000000ffffffff);
            break;
        default:
            fprintf(stderr, "Error: swapEndian wrong type\n");
            break;
    }
}

