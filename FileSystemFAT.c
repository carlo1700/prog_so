#include "FileSystemFAT.h"
#include "prints.h"

openFileInfo **ofiTable;
int openedFiles;

FileHandle *createFile(FileSystemFAT *fs, char *path, char *name, mode_type mode)
{
	FCB				*currentFCB;
	FCB				*retFCB;
	openFileInfo	*ofi;
	FileHandle		*ret;

	if (!nameIsValid(name))
		return NULL;

	currentFCB = createDirectoryWrapped(fs, path);
	if (!currentFCB)
		return NULL;

	retFCB = findFCB(fs, currentFCB, name);

	if(retFCB == NULL)
	{
		ofi = newOpenFileInfo(ofiTable, &openedFiles);
		if(!ofi)
			return NULL;
		ret = newFileHandle(ofi, mode);
		if(!ret)
		{
			remOpenFileInfo(ofiTable, &openedFiles, ofi);
			return NULL;
		}
		retFCB = createFCB(fs, currentFCB, name, 0);
		if (!retFCB)
		{
			remOpenFileInfo(ofiTable, &openedFiles, ofi);
			free(ret);
			return NULL;
		}
		ofi->fileSystem = fs;
		ofi->fcb = retFCB;
		return ret;
	}
	else
	{
		ofi = findOpenFileInfo(ofiTable, retFCB);
		if (!ofi)
		{
			ofi = newOpenFileInfo(ofiTable, &openedFiles);
			if(!ofi)
				return NULL;
			ofi->fileSystem = fs;
			ofi->fcb = retFCB;
			ret = newFileHandle(ofi, mode);
			if(!ret)
			{
				remOpenFileInfo(ofiTable, &openedFiles, ofi);
				return NULL;
			}
			return ret;
		}
		ret = newFileHandle(ofi, mode);
		if(!ret)
			return NULL;
		return ret;
	}
}

void eraseFile(FileSystemFAT *fs, char *path, char *name)
{
	char	*path_copy;
	char	*pathSegment;
	FCB		*currentFCB;
	FCB		*oldFCB;

	if (!nameIsValid(name))
		return;

	if (!pathIsValid(path))
		return;

	path_copy = strdup(path);
	pathSegment = strtok(path_copy, "/");

	if (strcmp(pathSegment, ROOT_DIR_NAME) != 0)
	{
		printf("%s",pathSegment);
		printf(RED"\neraseFile: path doesn't start with "RESET);
		printf(ROOT_DIR_NAME);
		printf("\n");
		return;
	}

	currentFCB = fs->rootFCB;

	pathSegment = strtok(NULL, "/");

	while(pathSegment != NULL)
	{
		oldFCB = currentFCB;
		currentFCB = findFCB(fs, oldFCB, pathSegment);
		if (currentFCB == NULL)
		{
			printf(PURPLE"\neraseFile: Directory not found\n"RESET);
			return;
		}
		pathSegment = strtok(NULL, "/");
	}

	oldFCB = currentFCB;
	currentFCB = findFCB(fs, oldFCB, name);
	if (currentFCB == NULL)
	{
		printf(RED"\neraseFile: File not found\n"RESET);
		return;
	}

	if (findOpenFileInfo(ofiTable, currentFCB))
	{
		printf(RED"\neraseFile: Can't delete opened File\n"RESET);
		return;
	}

	removeFromDir(fs, oldFCB, currentFCB);
	deleteFCB(fs, currentFCB);
}

void createDirectory(FileSystemFAT *fs, char *path)
{
	createDirectoryWrapped(fs, path);
}

void eraseDirectory(FileSystemFAT *fs, char *path)
{
	char				*path_copy;
	char				*pathSegment;
	DirectoryEntryMin	*dirEntry;
	FCB					*oldFCB;
	FCB					*currentFCB;

	if (!pathIsValid(path))
		return;

	path_copy = strdup(path);
	pathSegment = strtok(path_copy, "/");

	if (strcmp(pathSegment, ROOT_DIR_NAME) != 0)
	{
		printf("%s",pathSegment);
		printf(RED"\npath doesn't start with "RESET);
		printf(ROOT_DIR_NAME);
		printf("\n");
		return;
	}
	currentFCB = fs->rootFCB;

	pathSegment = strtok(NULL, "/");
	if (pathSegment == NULL)
	{
		printf(RED"\nCan't delete root FCB\n"RESET);
		return;
	}
	while(pathSegment != NULL)
	{
		oldFCB = currentFCB;
		currentFCB = findFCB(fs, oldFCB, pathSegment);
		if (currentFCB == NULL)
		{
			printf(RED"eraseDirectory: %s not found in %s \n\nn"RESET, pathSegment, oldFCB->fileName);
			return;
		}
		pathSegment = strtok(NULL, "/");
	}

	dirEntry = (DirectoryEntryMin *)currentFCB->data;
	if (dirEntry->numFCBS > 0)
	{
		printf(RED"eraseDirectory: %s not empty\n\n"RESET, currentFCB->fileName);
		return;
	}

	removeFromDir(fs, oldFCB, currentFCB);
	deleteFCB(fs, currentFCB);
}

