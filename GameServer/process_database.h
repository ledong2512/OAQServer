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
#include "sha256.h"



using namespace std;
typedef struct USER {
	int id;
	string email;
	string password;
	string nickname;
	int point;
	
} user_s;

MYSQL* connectionMysql();
vector <USER> getAllAccount();

USER searchUserByNickname(string nickname);
USER searchUserByEmail(string email);
USER searchUserById(int id);

USER login(string email, string pass);
int regist(string email, string pass);
int regist(string email, string pass, string nickname);
int logout(int id);
int updatePoint(int id, int point);
int updateNickname(int id, string nickname);
int updatePass(string email, string nickname, string pass);
int deleteAccount(int id);
void show(vector <USER> users);
vector <user_s> getlistUsersLogin();
vector <user_s> getlistUsersActive();