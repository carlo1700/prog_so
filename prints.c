#include "prints.h"
#include "utils.h"

void printFCB(FileSystemFAT *fs, FCB *fcb)
{
	char	*data;
	int		deep;

    printf("   FCB: ");
    printf("	fileName: %s | ", fcb->fileName);
    printf("isDirectory: %d | ", fcb->isDirectory);
    printf("BlockCount: %d | ", fcb->BlockCount);
    printf("FATNextIndex: %d | ", fcb->FATNextIndex);
	printf("FATIndex: %d | ", getBlockIdx(fs, (char *)fcb));
	printf("fileSize: %ld | ", fcb->fileSize);
    printf("data: ");

	deep = 0;
	data = fcb->data;
	do {
		if (deep > 0)
			printf(PURPLE"%d: "RESET, deep);
		for (unsigned long int i = 0; i < sizeof(data); i++) {
		if (data[i] != 0)
			printf("%02X ", (unsigned char)data[i]);
		}
		deep++;
		data = (char *) getDirBlock(fs, fcb, deep);
	} while (data != NULL);
}

void printDirectoryTree(FileSystemFAT *fs, FCB *dirFCB, int deep)
{
	DirectoryEntry		*de;
	DirectoryEntryMin	*deMin;
	int					i;
	int					count;
	char				color[20];

	switch (deep % 5)
	{
	case 0:
		strcpy(color, BOLDMAGENTA);
		break;
	case 1:
		strcpy(color, BOLDCYAN);
		break;
	case 2:
		strcpy(color, BOLDPURPLE);
		break;
	case 3:
		strcpy(color, BOLDYELLOW);
		break;
	case 4:
		strcpy(color, BOLDBLUE);
		break;
	default:
		break;
	}

	if (dirFCB == NULL || dirFCB->isDirectory == 0)
	{
		printf(RED "printDirectoryTree: dirFCB is NULL or not a directory\n"RESET);
		return;
	}

	if(strcmp(dirFCB->fileName, ROOT_DIR_NAME) == 0)
		printf(BOLDGREEN "%s\n[\n" RESET, dirFCB->fileName);

	deMin = (DirectoryEntryMin *)dirFCB->data;
	i = 0;
	count = 0;
	while (count < deMin->numFCBS)
	{
		if (deMin->FCBS[i] == NULL)
		{
			i++;
			continue;
		}
		else if (deMin->FCBS[i]->isDirectory == 1)
		{
			printf(color);
			printf("	%s ["RESET, deMin->FCBS[i]->fileName);
			printDirectoryTree(fs, deMin->FCBS[i], deep + 1);
			printf(color);
			printf("]\n"RESET);
		}
		else
			printf("%s - ", deMin->FCBS[i]->fileName);
		i++;
		count++;
	}
	int h = 0;
	de = getDirBlock(fs, dirFCB, ++h);
	while (de != NULL)
	{
		i = 0;
		count = 0;
		while (count < de->numFCBS)
		{
			if (de->FCBS[i] == NULL)
			{
				i++;
				continue;
			}
			else if (de->FCBS[i]->isDirectory == 1)
			{
				printf(color);
				printf("	%s ["RESET, de->FCBS[i]->fileName);
				printDirectoryTree(fs, de->FCBS[i], deep + 1);
				printf(color);
				printf("]\n"RESET);
			}
			else
				printf("%s ", de->FCBS[i]->fileName);
			i++;
			count++;
		}
		de = getDirBlock(fs, dirFCB, ++h);
	}
}

