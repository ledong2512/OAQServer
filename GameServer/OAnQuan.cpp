#include "stdafx.h"
#include "OAnQuan.h"




OAnQuan::OAnQuan(string name)
{
	strcpy(filename, name.c_str());
	for (int i = 0; i < 12; i++) {
		if (i == 0 || i == 6) board[i] = 10;
		else board[i] = 5;
	}
	//random first turn;
	nowTurn = random(0, 1);
	numTurn = 0;
	score[0] = 0; score[1] = 0;
}
OAnQuan::OAnQuan() {
	for (int i = 0; i < 12; i++) {
		if (i == 0 || i == 6) board[i] = 10;
		else board[i] = 5;
	}
	//random first turn;
	nowTurn = random(0, 1);
	numTurn = 0;
	score[0] = 0; score[1] = 0;

}
OAnQuan::~OAnQuan()
{
}

int OAnQuan::random(int minN, int maxN)
{
	srand((int)time(0));
	return minN + rand() % (maxN + 1 - minN);
}

int OAnQuan::randomPositon()
{
	return 0;
}

void OAnQuan::changeTurn()
{
	if (nowTurn == 0) {
		nowTurn = 1;
	}
	else nowTurn = 0;
}

int OAnQuan::isEndGame()
{
	if ((board[0] == 0 && board[6] == 0)) {
		for (int i = 1;i <= 5;i++) score[0] += board[i];
		for (int i = 7;i <= 11;i++) score[1] += board[i];
		if (score[0] > score[1]) playerWin = 0;
		else if (score[0] > score[1])
		{
			playerWin = 1;
		}
		else playerWin = 2;
		if (playerWin != 2) {
			string dataWin = "W:" + to_string(playerWin) + "";
			writeFile(filename, dataWin);
		}
		return 1;

	}
	else return 0;
}

void OAnQuan::setName(string name)
{
	strcpy(filename, name.c_str());
	ofstream file;
	file.open(filename);
	file << "";
	file.close();
}

void OAnQuan::surrender(int loser)
{
	playerWin = (loser + 1) % 2;
	string dataWin = "W:" + to_string(playerWin) + "";
	writeFile(filename, dataWin);
}

void OAnQuan::nextCursor(int & currentCursor, int & currentLastpos)
{
	// next
	int tmp = currentCursor;
	currentCursor += currentCursor - currentLastpos;
	currentLastpos = tmp;
	if (currentCursor == -1)
	{
		currentCursor = 11;
		currentLastpos = 12;
	}

	if (currentCursor == 12)
	{
		currentCursor = 0;
		currentLastpos = -1;
	}
}

int OAnQuan::checkMove(int position, int direction)
{
	if (direction == 0 || direction == 1) {
		if (nowTurn == 0) {
			if (position > 0 && position < 6 && board[position] != 0) { // Player 1's area is different non

				return 1;
			} return 0;
		}
		else {
			if (position > 6 && position < 12 && board[position] != 0) {// Player 2's area is different non
				return 1;
			}
			return 0;
		}
	}
	else
		return 0;
}

void OAnQuan::moveRock(int position, bool isRight)
{
	string data = "";
	if (nowTurn == 0 && board[1] + board[2] + board[3] + board[4] + board[5] == 0)
	{
		board[1] = 1;
		board[2] = 1;
		board[3] = 1;
		board[4] = 1;
		board[5] = 1;
		score[0] -= 5;
	}
	if (nowTurn == 1 && board[7] + board[8] + board[9] + board[10] + board[11] == 0)
	{
		board[7] = 1;
		board[8] = 1;
		board[9] = 1;
		board[10] = 1;
		board[11] = 1;
		score[1] -= 5;
	}
	data += "#" + to_string(nowTurn) + ":" + to_string(position) + ":" + to_string(isRight) + "+";
	writeFile(filename, data);
	if (checkMove(position, isRight)) {
		if (isRight) {
			int lastPos;
			if (nowTurn == 0)
			{
				lastPos = position + 1;
			}
			else
			{
				lastPos = position - 1;
			}

			while (board[position] != 0 && position != 0 && position != 6)
			{
				int onHand = board[position];
				board[position] = 0;
				while (onHand > 0)
				{
					nextCursor(position, lastPos);

					board[position] += 1;
					onHand -= 1;
				}
				nextCursor(position, lastPos);
			}while (board[position] == 0)
			{
				nextCursor(position, lastPos);


				if (board[position] == 0)
					break;
				else
				{
					score[nowTurn] += board[position];
					board[position] = 0;
					nextCursor(position, lastPos);
				}
			}
			changeTurn();

		}
		else if (!isRight)
		{
			int lastPos;
			if (nowTurn == 0)
			{
				lastPos = position - 1;
			}
			else
			{
				lastPos = position + 1;
			}


			while (board[position] != 0 && position != 0 && position != 6)
			{
				int onHand = board[position];
				board[position] = 0;
				while (onHand > 0)
				{
					nextCursor(position, lastPos);

					board[position] += 1;
					onHand -= 1;
				}
				nextCursor(position, lastPos);
			}while (board[position] == 0)
			{
				nextCursor(position, lastPos);


				if (board[position] == 0)
					break;
				else
				{

					score[nowTurn] += board[position];
					board[position] = 0;
					nextCursor(position, lastPos);
				}
			}
			changeTurn();

		}
	} numTurn++;
}
