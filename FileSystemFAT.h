#ifndef FILESYSTEMFAT_H
#define FILESYSTEMFAT_H

#include "FS_Structs.h"
#include "utils.h"

FileHandle *createFile(FileSystemFAT *fs, char *path, char *name, mode_type mode);
void eraseFile(FileSystemFAT *fs, char *path, char *name);
int close(FileHandle *fh);

int fs_seek(FileHandle *fd, unsigned int offset, int whence);
int fs_read(FileHandle *fd, void *buf, int count);
int fs_write(FileHandle *fd, const void *buf, int count);

void createDirectory(FileSystemFAT *fs, char *path);
void eraseDirectory(FileSystemFAT *fs, char *path);
void listDirectory(FileSystemFAT *fs, char *path);
void changeDirectory(FileSystemFAT *fs, char *name, char *oldPath, char *newPath);


void printOpenFileInfo();

void initOpenFileInfo();

#endif
