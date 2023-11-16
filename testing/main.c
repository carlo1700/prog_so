#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
	memset(disk->fat.tableFAT, -1, DATA_BLOCKS * sizeof(int));
	memset(disk->diskBuffer, 0, DISK_DATA_SIZE);
	bmap_init(&disk->fat.bitmap, DATA_BLOCKS / 8);
	createDir(disk, "ROOT");
	disk->currentDir = disk->root;
	return disk;
}


void testDir(Disk *disk)
{
	createDir(disk, "/test");
	createDir(disk, "/test2");
	createDir(disk, "/test3");
	createDir(disk, "ROOT/test4");
	createDir(disk, "ROOT/test4/test5");
	createDir(disk, "ROOT/test4/test6");
	createDir(disk, "ROOT/test4/test7");
	createDir(disk, "ROOT/test8/test9");
	// createDir(disk, "ROOT/test6/test.txt/test8");
	listDir(disk, disk->root);
	eraseDir(disk, "ROOT/test4");
	listDir(disk, disk->root);
	// changeDir(disk, disk->root, "test");
}

void testFile(Disk *disk)
{
	FileHandle *file = f_open(disk, "ROOT/test2/test.txt", RW);
	file = createFile(disk, "ROOT/test2/test.txt", RW);
	listDir(disk, disk->root);
	eraseFile(disk, "ROOT/test2/test.txt");
	f_close(file);
	FileHandle *file2 = createFile(disk, "ROOT/test2/test.txt", W);
	char *s = malloc(5000);
	for (size_t i = 0; i < 5000; i++)
	{
		s[i] = 'a';
	}
	if (f_write(disk, file2, s, strlen(s)) == -1)
		perror("error write\n");
	f_close(file2);
	file2 = createFile(disk, "ROOT/test2/test.txt", R);
	memset(s, 0, 5000);
	if (f_read(disk, file2, s, 5000) == -1)
		perror("error read\n");
	printf("%s\n", s);
	int ret = f_seek(disk, file2, 0, F_SEEK_SET);
	ret = f_seek(disk, file2, 4800, F_SEEK_CUR);
	ret = f_seek(disk, file2, 0, F_SEEK_END);
	free(s);
	f_close(file2);
	eraseFile(disk, "ROOT/test2/test.txt");
}

int main()
{
    Disk *disk = FS_init();
	if(!disk)
	{
		perror("init failed");
		return -1;
	}

	char *block = getBlock(disk);
	block = getBlock(disk);
	block = getBlock(disk);
	block = getBlock(disk);
	block = getBlock(disk);
	// bmap_print(&disk->fat.bitmap);

	testDir(disk);

	testFile(disk);

	//dealloca la mmap
	munmap(disk, DISK_SIZE);

	return 0;
}
