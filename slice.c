#include "slice.h"



Slice *initSlice(int cap) {
    Slice *newString = malloc(sizeof(Slice));
    if (newString == NULL) {
        return NULL;
    }

    newString->buffer = malloc(cap * sizeof(char));
    if (newString->buffer == NULL) {
        free(newString);
        return NULL;
    }

    newString->cap = cap;
    newString->len = 0;

    return newString;
}

void freeSlice(Slice *slice) {
    if (slice != NULL) {
        free(slice->buffer);
        free(slice);
    }
}


int nextLencap (int newLen, int oldCap) {
    int newCap = oldCap;
    int doubledCap = newCap + newCap;

    if (newLen > doubledCap) {
        return newLen;
    }

    const int treshold = 256;
    if (oldCap < treshold) {
        return doubledCap;
    }
    for (;;) {
        newCap += (newCap + 3*treshold) >> 2;

        if (newCap >= newLen) {
            break;
        }
    }

    if (newCap <= 0) {
        return newLen;
    }
    
    return newCap;
}


void copyOldSlice(Slice *oldSlice, char *value, size_t valueLen) {
    for (size_t i = 0; i < valueLen; i++) {
        oldSlice->buffer[oldSlice->len+i] = value[i];
    }

    oldSlice->len += valueLen;
    oldSlice->buffer[oldSlice->len] = '\0';
}


void copyNewSlice(Slice *oldSlice, char *value, size_t valueLen, Slice *newSlice) {
    for (size_t i = 0; i < oldSlice->len; i++) {
        newSlice->buffer[i] = oldSlice->buffer[i];
    }

    for (size_t i = 0; i < valueLen; i++) {
        newSlice->buffer[oldSlice->len + i] = value[i];
    }

    newSlice->len = oldSlice->len + valueLen;
    newSlice->buffer[newSlice->len] = '\0';  
}


Slice *appendSlice(Slice *currentSlice, char *value) {
    size_t sizeValue = strlen(value);
    int newLen = currentSlice->len + sizeValue;

    if (newLen > currentSlice->cap) {
        int newCap = nextLencap(newLen, currentSlice->cap);

        Slice *newSlice = initSlice(newCap);
        if (newSlice == NULL) {
            return NULL;
        }

        copyNewSlice(currentSlice, value, sizeValue, newSlice);

        free(currentSlice->buffer);
        free(currentSlice);

        return newSlice;

    } else {
        copyOldSlice(currentSlice, value, sizeValue);
        return currentSlice;    
    }
}
