#include "../include/bitmap.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Inizializza una nuova bitmap con una dimensione specificata e la restituisce.
 *
 * @param size
 * @return BitMap*
 *
 */
void bmap_init(BitMap *bitmap, int size)
{
    assert(size);
    assert(bitmap);
    bitmap->size = size;
    memset(bitmap->data, 0, size);
}

/**
 * @brief Imposta il bit alla posizione specificata nella bitmap.
 *
 * @param bitmap
 * @param index
 *
 */
void bmap_set(BitMap *bitmap, unsigned int index)
{
    assert(bitmap);
    if (index > bitmap->size)
        return;
    bitmap->data[index / 8] |= (1 << (index % 8));
}

/**
 * @brief Reimposta a 0 il bit alla posizione specificata nella bitmap.
 *
 * @param bitmap
 *
 */
void bmap_clear(BitMap *bitmap, unsigned int index)
{
    assert(bitmap);
    if (index > bitmap->size)
        return;
    bitmap->data[index / 8] &= ~(1 << (index % 8));
}

/**
 * @brief Inverte il valore del bit alla posizione specificata nella bitmap.
 *
 * @param bitmap
 *
 */
void bmap_toggle(BitMap *bitmap, unsigned int index)
{
    assert(bitmap);
    if (index > bitmap->size)
        return;
    bitmap->data[index / 8] ^= (1 << (index % 8));
}

/**
 * @brief Restituisce il valore del bit alla posizione specificata nella bitmap.
 *
 * @param bitmap
 *
 */
int bmap_getBit(BitMap *bitmap, unsigned int index)
{
    assert(bitmap);
    if (index > bitmap->size)
        return 0;
    return (bitmap->data[index / 8] & (1 << (index % 8))) != 0;
}

/**
 * @brief Restituisce 1 se tutti i bit della bitmap sono a 0, 0 altrimenti.
 *
 * @param bitmap
 * @return int
 *
 */
int bmap_isClear(BitMap *bitmap)
{
    assert(bitmap);
    unsigned long *data = (unsigned long *)bitmap->data;
    size_t realsize = (bitmap->size + 7) / 8;
    size_t numLongs = realsize / sizeof(unsigned long);

    for (size_t i = 0; i < numLongs; ++i)
    {
        if (data[i] != 0)
            return 0;
    }
    size_t remainingBytes = realsize % sizeof(unsigned long);
    unsigned char *remainingData = (unsigned char *)(data + numLongs);

    for (size_t i = 0; i < remainingBytes; ++i)
    {
        if (remainingData[i] != 0)
            return 0;
    }

    return 1;
}

/**
 * @brief Restituisce l'indice del primo bit a 0 nella bitmap.
 * 
 * @param bitmap 
 * @return int 
 */
int bmap_getFirstIndex(BitMap *bitmap)
{
    assert(bitmap);
    for (int i = 0; i < (int)bitmap->size; ++i)
    {
        if (bmap_getBit(bitmap, i) == 0)
            return i;
    }
    return -1;
}

/**
 * @brief Stampa a video i valori dei bit della bitmap.
 *
 * @param bitmap
 *
 */
void bmap_print(BitMap *bitmap)
{
    if (!bitmap)
        return;
    for (int i = 0; i < (int)bitmap->size; ++i)
    {
        printf("%d ", bmap_getBit(bitmap, i));
    }
    printf("\n");
}
