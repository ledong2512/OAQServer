#pragma once
#include"MessagesCode.h"
#include"process_database.h"
#pragma warning (disable:4996)
void makeItCombine(int messageCode, char *data, int dataLength, char *arrayOut) {
	//combine those stuff to arrayOut
	arrayOut[0] = messageCode;
	arrayOut[1] = (dataLength / (256 * 256)) % 256;
	arrayOut[2] = (dataLength / 256) % 256;
	arrayOut[3] = dataLength % 256;
	memcpy(arrayOut + 4, data, dataLength);
}
int headerHandle(char *message) {// handle the response message
	unsigned char length[3];
	length[0] = message[1];length[1] = message[2];length[2] = message[3];
	return int(length[2]) + int(length[1]) * 256 + int(length[0]) * 256 * 256;
}
void getIdAndPass(char *id, char*pass,char* message) { //[IN] message, [OUT]id,pass: the id and password from message
	int i = 4;
	int messageLength = headerHandle(message);
	messageLength += 4;
	for (i = 4;i < messageLength;i++) {
		if (message[i] == ' ') {
			id[i - 4] = 0;++i;break;
		}
		id[i - 4] = message[i];
	}
	for (int j = i;j <= messageLength;++j) pass[j - i] = message[j];
}
void revMessage(char *inputMes, char *outputMes1, int &lengthOutMes1, char *outputMes2, int &lengthOutMes2) {
	lengthOutMes1 = 4;
	lengthOutMes2 = 4;
	
	if (inputMes[0] == char(LOGIN_MESSAGE)) {
		char id[50], pass[50];
		USER result;
		getIdAndPass(id, pass, inputMes);
		result = login(id, pass);
		if (result.id == -1) {
			outputMes1[0] = char(LOGIN_FAIL);
			outputMes1[1] = 0;
			outputMes1[2] = 0;
			outputMes1[3] = 0;
		}
		else {
			char nickNameAndRank[100];
			string tmp = (result.nickname + " " + to_string(result.point));
			strcpy_s(nickNameAndRank,tmp.c_str());
			lengthOutMes1 = strlen(nickNameAndRank) + 4;
			makeItCombine(LOGIN_SUCCESS, nickNameAndRank, lengthOutMes1 - 4, outputMes1);
		}
	}
}