#include "../include/linked_list.h"
#include "../include/FileSystemFAT.h"
#include <stdio.h>
#include <string.h>

char *getBlock(Disk *disk)
{
    int idx;

    idx = bmap_getFirstIndex(&disk->fat.bitmap);
    if (idx == -1)
    {
        perror("getBlock index failed");
        return NULL;
    }

    bmap_set(&disk->fat.bitmap, idx);

    return disk->diskBuffer + (idx * disk->blockSize);    
}