#ifndef PRINTS_H
#define PRINTS_H

#include "FS_Structs.h"

void printFCB(FileSystemFAT *fs, FCB *fcb);

void printFS(FileSystemFAT *fs, const char* option);

void printFileContent(FileSystemFAT *fs, FCB *fileFCB);

void printDirectoryTree(FileSystemFAT *fs, FCB *dirFCB, int deep);

#endif
