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
 * @brief Restituisce l'indice del primo bit a 0 nella bitmap.
 * 
 * @param bitmap 
 * @return int 
 */
int bmap_getFirstIndex(BitMap *bitmap)
{
    assert(bitmap);
    for (int i = 0; i < (int)bitmap->size * 8; ++i)
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
    for (int i = 0; i < (int)bitmap->size * 8; ++i)
    {
        printf("%d ", bmap_getBit(bitmap, i));
    }
    printf("\n\n");
}
