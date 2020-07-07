// OAnQuan.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "mysql.h"
#include "iostream"
#include "string"
#include "algorithm"
#include "vector"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <string>
#include "sha256.h"
#include "process_database.h"
/* ----||  Final Config ||----*/
const char * HOST_DB = "localhost";
const char * USERNAME_DB = "root";
const char * PASSWORD_DB = "root";
const char * TABLE_NAME_DB = "o_an_quan";
const int PORT_DB = 3306;
/* ----||  ------------ ||----*/
extern map <string, SOCKET> nickNameToSOCKET;
using namespace std;
vector <USER> listLoginUser;
vector <USER> listActiveUser;
MYSQL* connectionMysql() {
	MYSQL* conn;
	conn = mysql_init(0);
	conn = mysql_real_connect(conn, HOST_DB, USERNAME_DB, PASSWORD_DB, TABLE_NAME_DB, PORT_DB, NULL, 0);
	return conn;
}

vector <USER> getAllAccount() {
	vector <USER> users;
	MYSQL* conn = connectionMysql();
	MYSQL_ROW row;
	MYSQL_RES *res;
	if (conn) {
		string query = "SELECT *FROM account;";
		const char* q = query.c_str();
		if (!mysql_query(conn, q)) {
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res))
			{
 
				USER user;
				if (row[0] != NULL)
					user.id = atoi(row[0]);
				if (row[4] != NULL)
					user.point = atoi(row[4]);
				if (row[1] != NULL)
					user.email = row[1];
				if (row[2] != NULL)
					user.password = row[2];
				if (row[3] != NULL)
					user.nickname = row[3];
				users.push_back(user);
			}

		}
	}
	mysql_close(conn);
	return users;
}

USER searchUserByNickname(string nickname) {
	MYSQL* conn = connectionMysql();
	MYSQL_ROW row;
	MYSQL_RES *res;
	USER user;
	user.id = -1;
	if (conn) {
		string query = "SELECT * from o_an_quan.account WHERE nickname ='" + nickname + "';";
		const char* q = query.c_str();
		if (!mysql_query(conn, q)) {
			res = mysql_store_result(conn);
			row = mysql_fetch_row(res);
			if (row) {
				if (row[0] != NULL)
					user.id = atoi(row[0]);
				if (row[1] != NULL)
					user.email = row[1];
				if (row[2] != NULL)
					user.password = row[2];
				if (row[3] != NULL)
					user.nickname = row[3];
				if (row[4] != NULL)
					user.point = atoi(row[4]);
			}
		}
		mysql_close(conn);
	}
	return user;
}
USER searchUserByEmail(string email) {
	MYSQL* conn = connectionMysql();
	MYSQL_ROW row;
	MYSQL_RES *res;
	USER user;
	user.id = -1;
	if (conn) {
		string query = "SELECT * from o_an_quan.account WHERE email ='" + email + "';";
		const char* q = query.c_str();
		if (!mysql_query(conn, q)) {
			res = mysql_store_result(conn);
			row = mysql_fetch_row(res);
			if (row) {
				if (row[0] != NULL)
					user.id = atoi(row[0]);
				if (row[1] != NULL)
					user.email = row[1];
				if (row[2] != NULL)
					user.password = row[2];
				if (row[3] != NULL)
					user.nickname = row[3];
				if (row[4] != NULL)
					user.point = atoi(row[4]);
			}
		}
		mysql_close(conn);
	}
	return user;
}
USER searchUserById(int id) {
	MYSQL* conn = connectionMysql();
	MYSQL_ROW row;
	MYSQL_RES *res;
	USER user;
	user.id = -1;
	if (conn) {
		string query = "SELECT * from o_an_quan.account WHERE id =" + to_string(id) + ";";
		const char* q = query.c_str();
		if (!mysql_query(conn, q)) {
			res = mysql_store_result(conn);
			row = mysql_fetch_row(res);
			if (row) {
				if (row[0] != NULL)
					user.id = atoi(row[0]);
				if (row[1] != NULL)
					user.email = row[1];
				if (row[2] != NULL)
					user.password = row[2];
				if (row[3] != NULL)
					user.nickname = row[3];
				if (row[4] != NULL)
					user.point = atoi(row[4]);
			}
		}
		mysql_close(conn);
	}
	return user;
}

USER login(string email, string pass) {
	MYSQL* conn = connectionMysql();
	MYSQL_ROW row;
	MYSQL_RES *res;
	USER user;
	user.id = -1;
	for (int i = 0; i < listLoginUser.size(); i++) {
		if (email.compare(listLoginUser.at(i).email) == 0) { user.id = -2;return user; }
	}
	if (conn) {
		string query = "SELECT * from o_an_quan.account WHERE email ='" + email + "' and password ='" + sha256(pass) + "';";
		const char* q = query.c_str();
		if (!mysql_query(conn, q)) {
			res = mysql_store_result(conn);
			row = mysql_fetch_row(res);
			if (row) {
				if (row[0] != NULL)
					user.id = atoi(row[0]);
				if (row[1] != NULL)
					user.email = row[1];
				if (row[2] != NULL)
					user.password = row[2];
				if (row[3] != NULL)
					user.nickname = row[3];
				if (row[4] != NULL)
					user.point = atoi(row[4]);
				listLoginUser.push_back(user);
				listActiveUser.push_back(user);
			}
			

		}
		else {
			cout << mysql_error(conn) << endl;
		}
		mysql_close(conn);
	}
	return user;
}

