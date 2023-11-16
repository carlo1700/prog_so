#include "../include/linked_list.h"
#include "../include/FileSystemFAT.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int filenameIsValid(const char *name)
{
    char *path_copy;
	char *filename;

    if (name == NULL || strlen(name) == 0 || (name[0] != '/' && !strcmp(name, "ROOT")))
	{
		perror("path for file is not valid");
		return 0;
	}

    path_copy = strchr(name, '.');
    if (path_copy != NULL && strchr(path_copy, '/'))
    {
        perror("name is not valid");
        return 0;
    }

	return 1;

}

FCB *searchFile(Disk *disk, const char *path)
{
    FCB *fcb;
    ListItem *item;
    char *path_copy;
    char *token;
    char *filename;

    assert(path);

    if (!filenameIsValid(path))
        return 0;

    if (!strncmp(path, "ROOT", 4))
        disk->currentDir = disk->root;

    path_copy = strdup(path);
    token = strtok(path_copy, "/");
    while (token != NULL)
    {
        if ((filename = strchr(token, '.')))
            break;
        if (!(disk->currentDir = searchDir(disk->currentDir, token)))
        {
            perror("Directory not found");
            return NULL;
        }
        token = strtok(NULL, "/");
    }

    filename = strstr(path, token);
    free(path_copy);

    item = disk->currentDir->fcbs.first;
    while (item)
    {
        fcb = (FCB *)item;
        if (!strcmp(fcb->filename, filename))
            return fcb;
        item = item->next;
    }

    return 0;
}

char *getFilename(const char *name)
{
    DIR *dir;
    char *path_copy;
    char *filename;

    if (!filenameIsValid(name))
        return NULL;

    path_copy = strdup(name);
    filename = strtok(path_copy, "/");
    while (filename != NULL)
    {
        if ((strchr(filename, '.')))
            break;
        filename = strtok(NULL, "/");
    }

    filename = strstr(name, filename);
    free(path_copy);
    return filename;
}

void fcbInit(FCB *fcb, const char *filename)
{
    memcpy(fcb->filename, filename, MAX_FILE_NAME_LENGTH);
    fcb->fileSize = 0;
    fcb->blockNum = 0;
    fcb->fatIndex = 0;
    fcb->list.next = NULL;
    fcb->list.prev = NULL;
    memset(fcb->data, 0, sizeof(fcb->data));
}

FileHandle *createFileHandle(FCB *fcb, ModeType mode)
{
    FileHandle *fileHandle;

    fileHandle = (FileHandle *)malloc(sizeof(FileHandle));
    assert(fileHandle);

    fileHandle->fcb = fcb;
    fileHandle->offset = 0;
    fileHandle->pos = fcb->data;
    fileHandle->permission = mode;

    return fileHandle;
}

FileHandle *createFile(Disk *disk, const char *path, ModeType mode)
{
    FCB *fcb;
    char *filename;
    filename = getFilename(path);
    if ((fcb = searchFile(disk, path)))
        return createFileHandle(fcb, mode);
    fcb = (FCB *)getBlock(disk);
    if (!fcb)
    {
        perror("Not enough space");
        return NULL;
    }
    fcbInit(fcb, filename);
    if (!dirAddFile(disk, fcb))
    {
        perror("File could not be added to directory");
        return NULL;
    }
    return createFileHandle(fcb, mode);
}

int eraseFile(Disk *disk, const char *path)
{
    FCB *fcb;
    char *block;
    char *filename;
    int idx;
    filename = getFilename(path);
    if (!filename)
        return 0;
    if (!(fcb = searchFile(disk, filename)))
    {
        perror("File not found");
        return 0;
    }
    while ((idx = getFatIndex(disk, fcb->fatIndex))!= -1)
    {
        block = getBlockFromIndex(disk, idx);
        eraseBlock(disk, block);
        fcb->fatIndex = resetFatIndex(disk, fcb->fatIndex);
    }
    dirRemoveFIle(disk, fcb);
    eraseBlock(disk, (char *)fcb);
    return 1;
}

FileHandle *f_open(Disk *disk, const char *path, ModeType mode)
{
    FCB *fcb;

    char *filename = getFilename(path);

    if ((fcb = searchFile(disk, path)))
        return createFileHandle(fcb, mode);
    perror("File not found");
    return 0;
}

