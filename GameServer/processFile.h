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

void randomString(char* stringRandom, int stringLength);
void writeFile( char* fileName, string data);
int deleteFile(const char* file_path);
int readFile(char *fileName,string &line);