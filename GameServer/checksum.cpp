#include "stdafx.h"
#include <iostream>
#include <stdio.h>


int checksum(char * fileData, int length) {
	int rs = 0;
	for (int i = 0; i < length; i++) {
		rs += (int)fileData[i];
	}
	return rs;
}