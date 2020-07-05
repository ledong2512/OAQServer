#include "stdafx.h"
#include "processFile.h"
#include <stdio.h> 
#include <cstdint>
#include <conio.h> 

void randomString(char* stringRandom, int stringLength)
{
	srand((int)time(0));
	char* box = "abcdefjhijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int boxLength = strlen(box);

	for (int i = 0; i < stringLength; i++)
	{
		stringRandom[i] = box[rand() % boxLength];
	}
	stringRandom[stringLength] = '.';
	stringRandom[stringLength + 1] = 't';
	stringRandom[stringLength + 2] = 'x';
	stringRandom[stringLength + 3] = 't';
	stringRandom[stringLength + 4] = '\0';
}

void writeFile(char* fileName, string data)
{
	if (fileName != NULL)
	{
		fstream file;
		file.open(fileName, ios::out | ios::app);
		file << data;
		file.close();
	}
}
int deleteFile(const char* file_path)
{
	int ret = remove(file_path);
	return ret = (ret == 0) ? 1 : 0;
}
int readFile(char *fileName,string &line) {
	errno_t err;
	FILE *pFile;
	err = fopen_s(&pFile, fileName, "r+");
	while (!feof(pFile)) {
		
		fscanf_s(pFile, "%s", &line);
	}
	fclose(pFile);
	return 0;
}