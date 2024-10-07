#ifndef MAIN_DEFINITION
#define MAIN_DEFINITION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Slice  Slice;

struct Slice
{
    char *buffer;
    int len;
    int cap;
};



Slice *initSlice(int cap);
void freeSlice(Slice *slice);

int nextLencap (int newLen, int oldCap);

void copyOldSlice(Slice *oldSlice, char *value, size_t valueLen);
void copyNewSlice(Slice *oldSlice, char *value, size_t valueLen, Slice *newSlice);

Slice *appendSlice(Slice *currentSlice, char *value);

#endif