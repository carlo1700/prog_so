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


// per ignorare i warning di compilazione 
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

typedef enum seek
{
    F_SEEK_SET = 0x1,
    F_SEEK_CUR = 0x2,
    F_SEEK_END = 0x3,
} FSeek;

typedef enum mode
{
    R = 0x1,
    W = 0x2,
    RW = 0x3,
} ModeType;

typedef struct bitmap
{
    unsigned char data[DATA_BLOCKS / 8];
    unsigned int size;
} BitMap;

typedef struct fcb
{
    ListItem list;
    int fileSize;
    int blockNum;
    int fatIndex;
    char filename[MAX_FILE_NAME_LENGTH];
    char data[BLOCK_SIZE - 44];
} FCB;

typedef struct FAT
{
    int tableFAT[DATA_BLOCKS];
    BitMap bitmap;
} FAT;

typedef struct dir
{
    ListItem list;
    ListHead fcbs;
    ListHead subDirs;
    char dirname[MAX_FILE_NAME_LENGTH];
    struct dir *parentDir;
} DIR;

typedef struct disk
{
    FAT fat;
    DIR *root;
    DIR *currentDir;
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


// Utils
char *getBlock(Disk *disk);
char *getBlockFromIndex(Disk *disk, int idx);
int getIndexFromBlock(Disk *disk, char *block);
int eraseBlock(Disk *disk, char *block);
int getFatIndex(Disk *disk, int idx);
int resetFatIndex(Disk *disk, int idx);
int updateFatIndex(Disk *disk, int idx, int newIdx);

// File Utils
FileHandle *createFile(Disk *disk, const char *path, ModeType mode);
FCB *searchFile(Disk *disk, const char *path);
int eraseFile(Disk *disk, const char *path);
int f_close(FileHandle *fileHandle);
int f_write(Disk *disk, FileHandle *f, char *buffer, int size);
int f_read(Disk *disk, FileHandle *f, char *buffer, int size);
int f_seek(Disk *disk, FileHandle *f, unsigned int offset, FSeek seek);

// Directory Utils
void dir_init(Disk *disk, DIR *dir, const char *dirname);
DIR *searchDir(DIR *dir, const char *dirname);
int dirAddFile(Disk *disk, FCB *fcb);
int dirRemoveFIle(Disk *disk, FCB *fcb);
void createDir(Disk *disk, const char *path);
int eraseDir(Disk *disk, const char *path);
void listDir(Disk *disk, DIR *dir);
int changeDir(Disk *disk, DIR *dir, const char *dirname);

#endif // __FILESYSTEMFAT_H__