void printFS(FileSystemFAT *fs, const char* option) {


	if (strcmp(option, "baseInfo") == 0) {
		printf(CYAN "FileSystemFAT:\n" RESET);
		printf(CYAN "diskSize:" RESET " %d bytes, %d MB\n", fs->diskSize, fs->diskSize / 1000000);
		printf(CYAN "blockSize:" RESET " %d bytes\n", fs->blockSize);
		printf(CYAN "blockNum:" RESET " %d\n", fs->blockNum);
		printf(CYAN "numFCBS:" RESET" %d\n", fs->numFCBS);
	}
	else if (strcmp(option, "bitMap") == 0) {
        printf(CYAN "bitMap: \n" RESET);
        for (unsigned long int i = 0; i < sizeof(fs->bitMap); i++) {
            unsigned char byte = fs->bitMap[i];
            for (int j = 0; j < 8; j++) {
                int bit = (byte >> j) & 1;
				if (bit == 1)
					printf(GREEN "%d" RESET, bit);
				else
                	printf("%d", bit);
            }
        }
        printf("\n");
    }
	else if (strcmp(option, "tableFAT") == 0) {

		printf(CYAN "tableFAT: \n" RESET);
		for (unsigned long int i = 0; i < sizeof(fs->tableFAT) / sizeof(fs->tableFAT[0]); i++) {
			printf("%ld[%d], ", i, fs->tableFAT[i]);
		}
		printf("\n");
	}
	else if (strcmp(option, "fcbList") == 0) {
		int count = 0;
        printf(CYAN "fcbList: \n" RESET);
		for (int i = 0; count < fs->numFCBS && i < MAX_FCBS; i++) {
			if (fs->fcbList[i] != NULL)
			{
				count++;
				printf("[");
				printf("p: %p |", fs->fcbList[i]);
				printFCB(fs, (FCB *)fs->fcbList[i]);
				printf("]\n");
			}
			else
				printf(" [empty]\n");
		}
		if (count == 0)
			printf(" [empty]\n");
        printf("\n");
    }
	else if (strcmp(option, "diskBuffer") == 0) {

		printf(CYAN "diskBuffer (1/1024): \n" RESET);

		int smallSize = DISK_DATA_SIZE / 1024;

		for (int i = 0; i < smallSize; i++) {
			printf("%02X ", fs->diskBuffer[i] & 0xFF); // Stampa il byte in esadecimale

			// Verifica se il conteggio raggiunge un multiplo della dimensione del blocco
			if ((i + 1) % (BLOCK_SIZE) == 0) {
				printf(BLUE "\n|block|\n" RESET); // Stampa una barra verticale colorata
			}
		}
		printf("\n");
	}
	else if (strcmp(option, "directoryTree") == 0) {
		printf(CYAN "directoryTree: \n" RESET);
		printDirectoryTree(fs, fs->rootFCB, 0);
		printf(BOLDGREEN "]" RESET);
		printf("\n");
	}
	else if (strcmp(option, "all") == 0) {
		printFS(fs, "baseInfo");
		printFS(fs, "bitMap");
		printFS(fs, "tableFAT");
		printFS(fs, "fcbList");
		printFS(fs, "diskBuffer");
		printFS(fs, "directoryTree");
	}
	else {
		printf(RED "printFS: option not valid\n" RESET);
        printf("option = {'all', 'directoryTree', 'baseInfo, 'bitMap', 'tableFAT', 'fcbList', 'diskBuffer'}.\n");
    }
    printf("\n");
}

void printFileContent(FileSystemFAT *fs, FCB *fileFCB) {

	if (fileFCB->isDirectory == 1) {
		printf(RED "printFileContent: fileFCB is a directory\n" RESET);
		return;
	}

	int i = 0;
	printf (CYAN"%s content: "GRAY, fileFCB->fileName);

	while (fileFCB->data[i] != '\0')
		printf("%c", fileFCB->data[i++]);

	int h = 0;
	FileEntry *fe = getFileDataBlock(fs, fileFCB, h);
	while (fe != NULL)
	{
		i = 0;
		while (fe->data[i] != '\0')
		{
			printf("%c", fe->data[i]);
			i++;
		}
		fe = getFileDataBlock(fs, fileFCB, ++h);
	}
	printf("\n\n"RESET);
}
