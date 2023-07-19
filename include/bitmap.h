#pragma once

#ifndef __BITMAP_H__
#define __BITMAP_H__

#include "FileSystemFAT.h"

typedef struct bitmap BitMap;

void bmap_init(BitMap *bitmap, int size);
void bmap_set(BitMap *bitmap, unsigned int index);
void bmap_clear(BitMap *bitmap, unsigned int index);
void bmap_toggle(BitMap *bitmap, unsigned int index);
int bmap_getFirstIndex(BitMap *bitmap);
int bmap_getBit(BitMap *bitmap, unsigned int index);
int bmap_isClear(BitMap *bitmap);
void bmap_print(BitMap *bitmap);

#endif // __BITMAP_H__