void listDirectory(FileSystemFAT *fs, char *path)
{
	FCB					*currentFCB;
	FCB					*oldFCB;
	FCB					*scanningFCB;
	DirectoryEntry		*de;
	DirectoryEntryMin	*deMin;
	char				*path_copy;
	char				*pathSegment;
	int					i;
	int					count;
	int					block;

	if (!pathIsValid(path))
		return;

	path_copy = strdup(path);
	pathSegment = strtok(path_copy, "/");

	if (strcmp(pathSegment, ROOT_DIR_NAME) != 0)
	{
		printf("%s",pathSegment);
		printf(RED"\npath doesn't start with "RESET);
		printf(ROOT_DIR_NAME);
		printf("\n");
		return;
	}

	currentFCB = fs->rootFCB;

	pathSegment = strtok(NULL, "/");
	while(pathSegment != NULL)
	{
		oldFCB = currentFCB;
		currentFCB = findFCB(fs, oldFCB, pathSegment);
		if (currentFCB == NULL)
		{
			printf(RED"\nDirectory not found\n"RESET);
			return;
		}
		pathSegment = strtok(NULL, "/");
	}

	if (currentFCB->isDirectory == 0)
	{
		printf(RED "%s is a file path\n"RESET, path);
		return;
	}
	printf(BOLDCYAN"%s: "RESET, currentFCB->fileName);
	deMin = (DirectoryEntryMin *)currentFCB->data;
	i = 0;
	count = 0;
	while (count < deMin->numFCBS)
	{
		scanningFCB = deMin->FCBS[i++];
		if (scanningFCB == NULL)
			continue;
		else if (scanningFCB->isDirectory == 1)
			printf(BOLDMAGENTA" %s/ "RESET, scanningFCB->fileName);
		else
			printf(" %s ", scanningFCB->fileName);
		count++;
	}
	block = 1;
	while (currentFCB->BlockCount < block)
	{
		de = getDirBlock(fs, currentFCB, block);
		i = 0;
		count = 0;
		while (count < de->numFCBS)
		{
			scanningFCB = de->FCBS[i++];
			if (scanningFCB == NULL)
				continue;
			else if (scanningFCB->isDirectory == 1)
				printf(CYAN " %s/"RESET, scanningFCB->fileName);
			else
				printf(" %s ", scanningFCB->fileName);
			count++;
		}
	}
	printf("\n\n");
}

void changeDirectory(FileSystemFAT *fs, char *name, char *oldPath, char *newPath)
{
	FCB					*currentFCB;
	FCB					*oldFCB;
	FCB					*newDirFCB;
	char				*path_copy;
	char				*pathSegment;

	if (!pathIsValid(oldPath))
		return;

	if (!nameIsValid(name))
		return;

	path_copy = strdup(oldPath);
	pathSegment = strtok(path_copy, "/");

	if (strcmp(pathSegment, ROOT_DIR_NAME) != 0)
	{
		printf("%s",pathSegment);
		printf(RED"\npath doesn't start with "RESET);
		printf(ROOT_DIR_NAME);
		printf("\n");
		return;
	}

	currentFCB = fs->rootFCB;

	pathSegment = strtok(NULL, "/");

	while(pathSegment != NULL)
	{
		oldFCB = currentFCB;
		currentFCB = findFCB(fs, oldFCB, pathSegment);
		if (currentFCB == NULL)
		{
			printf(PURPLE"\nDirectory not found\n"RESET);
			return;
		}
		pathSegment = strtok(NULL, "/");
	}

	oldFCB = currentFCB;
	currentFCB = findFCB(fs, oldFCB, name);
	removeFromDir(fs, oldFCB, currentFCB);

	newDirFCB = createDirectoryWrapped(fs, newPath);
	addToDir(fs, currentFCB, newDirFCB);
}


int close(FileHandle *fh)
{
	int ret;

	fh->info->numFileHandle = fh->info->numFileHandle - 1;
	if (fh->info->numFileHandle == 0)
	{
		ret = remOpenFileInfo(ofiTable, &openedFiles, fh->info);
		if (ret)
		{
			fh->info->numFileHandle = fh->info->numFileHandle + 1;
			return -1;
		}
	}
	free(fh);
	return 0;
}


