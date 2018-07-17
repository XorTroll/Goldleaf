#include "debug.h"

#include <stdio.h>

void printBytes(u8 *bytes, size_t size)
{
    int count = 0;

    printf("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    printf("-----------------------------------------------\n");

    for (int i = 0; i < size; i++)
    {
        printf("%02x ", bytes[i]);
        count++;
        if ((count % 16) == 0)
            printf("\n");
    }

    printf("\n");
}