int f_close(FileHandle *fileHandle)
{
    free(fileHandle);
    return 1;
}

int f_write(Disk *disk, FileHandle *f, char *buffer, int size)
{
    assert(disk);
    assert(f);
    assert(buffer);
    assert(f->permission == W || f->permission == RW);

    f->offset = 0;
    int pcbIndex = 0;
    int bytesWritten = 0;
    int bytesToWrite = size;
    int bytesLeft = sizeof(f->fcb->data);
    int bytesToCopy = bytesToWrite > bytesLeft ? bytesLeft : bytesToWrite;

    while (bytesToWrite > 0)
    {
        memcpy(f->pos, buffer, bytesToCopy);
        f->pos += bytesToCopy;
        f->offset += bytesToCopy;
        bytesWritten += bytesToCopy;
        bytesToWrite -= bytesToCopy;
        bytesLeft -= bytesToCopy;
        if (bytesLeft == 0)
        {
            pcbIndex = getIndexFromBlock(disk, (char *)f->fcb);
            f->fcb->fatIndex = pcbIndex;
            char *block = getBlock(disk);
            if (!block)
            {
                perror("Not enough space");
                return 0;
            }
            if (updateFatIndex(disk, f->fcb->fatIndex, getIndexFromBlock(disk, block)) == -1)
            {
                perror("Could not update FAT");
                return -1;
            }
            f->pos = block;
            ++f->fcb->blockNum;
            bytesLeft = BLOCK_SIZE - ((f->offset + (BLOCK_SIZE - sizeof(f->fcb->data))) % BLOCK_SIZE);
            bytesToCopy = bytesToWrite > bytesLeft ? bytesLeft : bytesToWrite;
        }
        f->fcb->fileSize = bytesWritten;
    }
    return bytesWritten;
}

int f_read(Disk *disk, FileHandle *f, char *buffer, int size)
{
    assert(disk);
    assert(f);
    assert(buffer);
    assert(f->permission == R || f->permission == RW);

    int i = -1;
    int bytesRead = 0;
    int bytesLeft;
    int bytesToRead;

    while (++i <= f->fcb->blockNum)
    {
        if (f->offset < (int) sizeof(f->fcb->data))
            bytesLeft = sizeof(f->fcb->data) - f->offset;
        else
            bytesLeft = (f->fcb->fileSize - f->offset > BLOCK_SIZE) ? BLOCK_SIZE : f->fcb->fileSize - f->offset;
        bytesToRead = size > bytesLeft ? bytesLeft : size;

        memcpy((buffer + bytesRead), f->pos, bytesToRead);
        f->pos += bytesToRead;
        f->offset += bytesToRead;
        bytesRead += bytesToRead;
        size -= bytesToRead;
        bytesLeft -= bytesToRead;
        if (bytesLeft == 0)
            f->pos = getBlockFromIndex(disk, getFatIndex(disk, f->fcb->fatIndex));
    }
    return bytesRead;
}

int f_seek(Disk *disk, FileHandle *f, unsigned int offset, FSeek seek)
{
    assert(f);
    assert(seek == F_SEEK_SET || seek == F_SEEK_CUR || seek == F_SEEK_END);

    int temp = -1;
    int idx = -1;
    int i = 0;
    switch (seek)
    {
    case F_SEEK_SET:
        f->offset = 0;
        f->pos = f->fcb->data;
        return f->offset;
    case F_SEEK_CUR:
        if ((int) (f->offset + offset) > f->fcb->fileSize)
        {
            perror("Invalid offset");
            return -1;
        }
        f->offset += offset;
        int x = (f->offset / BLOCK_SIZE);
        while (i++ <= (f->offset / BLOCK_SIZE) &&
            (temp = getFatIndex(disk, f->fcb->fatIndex)) != -1)
        {
            idx = temp;
        }
        f->pos = getBlockFromIndex(disk, idx);
        return f->offset;
    case F_SEEK_END:
        f->offset = f->fcb->fileSize;
        while (i++ < (f->offset / BLOCK_SIZE) &&
            (temp = getFatIndex(disk, f->fcb->fatIndex)) != -1)
        {
            idx = temp;
        }
        f->pos = getBlockFromIndex(disk, idx);
        return f->offset;
    default:
        break;
    }
    return 1;
}
