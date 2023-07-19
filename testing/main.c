#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "../include/bitmap.h"
#include "../include/linked_list.h"
#include "../include/FileSystemFAT.h"

Disk *FS_init(void)
{
	Disk	*disk;
	char	*fd;

	fd = (char *)mmap(NULL, DISK_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (fd == MAP_FAILED)
	{
		perror("map failed");
		return NULL;
	}
	disk = (Disk *) fd;

	disk->blockSize = BLOCK_SIZE;
	disk->blockNum = DATA_BLOCKS;
	disk->freeBlockNum = DATA_BLOCKS;
	disk->diskSize = DISK_SIZE;
	memset(disk->fat.tableFAT, -1, DATA_BLOCKS);
	bmap_init(&disk->fat.bitmap, DATA_BLOCKS / 8);
	List_init(&disk->root);

	return disk;
}


int main()
{
    Disk *disk = FS_init();
}