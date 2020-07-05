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
	int board[12]; // board game
	int score[2]; // sore [0] is point of player0 and sore [1] is point of player1
	int nowTurn; // 0 is turn of player0
	int numTurn = 0;
	char  filename[40]; // name file log
public:
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
	/*
	move rock
	input: param position is position in board
			isRight is direction( left or right)
*/



	OAnQuan();
	OAnQuan(string name);
	~OAnQuan();
};

