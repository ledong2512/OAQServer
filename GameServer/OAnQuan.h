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
	/*
	Function: Check the game end conditions
	*/
	void changeTurn(); //change turn among 2 player 0 and player 1
	/*
	Function: Check the game end conditions
	Return: 1 is end game
			0 is not end game
	*/
	int isEndGame(); 
	/*
	Function: set Name of file name
	Param: [IN] name:  to set
	*/
	void setName(string name);
	/*
	Function: When 1 player want to surrender
	Param: loser: id of player loser
	*/
	void surrender(int loser);
	/*
	Function: Cursor to each cell of the board
	*/
	void nextCursor(int& currentCursor, int& currentLastpos);
	/*
	Function: is the migration test valid
	Param: postion: [IN]position to select a piece
		   direction: [IN] direction want to move (left or right : 0,1)
	*/
	int checkMove(int position, int direction);
	/*
	Function: Move in the game
	Param: postion: [IN]position to select a piece
	isRight: [IN] direction want to move 
	*/
	void moveRock(int position, bool isRight);
	/*
	Function: write winner  to the file 
	*/
	void writeWin() {
		if (playerWin != 2) {
			string dataWin = "W:" + to_string(playerWin+1) + "";
			writeFile(filename, dataWin);
		}
	}



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

