#pragma once
#include "stdafx.h"
#include "mysql.h"
#include "iostream"
#include "string"
#include "algorithm"
#include "vector"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <chrono>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sha256.h"
#include"OAnQuan.h"
typedef std::chrono::system_clock Clock;

using namespace std;
typedef struct USER {
	int id;
	string email; 
	string password;
	string nickname;
	int point;
	time_t lastTime = 0;
} user_s;
typedef struct GAME {
	int number;
	string player[2];
	boolean ready[2] = { false,false };
	OAnQuan gameBoard; 
	int inUse = 0;
	int gameStart = 0;
	time_t lastMove[2];
	string ip[2];
	boolean readyToDel[2] = { false,false };
	time_t acceptTime;
};
/*
Function: connect database
Return: MYSQL
*/
MYSQL* connectionMysql();
/*
Function: get all account
Return: Account list in vector form
*/
vector <USER> getAllAccount();
/*
Function: search acount by nickname
Param: [IN] nickname: nickname of user
Return: struct USER
*/
USER searchUserByNickname(string nickname);
/*
Function: search acount by email
Param: [IN] email: mail of user
Return: struct USER
*/
USER searchUserByEmail(string email);
/*
Function: search acount by ID
Param: [IN] id: id of user
Return: struct USER
*/
USER searchUserById(int id);
/*
Function: Log into the system
Param: [IN] email: mail of user
Return: sucess returns the user who needs to login, fail return a user has id = -1.
*/
USER login(string email, string pass);
/*
Function: create new a account
Return: sucess returns 1, fail return  0.
*/
int regist(string email, string pass);
int regist(string email, string pass, string nickname);
/*
Function: logout
Return: sucess returns 1, fail return  0.
*/
int logout(int id);
/*
Function: update score of user
Param: id: [IN] id of user
	   point:[IN] point want to update
Return: sucess returns 1, fail return  0.
*/
int updatePoint(int id, int point);
/*
Function: update nickname of user
Param: id: [IN] id of user
nickname:[IN] nickname want to update
Return: sucess returns 1, fail return  0.
*/
int updateNickname(int id, string nickname);
/*
Function: update password of user
Return: sucess returns 1, fail return  0.
*/
int updatePass(string email, string nickname, string pass);
/*
Function: delete password of user
Return: sucess returns 1, fail return  0.
*/
int deleteAccount(int id);
/*
Function: update information when endgame
*/
void updateAfterGame(string winner, string loser);
void show(vector <USER> users);
/*
Function: update active of user
*/
void updateActive(string name, int code);
/*
Function: get all user is login
*/
vector <user_s> getlistUsersLogin();
/*
Function: get all user is active
*/
vector <user_s> getlistUsersActive();