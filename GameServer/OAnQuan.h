#pragma once
#include "stdafx.h"
#include<string>
#include<vector>
#include <iostream>
#include "processFile.h"
#include <time.h>
#define GIVE_UP -1
using namespace std;
class OAnQuan
{
private:
	
	int score[2]; // sore [0] is point of player0 and sore [1] is point of player1
	int numTurn = 0;
	char  filename[40]; // name file log
public:
	int suran=0;
	int board[12]; // board game
	int nowTurn; // 0 is turn of player0
	int playerWin = 0; //1 or 2
	int random(int minN, int maxN);//random in paragraph
	int randomPositon();// random move
	void changeTurn();
	int isEndGame();
	void setName(string name);
	void surrender(int loser);
	void nextCursor(int& currentCursor, int& currentLastpos);
	int checkMove(int position, int direction);
	void moveRock(int position, bool isRight);
	void writeWin() {
		if (playerWin != 2) {
			string dataWin = "W:" + to_string(playerWin) + "";
			writeFile(filename, dataWin);
		}
	}
	/*
	move rock
	input: param position is position in board
			isRight is direction( left or right)
*/


	void show() {
		printf("score: %d ==== %d ---------------------------------------Turn: %d\n", score[0], score[1], nowTurn);
		printf("      | 1 | 2 | 3 | 4 | 5 |    \n");
		printf("-------------------------------------------\n");
		printf("      | %d | %d | %d | %d | %d |   \n", board[1], board[2], board[3], board[4], board[5]);
		printf(" %d                               %d\n", board[0], board[6]);
		printf("      | %d | %d | %d | %d | %d |   \n", board[11], board[10], board[9], board[8], board[7]);
		printf("-------------------------------------------\n");
		printf("      | 11 | 10 | 9 | 8 | 7 |   \n");
		printf("-------------------------------------------\n");
	}
	OAnQuan();
	OAnQuan(string name);
	~OAnQuan();
};

