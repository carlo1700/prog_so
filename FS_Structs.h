#ifndef FS_STRUCTS_H
#define FS_STRUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>


#define DATA_BLOCKS 1024
#define BLOCK_SIZE 4096 // bytes

#define META_DATA_SIZE 4096 * 2  // 4 + 4 + 4 + 4 + 8 + 128 + 8000 + 40 = 8192 bytes
#define META_DATA_BLOCKS 2

#define DISK_DATA_SIZE 1024 * 4096
#define DISK_SIZE 4096 * 2 + 1024 * 4096 // META_DATA_SIZE + DISK_DATA_SIZE
#define DISK_BLOCKS (4096 * 2 + 1024 * 4096) / 4096 // DISK_SIZE / BLOCK_SIZE

#define MAX_OPEN_FILES 20
#define MAX_FCBS 1200
#define MAX_FILE_NAME_LENGTH 12

#define MAX_DIR_IN_MIN ( BLOCK_SIZE - 40 ) / 8
#define MAX_DIR_IN_BLOCK ( BLOCK_SIZE - 8 ) / 8

#define ROOT_DIR_NAME "$ROOT$"

#define RED     "\x1b[31m"
#define BLUE     "\x1b[34m"
#define GREEN    "\x1b[32m"
#define CYAN     "\x1b[36m"
#define RESET   "\x1b[0m"
#define MAGENTA  "\x1b[35m"
#define YELLOW   "\x1b[33m"
#define WHITE    "\x1b[37m"
#define BLACK    "\x1b[30m"
#define PURPLE   "\x1b[35m"
#define BROWN    "\x1b[33m"
#define GRAY     "\x1b[30m"
#define BOLDBLACK    "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"
#define BOLDPURPLE  "\033[1m\033[35m"


#define EX_W_R 7
#define EX_W 6
#define EX_R 5
#define EX 4
#define W_R 3
#define W 2
#define R 1

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define MAX_READ_WRITE_SIZE 4096 * 3

typedef int32_t mode_type;

typedef struct FCB{
	char	fileName[MAX_FILE_NAME_LENGTH];
	int32_t	isDirectory; // (0 = false, 1 = true)
	int32_t	BlockCount;
	int32_t	FATNextIndex;
	int64_t	fileSize; // not used for directories
	char	data[BLOCK_SIZE - 32]; // 4064 bytes
} FCB;


typedef struct FileSystemFAT {
	int32_t	tableFAT[DATA_BLOCKS]; // 1024 * 4 bytes = 4096 bytes
	int32_t	diskSize;
	int32_t	blockSize;
	int32_t	blockNum;
	int32_t	numFCBS;
	FCB		*rootFCB; // 8 bytes
	char	bitMap[DATA_BLOCKS / 8];
	FCB		*fcbList[MAX_FCBS]; // 1000 * 8 bytes = 8000 bytes
	char	info_padding[40]; // 4096 * 2 - (4 + 4 + 4 + 4 + 8 + 128 + 8000) = 40 bytes (padding)
	char	diskBuffer[DISK_DATA_SIZE];
} FileSystemFAT;


typedef struct DirectoryEntry {
	int32_t	numFCBS;
	int32_t	isLast;
	FCB		*FCBS[MAX_DIR_IN_BLOCK]; // 511 pointers * 8 bytes each = 4088 bytes
} DirectoryEntry;


typedef struct FileEntry {
	char	data[BLOCK_SIZE];
} FileEntry;


// 4064 bytes
typedef struct DirectoryEntryMin {
	int32_t	numFCBS;
	int32_t	isLast;
	FCB		*FCBS[MAX_DIR_IN_MIN]; // 507 pointers * 8 bytes each = 4056 bytes
} DirectoryEntryMin;


// 4064 bytes
typedef struct FileEntryMin {
	char	data[BLOCK_SIZE - 32];
} FileEntryMin;



typedef struct openFileInfo {
	FileSystemFAT	*fileSystem;
	FCB				*fcb;
	int				numFileHandle;
	int				isUsed;
} openFileInfo;

typedef struct FileHandle {
	int				offset;
	mode_type		permissions;
	openFileInfo	*info;
} FileHandle;



#endif
