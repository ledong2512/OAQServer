#pragma once
#include<Windows.h>
#include"MessagesCode.h"
#include"process_database.h"
#pragma warning (disable:4996)
extern map <string, SOCKET> nickNameToSOCKET;
extern map <int, string> offerFightToNickname;
extern int offerFightNumber ;
extern map <int, GAME> gameNum;
extern int boardNum;
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
void getIdAndPass(char *id, char*pass,char* message) { //[IN] message, [OUT]id,pass: the id and password from message or 2 string split by ' '
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
	} 
	else if ((inputMes[0] == char(GET_LIST_PLAYER))) {
		int tmp = user.id;
		inputMes[1] = (tmp / (256 * 256)) % 256;
		inputMes[2] = (tmp / 256) % 256;
		inputMes[3] = tmp % 256;
	}
	else if ((inputMes[0] == char(CHAT))) {
		if (user.lastTime != 0) {
			auto sec =  Clock::now() - std::chrono::system_clock::from_time_t(user.lastTime);
			if (std::chrono::duration_cast<std::chrono::seconds>(sec).count() < 30) {
				inputMes[1] = 0;
				inputMes[2] = 0;
				inputMes[3] = 0;
				return;
			}
		}
		auto now = Clock::now();
		user.lastTime = Clock::to_time_t(now);
		int length = headerHandle(inputMes);
		char data_c[5120];
		memcpy(data_c, inputMes + 4, length);
		data_c[length] = 0;
		string name = "["+user.nickname+"]" + " : ";
		name += string(data_c);
		strcpy(data_c, name.c_str());
		makeItCombine(CHAT, data_c, strlen(data_c), inputMes);
	}
	else if ((inputMes[0] == char(CONNECT_TO_PLAY))) {
		char rivalNickName[100], playerName[100];
		getIdAndPass(rivalNickName, playerName, inputMes);
		string check = string(playerName);
		if (check != user.nickname ) {
			inputMes[0] = WORNG_SYSTAX;
			inputMes[1] = 0;
			inputMes[2] = 0;
			inputMes[3] = 0;
			return;
		};
		if (clientWindowStt != 1) {
			inputMes[0] = CONNECT_TO_PLAY;
			inputMes[1] = 0;
			inputMes[2] = 0;
			inputMes[3] = 0;
			return;
		}
		USER checkU = searchUserByNickname(string(rivalNickName));
		if (abs(checkU.point-user.point)>10) {
			inputMes[0] = RANK_DIF_ERROR;
			inputMes[1] = 0;
			inputMes[2] = 0;
			inputMes[3] = 0;
			return;
		}
	}
	else if ((inputMes[0] == char(AGREE_TO_PLAY))) {
		string name;
		char name_c[100],num_c[10];
		getIdAndPass(name_c, num_c, inputMes);
		name = string(name_c);
		if (offerFightToNickname[atoi(num_c)] != name|| clientWindowStt!=1) {// worng systax
			inputMes[1] = WORNG_SYSTAX;
			inputMes[1] = 0;
			inputMes[2] = 0;
			inputMes[3] = 0;
		}
		else {
			char data_c[5120];
			name += " ";
			name += user.nickname;
			strcpy(data_c, name.c_str());
			makeItCombine(AGREE_TO_PLAY, data_c, strlen(data_c), inputMes);
		}
	}
	else if ((inputMes[0] == char(LETS_PLAY))) {
		int j, k = 0, check = 0;

		char number[10];
		int len = headerHandle(inputMes);
		for (int i = 0;i < len;i++) {
			number[i] = inputMes[i + 4];
		}
		number[len] = 0;
		int num = atoi(number);
		while (gameNum[num].inUse == 1) {
			Sleep(20);
		}
		gameNum[num].inUse = 1;
		for (int i = 0;i < 2;i++) {
			if (gameNum[num].ready[i] == 0) {
				k ++;
			}
		}
		for (int i = 0;i < 2;i++) {
			if (gameNum[num].player[i] == user.nickname) {
				gameNum[num].ready[i] = 1;j = i;
			}
			else check++;
		}
		if (check == 2||k==2) {
			inputMes[0] = WORNG_SYSTAX;
			inputMes[1] = 0;
			inputMes[2] = 0;
			inputMes[3] = 0;
			gameNum[num].inUse = 0;
			return;
		}
		printf("check point \n");
		inputMes[1] = j;// because number of game alway <10000 <256^2 then we use the first byte to perform the number of user turn in game
		inputMes[2] = (num / 256) % 256;
		inputMes[3] = num % 256;
		gameNum[num].inUse = 0;
		return;
		
	}
}
void updateClientStatus(int returnMess,char *inputMes, int &length,USER &user ,int &clientWindowStt,SOCKET &client) {
	/*this is post-process function after recv message, update perHandleData of client
	[in] returnMess: the code of the return message
	[IN] inputMes: the inputdata to process, usually is the outputMes2 from revMessage
	[in] length: length of inputMes
	[out]clientWindowStt: clientWindowStt of perHandleData
	[out] user: user of perHandleData
	[in] socket: help to update datastruct nickNameToSOCKET
	*/
	switch (returnMess)
	{
	case(LOGIN_SUCCESS): {
		clientWindowStt = 1;
		
		memcpy(&user, inputMes, length);
		nickNameToSOCKET[user.nickname] = client;
		break;
	}
	case(LOGOUT_SUCCESS): {
		clientWindowStt = 0;
		break;
	}
	}
}
void revMessage(char *inputMes, char *outputMes1, int &lengthOutMes1, char *outputMes2, int &lengthOutMes2,SOCKET &rival, int &sendType) {
	/*this is process function after recv message
	[IN] inputMes: the inputdata to process, usually is the messages recv from client afte pre-processing
	[out] outputMes1: the output of the process, usually is the messages send back to client
	[out] lengthOutMes1; length of outputMes1
	[out] outputMes2: the output of the process, usually is the messages send back to another client ( rival player in the game)
	sometime is the raw data to update perHandleData of client
	[out] lengthOutMes2; length of outputMes2
	[out] rival: SOCKET of rival client
	[out] send type 0 is send to no one, 1 is send back, 2 is send to rival client, 3 is send to both, 4 is broad cass
	*/
	lengthOutMes1 = 4;
	lengthOutMes2 = 4;
	sendType = 1;
	if (inputMes[0] == char(WORNG_SYSTAX)) {
		outputMes1[0] = 0;
		outputMes1[1] = 0;
		outputMes1[2] = 0;
		outputMes1[3] = 0;
	}
	else if (inputMes[0] == char(LOGIN_MESSAGE)) {
		char id[50], pass[50];
		USER result;
		getIdAndPass(id, pass, inputMes);
		result = login(id, pass);
		if (result.id == -1) {// worng information
			lengthOutMes1 = strlen(WORNG_INFOMATION) + 4;
			makeItCombine(ERROR_MESSAGE, WORNG_INFOMATION, lengthOutMes1 - 4, outputMes1);

		}
		else if (result.id == -2) {//already loged in on another client
			lengthOutMes1 = strlen(ALREADY_LOGGED_IN) + 4;
			makeItCombine(ERROR_MESSAGE, ALREADY_LOGGED_IN, lengthOutMes1 - 4, outputMes1);
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
	}
	else if (inputMes[0] == char(GET_LIST_PLAYER)) {
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
	else if (inputMes[0] == char(CONNECT_TO_PLAY)) {
		if (headerHandle(inputMes) == 0) {
			makeItCombine(ERROR_MESSAGE, RIVAL_OFFLINE, 3, outputMes1);
			lengthOutMes1 = 7;
			return;
		}
		sendType = 2;
		char rivalNickName[100],playerName[100];
		getIdAndPass(rivalNickName, playerName, inputMes);
		rival = nickNameToSOCKET[rivalNickName];
		offerFightNumber++;
		offerFightNumber %= 10000;
		int num = offerFightNumber;
		--num;
		offerFightToNickname[num] = playerName;
		char numChar[10];
		_itoa_s(num, numChar, 10);
		strcat(playerName, " ");
		strcat(playerName, numChar);
		makeItCombine(CONNECT_TO_PLAY, playerName, strlen(playerName), outputMes2);
		lengthOutMes2 = strlen(playerName) + 4;
		//prepare error mess
		makeItCombine(ERROR_MESSAGE, RIVAL_OFFLINE, 3, outputMes1);
		lengthOutMes1 = 7;

	}
	else if (inputMes[0] == char(CHAT)) {
		sendType = 4;
		char data_c[5120];
		int length = headerHandle(inputMes);
		if (length == 0) {
			sendType = 1;
			makeItCombine(ERROR_MESSAGE, MESSAGE_NOT_YET, 3, outputMes1);
			lengthOutMes1 = 7;
			return;
		}
		memcpy(data_c, inputMes + 4, length);
		data_c[length] = 0;
		makeItCombine(CHAT, data_c, length, outputMes1);
		lengthOutMes1 = length + 4;
	}
	else if (inputMes[0] == char(RANK_DIF_ERROR)) {
		makeItCombine(ERROR_MESSAGE, DIFF_RANK, 3, outputMes1);
		lengthOutMes1 = 7;
	}
	else if (inputMes[0] == char(AGREE_TO_PLAY)) {
		sendType = 3;
		char rivalNickName[100], playerName[100];
		getIdAndPass(rivalNickName, playerName, inputMes);
		rival = nickNameToSOCKET[rivalNickName];
		boardNum++;
		boardNum %= 10000;
		int num = boardNum;
		--num;
		GAME newgame;
		newgame.number = num;
		newgame.ready[0] = 0;newgame.ready[1] = 0;
		int ranN = rand() % 2;
		newgame.player[ranN] = string(playerName);
		ranN++;ranN %= 2;
		newgame.player[ranN] = string(rivalNickName);
		gameNum[num] = newgame;


		char numChar[10];
		_itoa_s(num, numChar, 10);
		strcat(playerName, " ");strcat(rivalNickName, " ");
		strcat(rivalNickName, numChar);strcat(playerName, numChar);
		makeItCombine(START, rivalNickName, strlen(rivalNickName), outputMes2);
		lengthOutMes2 = strlen(rivalNickName) + 4;
		makeItCombine(START, playerName, strlen(playerName), outputMes1);
		lengthOutMes1 = strlen(playerName) + 4;
	}
	else if (inputMes[0] == char(LETS_PLAY)) {
		printf("check point 1\n");
		sendType = 3;
		int k;
		k = inputMes[1];
		if (k < 0) k += 256;
		inputMes[1] = 0;
		int num = headerHandle(inputMes);
		rival = nickNameToSOCKET[gameNum[num].player[(k + 1) % 2]];
		char rivalTurn[2], playerTurn[2],playerMes[150],rivalMes[150];
		rivalTurn[1] = 0;rivalTurn[0] = ((k + 1) % 2) + '0';
		playerTurn[1] = 0;playerTurn[0] = k + '0';
		strcpy(playerMes, gameNum[num].player[(k + 1) % 2].c_str());strcat(playerMes, " ");strcat(playerMes, playerTurn);
		strcpy(rivalMes, gameNum[num].player[k].c_str());strcat(rivalMes, " ");strcat(rivalMes, rivalTurn);
		makeItCombine(LETS_PLAY, rivalMes, strlen(rivalMes), outputMes2);
		lengthOutMes2 = strlen(rivalMes)+4;
		makeItCombine(LETS_PLAY, playerMes, strlen(playerMes), outputMes1);
		lengthOutMes1 = strlen(playerMes) + 4;
	}
}