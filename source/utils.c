#include "../include/linked_list.h"
#include "../include/FileSystemFAT.h"
#include <stdio.h>
#include <string.h>

int getIndexFromBlock(Disk *disk, char *block)
{
    int idx;

    idx = (block - disk->diskBuffer) / disk->blockSize;
    if (idx == -1 || idx > disk->blockNum)
    {
        perror("getIndexFromBlock index failed");
        return -1;
    }
    return idx;
}

char *getBlockFromIndex(Disk *disk, int idx)
{
    char    *block;

    if (idx == -1 || idx > disk->blockNum)
    {
        perror("getBlock index failed");
        return NULL;
    }

    if (!bmap_getBit(&disk->fat.bitmap, idx))
        return NULL;

    block = disk->diskBuffer + (idx * disk->blockSize);
    return block;    
}

char *getBlock(Disk *disk)
{
    int     idx;
    char    *block;

    idx = bmap_getFirstIndex(&disk->fat.bitmap);
    if (idx == -1)
    {
        perror("getBlock index failed! no free block");
        return NULL;
    }

    bmap_set(&disk->fat.bitmap, idx);

    block = getBlockFromIndex(disk, idx);
    memset(block, 0, disk->blockSize);

    return block;    
}

int eraseBlock(Disk *disk, char *block)
{
    int idx;

    idx = getIndexFromBlock(disk, block);
    if (!bmap_getBit(&disk->fat.bitmap, idx))
    {
        perror("eraseBlock index failed! block was not allocated");
        return -1;
    }
    memset(block, 0, disk->blockSize);
    bmap_clear(&disk->fat.bitmap, idx);
    disk->freeBlockNum++;
    return 0;
}

int getFatIndex(Disk *disk, int idx)
{
    if (idx == -1 || idx > disk->blockNum)
    {
        perror("getFatIndex index failed");
        return -1;
    }
    return disk->fat.tableFAT[idx];
}

int resetFatIndex(Disk *disk, int idx)
{
    int newIdx = -1;
    if (idx == -1 || idx > disk->blockNum)
    {
        perror("resetFatIndex index failed");
        return -1;
    }
    newIdx = disk->fat.tableFAT[idx];
    disk->fat.tableFAT[idx] = -1;
    return newIdx;
}

int updateFatIndex(Disk *disk, int idx, int newIdx)
{
    int temp;
    if (idx == -1 || idx > disk->blockNum)
    {
        perror("updateFatIndex index failed");
        return -1;
    }
    while ((temp = getFatIndex(disk, idx)) != -1)
    {
        idx = temp;
        if (idx == newIdx)
        {
            perror("updateFatIndex index failed! loop detected");
            return -1;
        }
    }
    disk->fat.tableFAT[idx] = newIdx;
    return newIdx;
}