int fs_read(FileHandle *fd, void *buf, int count)
{
	int			i;
	int			ret;
	int			blockToRead;
	char		*data;
	FileEntry	*fe;

	if (fd->permissions != R && fd->permissions != W_R && fd->permissions != EX_R && fd->permissions != EX_W_R)
	{
		printf(RED "File is not readable\n"RESET);
		return -1;
	}

	if (count > MAX_READ_WRITE_SIZE)
		count = MAX_READ_WRITE_SIZE;

	ret = 0;
	if (fd->info->fcb->fileSize < fd->offset + count)
		count = fd->info->fcb->fileSize - fd->offset;

	while (count > 0)
	{
		blockToRead = fd->offset / BLOCK_SIZE;
		if (blockToRead == 0)
			data = fd->info->fcb->data;
		else
		{
			fe = getFileDataBlock(fd->info->fileSystem, fd->info->fcb, blockToRead);
			if (fe == NULL)
				return ret;
			data = fe->data;
		}

		i = 0;
		while (i < count && fd->offset % BLOCK_SIZE + i < BLOCK_SIZE)
		{
			((char *)buf)[ret] = data[fd->offset % BLOCK_SIZE + i];
			i++;
			ret++;
		}
		count -= i;
		fd->offset += i;
	}
	return ret;
}

int fs_write(FileHandle *fd, const void *buf, int count)
{
	int			i;
	int			ret;
	int			blockToWrite;
	char		*data;
	FileEntry	*fe;

	if (fd->permissions != W && fd->permissions != W_R && fd->permissions != EX_W && fd->permissions != EX_W_R)
	{
		printf(RED "File is not writable\n"RESET);
		return -1;
	}

	if (count > MAX_READ_WRITE_SIZE)
		count = MAX_READ_WRITE_SIZE;

	ret = 0;
	while (count > 0)
	{
		blockToWrite = fd->offset / BLOCK_SIZE;
		if (blockToWrite == 0)
			data = fd->info->fcb->data;
		else
		{
			fe = createFileDataBlock(fd->info->fileSystem, fd->info->fcb, blockToWrite);
			if (fe == NULL)
				return ret;
			data = fe->data;
		}
		i = 0;
		while (i < count && fd->offset % BLOCK_SIZE + i < BLOCK_SIZE)
		{
			data[fd->offset % BLOCK_SIZE + i] = ((char *)buf)[ret];
			i++;
			ret++;
		}
		count -= i;
		fd->offset += i;
	}
	fd->info->fcb->fileSize += ret;
	return ret;
}

int fs_seek(FileHandle *fd, unsigned int offset, int whence)
{
	int ret;

	if (whence == SEEK_SET)
	{
		if (offset > fd->info->fcb->fileSize)
		{
			printf(RED "Offset is greater than file size\n"RESET);
			return -1;
		}
		fd->offset = offset;
		ret = offset;
	}
	else if (whence == SEEK_CUR)
	{
		if (fd->offset + offset > fd->info->fcb->fileSize)
		{
			printf(RED "Offset is greater than file size\n"RESET);
			return -1;
		}
		fd->offset += offset;
		ret = fd->offset;
	}
	else if (whence == SEEK_END)
	{
		if (fd->info->fcb->fileSize + offset > fd->info->fcb->fileSize)
		{
			printf(RED "with SEEK_END the offset must be 0\n"RESET);
			return -1;
		}
		fd->offset = fd->info->fcb->fileSize + offset;
		ret = fd->offset;
	}
	else
	{
		printf(RED "Invalid whence\n"RESET);
		return -1;
	}
	return ret;
}

void initOpenFileInfo()
{
	openedFiles = 0;
	ofiTable = (openFileInfo **)malloc(sizeof(openFileInfo *) * (MAX_OPEN_FILES + 1));
	for (int i = 0; i < MAX_OPEN_FILES; i++)
	{
		ofiTable[i] = (openFileInfo *)malloc(sizeof(openFileInfo));
		ofiTable[i]->isUsed = 0;
		ofiTable[i]->numFileHandle = 0;
		ofiTable[i]->fcb = NULL;
		ofiTable[i]->fileSystem = NULL;
	}
	ofiTable[MAX_OPEN_FILES] = NULL;

}

void printOpenFileInfo()
{
	printf(CYAN "OpenFileInfo: \n" RESET);
	printf("numOpenFiles: %d\n", openedFiles);

	for (int i = 0; i < MAX_OPEN_FILES; i++)
	{
		if (ofiTable[i]->isUsed == 1)
		printf("[%d]: %s\n", i + 1, ofiTable[i]->fcb->fileName);
	}
	printf("\n");
}
