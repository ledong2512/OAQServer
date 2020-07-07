// GameServer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <conio.h>
#include <map>
#include<string.h>
#include<iostream>
#include <fstream>
#include "cstring"
#include "process.h"
#include"InOutHelper.h"
#include"process_database.h"
using namespace std;
#define BUFF_SIZE 5120
#define PORT 5500
#define DATA_BUFSIZE 8192
#define RECEIVE 0
#define SEND 1
#pragma comment(lib, "Ws2_32.lib")
#pragma warning (disable:4996)
map <string, SOCKET> nickNameToSOCKET;
map <int, string> offerFightToNickname;
int offerFightNumber;
map <int, GAME> gameNum;
map <int, int> SOCKETtoGame;
map <int, string> SOCKETtoIp;
int boardNum;
// Structure definition
typedef struct {
	WSAOVERLAPPED overlapped;
	WSABUF dataBuff;
	CHAR buffer[DATA_BUFSIZE];
	int bufLen;
	int recvBytes;
	int sentBytes;
	int operation;
} PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA;

typedef struct {
	SOCKET socket;
	SOCKADDR_STORAGE clientAddr;
	int status; //0 is recv header, 1 is recv data;
	int length, length1, length2;
	char  outputMes1[BUFF_SIZE], outputMes2[BUFF_SIZE];
	char inputMess[BUFF_SIZE];
	USER client;
	int clientWindowStt = 0;//0 is not login yet, 1 is loged in, 2 is in the game
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;
unsigned __stdcall autoMoveThread(LPVOID variable);
unsigned __stdcall cleanThread(LPVOID variable);
unsigned __stdcall serverWorkerThread(LPVOID CompletionPortID);
int _tmain(int argc, char* argv[])
{
	srand((int)time(0));
	boardNum = 0;
	offerFightNumber = 0;
	SOCKADDR_IN serverAddr;
	SOCKET listenSock, acceptSock;
	HANDLE completionPort;
	SYSTEM_INFO systemInfo;
	LPPER_HANDLE_DATA perHandleData;
	LPPER_IO_OPERATION_DATA perIoData;
	DWORD transferredBytes;
	DWORD flags;
	WSADATA wsaData;

	if (WSAStartup((2, 2), &wsaData) != 0) {
		printf("WSAStartup() failed with error %d\n", GetLastError());
		return 1;
	}

	// Step 1: Setup an I/O completion port
	if ((completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL) {
		printf("CreateIoCompletionPort() failed with error %d\n", GetLastError());
		return 1;
	}

	// Step 2: Determine how many processors are on the system
	GetSystemInfo(&systemInfo);

	// Step 3: Create worker threads based on the number of processors available on the
	// system. Create two worker threads for each processor	
	for (int i = 0; i < (int)systemInfo.dwNumberOfProcessors * 2-1; i++) {
		// Create a server worker thread and pass the completion port to the thread
		if (_beginthreadex(0, 0, serverWorkerThread, (void*)completionPort, 0, 0) == 0) {
			printf("Create thread failed with error %d\n", GetLastError());
			return 1;
		}
	}

	// Step 4: Create a listening socket
	if ((listenSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
		printf("WSASocket() failed with error %d\n", WSAGetLastError());
		return 1;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(5500);
	if (bind(listenSock, (PSOCKADDR)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		printf("bind() failed with error %d\n", WSAGetLastError());
		return 1;
	}

	// Prepare socket for listening
	if (listen(listenSock, 20) == SOCKET_ERROR) {
		printf("listen() failed with error %d\n", WSAGetLastError());
		return 1;
	}
	_beginthreadex(0, 0, autoMoveThread, NULL, 0, 0);
	_beginthreadex(0, 0, cleanThread, NULL, 0, 0);
	while (1) {
		// Step 5: Accept connections
		SOCKADDR_IN clientA;
		int clientAddrLen = sizeof(clientA);
		if ((acceptSock = WSAAccept(listenSock, (sockaddr *)&clientA, &clientAddrLen, NULL, 0)) == SOCKET_ERROR) {
			printf("WSAAccept() failed with error %d\n", WSAGetLastError());
			return 1;
		}

		// Step 6: Create a socket information structure to associate with the socket
		if ((perHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA))) == NULL) {
			printf("GlobalAlloc() failed with error %d\n", GetLastError());
			return 1;
		}

		// Step 7: Associate the accepted socket with the original completion port
		printf("Socket number %d got connected...\n", acceptSock);
		perHandleData->socket = acceptSock;
		char sendId[2048] = "";
		char sendPort[50];
		strcat_s(sendId, inet_ntoa(clientA.sin_addr));
		strcat_s(sendId, ":");
		_itoa_s(ntohs(clientA.sin_port), sendPort, 10);
		strcat_s(sendId, sendPort);
		int acINT = acceptSock;SOCKETtoIp[acINT] = string(sendId);
		memcpy(&perHandleData->clientAddr, &clientA, clientAddrLen);
		perHandleData->status = 1;// header is recv-ed
		if (CreateIoCompletionPort((HANDLE)acceptSock, completionPort, (ULONG_PTR)perHandleData, 0) == NULL) {
			printf("CreateIoCompletionPort() failed with error %d\n", GetLastError());
			return 1;
		}

		// Step 8: Create per I/O socket information structure to associate with the WSARecv call
		if ((perIoData = (LPPER_IO_OPERATION_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_OPERATION_DATA))) == NULL) {
			printf("GlobalAlloc() failed with error %d\n", GetLastError());
			return 1;
		}

		ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
		perIoData->sentBytes = 0;
		perIoData->recvBytes = 0;
		perIoData->dataBuff.len = 4;// size of header
		perIoData->dataBuff.buf = perIoData->buffer;
		perIoData->operation = RECEIVE;
		flags = 0;

		if (WSARecv(acceptSock, &(perIoData->dataBuff), 1, &transferredBytes, &flags, &(perIoData->overlapped), NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != ERROR_IO_PENDING) {
				printf("WSARecv() failed with error %d\n", WSAGetLastError());
				return 1;
			}
		}
	}
	_getch();
	return 0;
}

unsigned __stdcall serverWorkerThread(LPVOID completionPortID)
{
	HANDLE completionPort = (HANDLE)completionPortID;
	DWORD transferredBytes;
	LPPER_HANDLE_DATA perHandleData;
	LPPER_IO_OPERATION_DATA perIoData;
	DWORD flags;
	int sendType;
	SOCKET rival;
	while (TRUE) {
		if (GetQueuedCompletionStatus(completionPort, &transferredBytes,
			(PULONG_PTR)&perHandleData, (LPOVERLAPPED *)&perIoData, INFINITE) == 0) {
			printf("Socket number %d got close...\n", perHandleData->socket);
			//logout
			
			int sock = perHandleData->socket;
			if (SOCKETtoGame.find(sock) != SOCKETtoGame.end()) {// we can tell the program that this player is surender
				int gameNN = SOCKETtoGame[sock];
				int turn=-1;
				if (nickNameToSOCKET[gameNum[gameNN].player[0]]== perHandleData->socket) turn = 0;
				if (nickNameToSOCKET[gameNum[gameNN].player[1]] == perHandleData->socket) turn = 1;
				if (turn == 0 || turn == 1) {
					gameNum[gameNN].gameStart = -1;
					gameNum[gameNN].gameBoard.surrender(turn);
					updateAfterGame(gameNum[gameNN].player[(turn + 1) % 2], gameNum[gameNN].player[turn]);
					char endMes[5];
					endMes[0] = char(END_GAME);
					endMes[1] = 0;
					endMes[2] = 0;
					endMes[3] = 0;
					send(nickNameToSOCKET[gameNum[gameNN].player[(turn + 1) % 2]], endMes, 4, 0);
					gameNum[gameNN].readyToDel[turn] = true;
				}
				SOCKETtoGame.erase(sock);
			}
			SOCKETtoIp.erase(sock);
			logout(perHandleData->client.id);
			printf("GetQueuedCompletionStatus() failed with error %d\n", GetLastError());
			return 0;
		}
		// Check to see if an error has occurred on the socket and if so
		// then close the socket and cleanup the SOCKET_INFORMATION structure
		// associated with the socket 
		SOCKADDR_IN clientAddr;
		memcpy(&clientAddr, &perHandleData->clientAddr, sizeof(clientAddr));
		

		if (transferredBytes == 0) {
			printf("Closing socket %d\n", perHandleData->socket);
			if (closesocket(perHandleData->socket) == SOCKET_ERROR) {
				printf("closesocket() failed with error %d\n", WSAGetLastError());
				return 0;
			}
			GlobalFree(perHandleData);
			GlobalFree(perIoData);
			continue;
		}

		// Check to see if the operation field equals RECEIVE. If this is so, then
		// this means a WSARecv call just completed so update the recvBytes field
		// with the transferredBytes value from the completed WSARecv() call
		
		if (perHandleData->status == 1 && perIoData->operation == RECEIVE) {// recv-ed header
			perHandleData->status = 0;// reset status;
			//for (int i = 0;i<4;i++)  printf("%d ", perIoData->dataBuff.buf[i]);
			if (perIoData->dataBuff.buf[0] == char(52)) cout << "check point2" << endl;
			printf("\n");
			perHandleData->length = headerHandle(perIoData->buffer);
			if (perHandleData->length == 0) {// message without payload
				perHandleData->status = 1;// reset status;
				ZeroMemory(perHandleData->inputMess, sizeof(perHandleData->inputMess));// reset data
				memcpy(perHandleData->inputMess, perIoData->buffer, 4);
				ZeroMemory(perHandleData->outputMes1, sizeof(perHandleData->outputMes1));// reset data
				ZeroMemory(perHandleData->outputMes2, sizeof(perHandleData->outputMes2));// reset data
				preProcess(perHandleData->inputMess, perHandleData->client, perHandleData->clientWindowStt);
				revMessage(perHandleData->inputMess, perHandleData->outputMes1, perHandleData->length1, perHandleData->outputMes2, perHandleData->length2, rival, sendType);
				unsigned char tmp = perHandleData->outputMes1[0];
				updateClientStatus((int)tmp, perHandleData->outputMes2, perHandleData->length2, perHandleData->client, perHandleData->clientWindowStt, perHandleData->socket);
				perIoData->recvBytes = transferredBytes;
				perIoData->sentBytes = 0;
				perIoData->operation = SEND;
			}
			else {
				ZeroMemory(perHandleData->inputMess, sizeof(perHandleData->inputMess));// reset data
				memcpy(perHandleData->inputMess, perIoData->buffer, 4);
				perIoData->sentBytes += transferredBytes;
				perIoData->recvBytes = 0;
			}
		}
		else if (perIoData->operation == RECEIVE) {//recv-ed data	
			perHandleData->status = 1;// reset status;
			perIoData->buffer[transferredBytes] = 0;
			perIoData->dataBuff.buf = perIoData->buffer;
			strcat(perHandleData->inputMess + 4, perIoData->dataBuff.buf);
			ZeroMemory(perHandleData->outputMes1, sizeof(perHandleData->outputMes1));// reset data
			ZeroMemory(perHandleData->outputMes2, sizeof(perHandleData->outputMes2));// reset data
			preProcess(perHandleData->inputMess, perHandleData->client, perHandleData->clientWindowStt);
			revMessage(perHandleData->inputMess, perHandleData->outputMes1, perHandleData->length1, perHandleData->outputMes2, perHandleData->length2,rival,sendType);
			unsigned char tmp = perHandleData->outputMes1[0];
			updateClientStatus((int)tmp, perHandleData->outputMes2, perHandleData->length2, perHandleData->client, perHandleData->clientWindowStt,perHandleData->socket);
			perIoData->recvBytes = transferredBytes;
			perIoData->sentBytes = 0;
			perIoData->operation = SEND;
		}
		else if (perIoData->operation == SEND) {

			perIoData->sentBytes += transferredBytes;
			perIoData->recvBytes = 0;
		}

		if (perIoData->recvBytes > perIoData->sentBytes) {
			// Post another WSASend() request.
			// Since WSASend() is not guaranteed to send all of the bytes requested,
			// continue posting WSASend() calls until all received bytes are sent.
			ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
			
			perIoData->dataBuff.buf = perHandleData->outputMes1;
			perIoData->dataBuff.len = perHandleData->length1;
			perIoData->operation = SEND;
			cout << "Send: " << sendType << endl;
			
			if (sendType > 1&& sendType!=4){

				cout << rival << endl;
				printf("á2\n");
				for (int i = 0;i<4;i++)  printf("%d ", perHandleData->outputMes2[i]);
				printf("\n");
				if (rival == 0) {//the rival is offline
					if (send(perHandleData->socket, perHandleData->outputMes1, perHandleData->length1, 0) == SOCKET_ERROR) {
						printf("send() failed with error %d\n", WSAGetLastError());
					}
				}
				else if (send(rival, perHandleData->outputMes2, perHandleData->length2, 0) == SOCKET_ERROR) {
					printf("send() failed with error %d\n", WSAGetLastError());
				}
				if (sendType == 2) {
					perIoData->recvBytes = 0;
					perIoData->operation = RECEIVE;
					flags = 0;
					ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
					perIoData->dataBuff.len = 4;
					perIoData->dataBuff.buf = perIoData->buffer;
					if (WSARecv(perHandleData->socket,
						&(perIoData->dataBuff),
						1,
						&transferredBytes,
						&flags,
						&(perIoData->overlapped), NULL) == SOCKET_ERROR) {
						if (WSAGetLastError() != ERROR_IO_PENDING) {
							printf("WSARecv() failed with error %d\n", WSAGetLastError());
							return 0;
						}



					}

				}
			}
			if (sendType % 2 == 1) {
				for (int i = 0;i<4;i++)  printf("%d ", perIoData->dataBuff.buf[i]);
				printf("\n");
				if (WSASend(perHandleData->socket,
					&(perIoData->dataBuff),
					1,
					&transferredBytes,
					0,
					&(perIoData->overlapped),
					NULL) == SOCKET_ERROR) {
					if (WSAGetLastError() != ERROR_IO_PENDING) {
						printf("WSASend() failed with error %d\n", WSAGetLastError());
						return 0;
					}
				}
			}
			if (sendType == 4) {
				vector <USER> listActiveUser = getlistUsersActive();
				for (int i = 0; i < listActiveUser.size(); i++) {
					if (send(nickNameToSOCKET[listActiveUser.at(i).nickname], perHandleData->outputMes1, perHandleData->length1, 0) == SOCKET_ERROR) {
						printf("send() failed with error %d\n", WSAGetLastError());
					}
				}
				perIoData->recvBytes = 0;
				perIoData->operation = RECEIVE;
				flags = 0;
				ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
				perIoData->dataBuff.len = 4;
				perIoData->dataBuff.buf = perIoData->buffer;
				if (WSARecv(perHandleData->socket,
					&(perIoData->dataBuff),
					1,
					&transferredBytes,
					&flags,
					&(perIoData->overlapped), NULL) == SOCKET_ERROR) {
					if (WSAGetLastError() != ERROR_IO_PENDING) {
						printf("WSARecv() failed with error %d\n", WSAGetLastError());
						return 0;
					}
				}
			}
		}
		else {
			// No more bytes to send post another WSARecv() request
			perIoData->recvBytes = 0;
			perIoData->operation = RECEIVE;
			flags = 0;
			ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
			if (perHandleData->status == 0)
				perIoData->dataBuff.len = perHandleData->length;
			else {
				perIoData->dataBuff.len = 4;
			}
			perIoData->dataBuff.buf = perIoData->buffer;
			if (WSARecv(perHandleData->socket,
				&(perIoData->dataBuff),
				1,
				&transferredBytes,
				&flags,
				&(perIoData->overlapped), NULL) == SOCKET_ERROR) {
				if (WSAGetLastError() != ERROR_IO_PENDING) {
					printf("WSARecv() failed with error %d\n", WSAGetLastError());
					return 0;
				}



			}

		}
	}
}
unsigned __stdcall autoMoveThread(LPVOID variable)
{
	map <int, GAME>::iterator it;
	while (1) {
		for (it = gameNum.begin();it != gameNum.end();++it) {
			if (it->second.gameStart == 1) {
				auto now = Clock::now();
				int turn = it->second.gameBoard.nowTurn;
				auto sec = now- std::chrono::system_clock::from_time_t(it->second.lastMove[turn]);
				if (std::chrono::duration_cast<std::chrono::seconds>(sec).count() > 130) {
					int cell, dir = 0;
					int check = 0;
					for (int i = turn * 6 + 1;i <= turn * 6 + 5;i++) {
						if (it->second.gameBoard.board[i] != 0) {
							cell = i;break;
						}
					}
					if (cell == 0) cell = turn * 6 + 1;
						auto now = Clock::now();
						it->second.lastMove[(turn + 1) % 2] = Clock::to_time_t(now);
						it->second.gameBoard.moveRock(cell, dir);
						if (it->second.gameBoard.isEndGame() != 0) {
							if(it->second.gameBoard.suran==1){
								check = 2;
							}
							else {
								check = 1;
								if (it->second.gameBoard.playerWin == 0) {
									updateAfterGame(it->second.player[0], it->second.player[1]);
								}
								else if (it->second.gameBoard.playerWin == 1) {
									updateAfterGame(it->second.player[1], it->second.player[0]);
								}
								else;
								it->second.gameStart = -1;
								it->second.gameBoard.writeWin();
							}
						}
						char moveMes[50], sendBuff[100];
						char tmp[10];
						_itoa_s(it->first, moveMes, 10);
						strcat(moveMes, " ");
						_itoa_s(turn, tmp, 10);
						strcat(moveMes, tmp);strcat(moveMes, " ");
						_itoa_s(cell, tmp, 10);
						strcat(moveMes, tmp);strcat(moveMes, " ");
						_itoa_s(dir, tmp, 10);
						strcat(moveMes, tmp);
						int len = strlen(moveMes);
						makeItCombine(PLAYER_MOVE, moveMes, len, sendBuff);
						if (check != 2) {
							send(nickNameToSOCKET[it->second.player[0]], sendBuff, len + 4, 0);
							send(nickNameToSOCKET[it->second.player[1]], sendBuff, len + 4, 0);
						}
						if (check == 1) {
							char endMes[5];
							endMes[0] = char(END_GAME);
							endMes[1] = 0;
							endMes[2] = 0;
							endMes[3] = 0;
							send(nickNameToSOCKET[it->second.player[0]], endMes,  4, 0);
							send(nickNameToSOCKET[it->second.player[1]], endMes,  4, 0);
						}
					}
				}
			}
		}
	}
unsigned __stdcall cleanThread(LPVOID variable) {
	map <int, GAME>::iterator it;
	while (1) {
		for (it = gameNum.begin();it != gameNum.end();++it) {
			if (it->second.gameStart == 0) {
				auto now = Clock::now();
				int start = it->second.acceptTime;
				auto sec = now - std::chrono::system_clock::from_time_t(start);
				if (std::chrono::duration_cast<std::chrono::seconds>(sec).count() > 10) {
					char errMess[10];
					makeItCombine(ERROR_MESSAGE, START_ERROR, 3, errMess);
					send(nickNameToSOCKET[it->second.player[0]], errMess, 7, 0);
					send(nickNameToSOCKET[it->second.player[1]], errMess, 7, 0);
					int x = it->first;
					gameNum.erase(x);
				}
			}
			else if (it->second.gameStart == -1) {
				if (it->second.readyToDel[0] == true && it->second.readyToDel[1] == true) {
					deleteFile(to_string(it->first).c_str());
					int x = it->first;
					gameNum.erase(x);
				}
			}
		}
	}
}