int regist(string email, string pass) {
	USER u = searchUserByEmail(email);
	if (u.id >= 0) return 0;
	MYSQL * conn = connectionMysql();
	if (conn) {
		string query = "INSERT INTO account(email,password,nickname) VALUES ('" + email + "','" + sha256(pass) + "','" + email + "');";
		int query_state = mysql_query(conn, query.c_str());// thuc hien truy van
		if (query_state == 0) {
			mysql_close(conn);
			return 1;
		}
		else {
			cout << mysql_error(conn) << endl;
			mysql_close(conn);
			return 0;
		}
	}

}
int regist(string email, string pass, string nickname) {
	USER u = searchUserByEmail(email);
	if (u.id >= 0) return -1;
	 u = searchUserByNickname(nickname);
	if (u.id >= 0) return -2;
	MYSQL * conn = connectionMysql();
	if (conn) {
		string query = "INSERT INTO account(email,password,nickname) VALUES ('" + email + "','" + sha256(pass) + "','" + nickname + "');";

		int query_state = mysql_query(conn, query.c_str());// thuc hien truy van
		if (query_state == 0) {
			mysql_close(conn);
			return 1;
		}
		else {
			cout << mysql_error(conn) << endl;
			mysql_close(conn);
			return 0;
		}
	}

}
int logout(int id) {
	for (int i = 0; i < listActiveUser.size(); i++) {
		if (id == listActiveUser.at(i).id) {
			listActiveUser.erase(listActiveUser.begin() + i);
		}
	}
		for (int i = 0; i < listLoginUser.size(); i++) {
			if (id == listLoginUser.at(i).id) {
				nickNameToSOCKET.erase(listLoginUser.at(i).nickname);
				listLoginUser.erase(listLoginUser.begin() + i);
				return 1;
			}
		}
		return 0; //khong tim thay id
	
}
int updatePoint(int id, int point) {
	MYSQL * conn = connectionMysql();
	if (conn) {
		string query = "UPDATE `account` SET `point` = " + to_string(point) + " WHERE `id` = " + to_string(id) + ";";
		int query_state = mysql_query(conn, query.c_str());// thuc hien truy van
		if (query_state == 0) {
			mysql_close(conn);
			return 1;
		}
		else {
			cout << mysql_error(conn) << endl;
			mysql_close(conn);
			return 0;
		}
	}
}
int updateNickname(int id, string nickname) {
	MYSQL * conn = connectionMysql();
	if (conn) {
		string query = "UPDATE `account` SET `nickname` = '" + nickname + "' WHERE `id` = " + to_string(id) + ";";
		int query_state = mysql_query(conn, query.c_str());// thuc hien truy van
		if (query_state == 0) {
			mysql_close(conn);
			return 1;
		}
		else {
			cout << mysql_error(conn) << endl;
			mysql_close(conn);
			return 0;
		}
	}
}

int updatePass(string email, string nickname, string pass) {
	MYSQL * conn = connectionMysql();
	if (conn) {
		string query = "UPDATE `account` SET `password` = '" + sha256(pass) + "' WHERE `email` = '" + email + "' and nickname = '" + nickname + "';";

		int query_state = mysql_query(conn, query.c_str());// thuc hien truy van
		if (query_state == 0) {
			mysql_close(conn);
			return 1;
		}
		else {
			cout << mysql_error(conn) << endl;
			mysql_close(conn);
			return 0;
		}
	}
	return 0;
}
int deleteAccount(int id) {
	MYSQL * conn = connectionMysql();
	if (conn) {
		string query = "DELETE FROM `o_an_quan`.`account` WHERE `id` = " + to_string(id);

		int query_state = mysql_query(conn, query.c_str());// thuc hien truy van
		if (query_state == 0) {
			mysql_close(conn);
			return 1;
		}
		else {
			cout << mysql_error(conn) << endl;
			mysql_close(conn);
			return 0;
		}
	}
	return 0;
}
void updateAfterGame(string winner, string loser)
{
	int winnerPoint, loserPoint,winId,loseId;
	USER winU = searchUserByNickname(winner);
	USER loseU = searchUserByNickname(loser);
	if (loseU.point - 3 < 0)loseU.point = 3;
	updatePoint(winU.id, winU.point + 3);
	updatePoint(loseU.id, loseU.point - 3);
}
void show(vector <USER> users) {
	for (int i = 0; i < users.size(); i++) {
		printf("id: %d, email: %s,  nickname: %s, point: %d\n",
			users.at(i).id, users.at(i).email, users.at(i).nickname, users.at(i).point);

	}
}

void updateActive(string name,int code)
{
	USER user = searchUserByNickname(name);
	if (code == 1) {
		listActiveUser.push_back(user);
	}
	else {
		for (int i = 0; i < listActiveUser.size(); i++) {
			if (name == listActiveUser.at(i).nickname) {
				listActiveUser.erase(listActiveUser.begin() + i);
			}
		}
	}
}

vector <user_s> getlistUsersLogin() {
	return listLoginUser;
}
vector <user_s> getlistUsersActive() {
	return listActiveUser;
}