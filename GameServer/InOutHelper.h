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
void preProcess(char *inputMes, USER &user, int &clientWindowStt) {
	/*this is preprocess function
	[IN/OUT] inputMes: the inputdata to process,
	[in]clientWindowStt: clientWindowStt of perHandleData
	[in] user: user of perHandleData
	*/
	if ((inputMes[0] == char(LOGOUT_MESSAGE))) {
		int tmp = user.id;
		inputMes[1] = (tmp / (256 * 256)) % 256;
		inputMes[2] = (tmp / 256) % 256;
		inputMes[3] = tmp % 256;
	} else if ((inputMes[0] == char(GET_LIST_PLAYER))) {
		int tmp = user.id;
		inputMes[1] = (tmp / (256 * 256)) % 256;
		inputMes[2] = (tmp / 256) % 256;
		inputMes[3] = tmp % 256;
	}
}
void updateClientStatus(int returnMess,char *inputMes, int &length,USER &user ,int &clientWindowStt) {
	/*this is post-process function after recv message, update perHandleData of client
	[in] returnMess: the code of the return message
	[IN] inputMes: the inputdata to process, usually is the outputMes2 from revMessage
	[in] length: length of inputMes
	[out]clientWindowStt: clientWindowStt of perHandleData
	[out] user: user of perHandleData
	*/
	switch (returnMess)
	{
	case(LOGIN_SUCCESS): {
		clientWindowStt = 1;
		memcpy(&user, inputMes, length);
		break;
	}
	case(LOGOUT_SUCCESS): {
		clientWindowStt = 0;
		break;
	}
	}
}
void revMessage(char *inputMes, char *outputMes1, int &lengthOutMes1, char *outputMes2, int &lengthOutMes2) {
	/*this is process function after recv message
	[IN] inputMes: the inputdata to process, usually is the messages recv from client afte pre-processing
	[out] outputMes1: the output of the process, usually is the messages send back to client
	[out] lengthOutMes1; length of outputMes1
	[out] outputMes2: the output of the process, usually is the messages send back to another client ( rival player in the game)
	sometime is the raw data to update perHandleData of client
	[out] lengthOutMes2; length of outputMes2
	*/
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
			strcpy_s(nickNameAndRank, tmp.c_str());
			lengthOutMes1 = strlen(nickNameAndRank) + 4;
			makeItCombine(LOGIN_SUCCESS, nickNameAndRank, lengthOutMes1 - 4, outputMes1);
			lengthOutMes2 = sizeof(result);
			memcpy(outputMes2, &result, lengthOutMes2);// prepare data to post-process
		}
	}
	else if (inputMes[0] == char(LOGOUT_MESSAGE)) {
		int id = headerHandle(inputMes);
		logout(id);
		outputMes1[0] = char(LOGOUT_SUCCESS);
		outputMes1[1] = 0;
		outputMes1[2] = 0;
		outputMes1[3] = 0;
	}if (inputMes[0] == char(GET_LIST_PLAYER)) {
		int id = headerHandle(inputMes);
		vector <USER> listActiveUser = getlistUsersActive();
		string data;
		show(listActiveUser);
		for (int i = 0; i < listActiveUser.size(); i++) {
			if (id != listActiveUser.at(i).id) {
				data += listActiveUser.at(i).nickname;
				data += " ";
				data += to_string(listActiveUser.at(i).point);
				data += " ";
			}
		}
		char data_c[5120];
		strcpy(data_c, data.c_str());
		makeItCombine(LIST_PLAYER, data_c, strlen(data_c), outputMes1);
		lengthOutMes1 = strlen(data_c) + 4;
	}
}