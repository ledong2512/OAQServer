#pragma once
#include "stdafx.h" 
#include <stdio.h> 
#include <cstdint>
#include <conio.h> 
#pragma warning(disable:4996)
#include <string>
#include "cstring"
#include "process.h"
#include "string.h"
#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;
/*
Function: creates a random string ending in .txt
Param: + stringRandom: [out] save the result
	   + stringLength:[IN] length of string need random
*/
void randomString(char* stringRandom, int stringLength);

/*
Function: write string content to file
Param: + fileName: [in] path of file
+ data:[IN] length of string need random
*/
void writeFile( char* fileName, string data);
/*
Function: deleteFile
Param: + file_path: [in] path of file
Return: +1 :delete file success
		+0 : fail
*/
int deleteFile(const char* file_path);
/*
Function: read string content to file
Param: + fileName: [in] path of file
+ line:[out] result
*/
int readFile(char *fileName,string &line);