#include "../include/linked_list.h"
#include "../include/FileSystemFAT.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dir_init(Disk *disk, DIR *dir, const char *dirname)
{
	memcpy(dir->dirname, dirname, MAX_FILE_NAME_LENGTH);
	List_init(&dir->fcbs);
	List_init(&dir->subDirs);
	dir->list.next = NULL;
	dir->list.prev = NULL;
	dir->parentDir = disk->currentDir;
	if (dir->parentDir)
		List_pushBack(&dir->parentDir->subDirs, (ListItem *)dir);
	memset(&dir->fcbs, 0, sizeof(dir->fcbs));
}

DIR *searchDir(DIR *dir, const char *dirname)
{
	DIR *subDir;
	ListItem *item;

	assert(dirname);

	if (!dir)
		return NULL;

	if (!strcmp(dir->dirname, dirname))
		return dir;

	item = dir->subDirs.first;
	while (item)
	{
		subDir = (DIR *)item;
		if (!strcmp(subDir->dirname, dirname))
			return subDir;
		item = item->next;
	}
	return NULL;
}

int pathIsValid(const char *path)
{ // !strcmp(path, "ROOT")
	if (path == NULL || strlen(path) == 0 || (path[0] != '/' && strncmp(path, "ROOT", 4)))
	{
		perror("path is not valid");
		return 0;
	}
	char *path_copy = strdup(path);
	char *dirname = strtok(path_copy, "/");
	while (dirname != NULL)
	{
		if (strchr(dirname, '.') != NULL)
		{
			free(path_copy);
			perror("path contains invalid file name");
			return 0;
		}
		dirname = strtok(NULL, "/");
	}
	free(path_copy);
	return 1;
}

int dirAddFile(Disk *disk, FCB *fcb)
{
	assert(disk);
	assert(fcb);

	if (searchFile(disk, fcb->filename))
	{
		perror("dirAddFile failed! file already exists");
		return 0;
	}
	List_pushFront(&disk->currentDir->fcbs, (ListItem *)fcb);
	return 1;
}

int dirRemoveFIle(Disk *disk, FCB *fcb)
{
	assert(disk);
	assert(fcb);

	if (!searchFile(disk, fcb->filename))
	{
		perror("dirRemoveFile failed! file not found");
		return 0;
	}
	List_detach(&disk->currentDir->fcbs, (ListItem *)fcb);
	return 1;
}

void createDir(Disk *disk, const char *path)
{
	DIR *exist;
	DIR *new_dir;
	char *path_copy;
	char *dirname;
	if (!pathIsValid(path))
		return;
	path_copy = strdup(path);
	dirname = strtok(path_copy, "/");
	while (dirname != NULL)
	{
		if (!(exist = searchDir(disk->currentDir, dirname)))
		{
			if (!(new_dir = (DIR *)getBlock(disk)))
			{
				free(path_copy);
				perror("createDir failed! getBlock failed");
				return;
			}
			dir_init(disk, new_dir, dirname);
			disk->currentDir = new_dir;
			if (!strcmp(dirname, "ROOT"))
				disk->root = new_dir;
		}
		else
			disk->currentDir = exist;
		dirname = strtok(NULL, "/");
	}
	disk->currentDir = disk->root;
	free(path_copy);
	return;
}

static int __eraseDir(Disk *disk, DIR *dir)
{
	FCB *fcb;
	DIR *subDir;

	assert(disk);

	if (!dir)
		return -1;

	while ((fcb = (FCB *)dir->fcbs.first))
	{
		// eraseFile(disk, fcb);
	}

	while ((subDir = (DIR *)dir->subDirs.first))
	{
		__eraseDir(disk, subDir);
	}

	List_detach(&dir->parentDir->subDirs, (ListItem *)dir);
	eraseBlock(disk, (char *)dir);
	return 0;
}

int eraseDir(Disk *disk, const char *path)
{
	DIR *dir;
	DIR *subDir;
	const char *dirname;
	char *path_copy;

	if (!pathIsValid(path))
		return 0;

	if (!strcmp(path, "ROOT"))
	{
		perror("eraseDir failed! cannot erase root");
		return 0;
	}

	path_copy = strdup(path);
	dirname = strtok(path_copy, "/");
	dir = disk->currentDir;
	while (dirname != NULL)
	{
		subDir = searchDir(dir, dirname);
		if (!subDir)
		{
			free(path_copy);
			perror("eraseDir failed! subdir not found");
			return 0;
		}
		dir = subDir;
		dirname = strtok(NULL, "/");
	}

	free(path_copy);
	return __eraseDir(disk, dir);
}

int changeDir(Disk *disk, DIR *dir, const char *dirname)
{
	DIR *subDir;
	ListItem *item;

	assert(disk);
	assert(dirname);

	if (!dir)
		return 0;

	item = dir->subDirs.first;
	while (item)
	{
		subDir = (DIR *)item;
		if (strcmp(subDir->dirname, dirname) == 0)
		{
			disk->currentDir = subDir;
			return 1;
		}
		item = item->next;
	}
	return 0;
}

void listDir(Disk *disk, DIR *dir)
{
	FCB *fcb;
	DIR *subDir;
	ListItem *item;
	static int i = 0;

	assert(disk);

	if (!dir)
		return;

	printf("%s---v\n", dir->dirname);

	++i;
	item = dir->fcbs.first;
	while (item)
	{
		fcb = (FCB *)item;
		for (int j = 0; j < i; ++j)
			printf("\t ");
		printf("%s\n", fcb->filename);
		item = item->next;
	}

	item = dir->subDirs.first;
	while (item)
	{
		subDir = (DIR *)item;
		for (int j = 0; j < i; ++j)
			printf("\t ");
		listDir(disk, subDir);
		item = item->next;
	}
	--i;
}
