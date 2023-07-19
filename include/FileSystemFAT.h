#pragma once

#ifndef __FILESYSTEMFAT_H__
#define __FILESYSTEMFAT_H__

#define DATA_BLOCKS 1024
#define BLOCK_SIZE 4096 // bytes

#define DISK_DATA_SIZE 1024 * 4096
#define DISK_SIZE 4096 * 2 + 1024 * 4096            // META_DATA_SIZE + DISK_DATA_SIZE
#define DISK_BLOCKS (4096 * 2 + 1024 * 4096) / 4096 // DISK_SIZE / BLOCK_SIZE

#define MAX_OPEN_FILES 20
#define MAX_FCBS 1200
#define MAX_FILE_NAME_LENGTH 12
#define MAX_LEVEL DISK_SIZE / BLOCK_SIZE

#include "linked_list.h"
#include "bitmap.h"
#include <assert.h>

typedef enum mode
{
    R = 0x1,
    W = 0x2,
    RW = 0x3,
    A = 0x4,
} ModeType;

typedef struct bitmap
{
    unsigned char data[DATA_BLOCKS / 8];
    unsigned int size;
} BitMap;

typedef struct fcb
{
    char filename[MAX_FILE_NAME_LENGTH];
    int fileSize;
    int blockNum;
    int fatIndex;
    ListItem list;
    char data[DISK_BLOCKS - 44];
} FCB;

typedef struct dir
{
    char dirname[MAX_FILE_NAME_LENGTH];
    int level;
    ListHead fcbs;
    ListItem subDir;
} DIR;

typedef struct FAT
{
    int tableFAT[DATA_BLOCKS];
    BitMap bitmap;
} FAT;

typedef struct disk
{
    FAT fat;
    ListHead root;
    int diskSize;
    int blockSize;
    int blockNum;
    int freeBlockNum;
    char diskBuffer[DISK_DATA_SIZE];
} Disk;

typedef struct fileHandler
{
    FCB *fcb;
    int offset;
    char *pos;
    ModeType permission;
} FileHandle;

#endif // __FILESYSTEMFAT_H__