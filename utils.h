#ifndef UTILS_H
#define UTILS_H

#include "FS_Structs.h"

char *getDataBlock(FileSystemFAT *fs);


int getBlockIdx(FileSystemFAT *fs, char *block_ptr);

char *getBlockPointer(FileSystemFAT *fs, int idx);


void removeBit(char *bitMap, int index);

void setBit(char *bitMap, int index);

void removeFatIndex(FileSystemFAT *fs, FCB *dirFCB);


FCB *createFCB(FileSystemFAT *fs, FCB *fcbDir, char *fileName, int32_t isDirectory);

int deleteFCB(FileSystemFAT *fs, FCB *fcb);

FCB *findFCB(FileSystemFAT *fs, FCB *dirFcb, char *name);

FCB *createDirectoryWrapped(FileSystemFAT *fs, char *path);

void addToDir(FileSystemFAT *fs, FCB *fcb, FCB *dirFCB);

void removeFromDir(FileSystemFAT *fs, FCB *dirFCB, FCB *fcb);



DirectoryEntry *getDirBlock(FileSystemFAT *fs, FCB *dirFcb, int deep);

DirectoryEntry *createDirBlock(FileSystemFAT *fs, FCB *dirFcb, int deep);

FileEntry *getFileDataBlock(FileSystemFAT *fs, FCB *fileFcb, int deep);

FileEntry *createFileDataBlock(FileSystemFAT *fs, FCB *fileFcb, int deep);



openFileInfo *newOpenFileInfo(openFileInfo **ofiTable, int *openedFiles);

openFileInfo *findOpenFileInfo(openFileInfo **ofiTable, FCB *toFind);

int remOpenFileInfo(openFileInfo **ofiTable, int *openedFiles, openFileInfo *elem);

FileHandle *newFileHandle(openFileInfo *ofi, mode_type mode);




int pathIsValid(char *path);

int nameIsValid(char *name);

#endif
