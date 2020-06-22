#include "stdafx.h"
#include "check_format_regex.h"
#include<iostream>
#include<regex>
#include<stdio.h>
using namespace std;
int checkFormatEmail(std::string email)
{
	const regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
	return regex_match(email, pattern);
}
/*
�g^�h represents that starting character of the string.
�g[aA-zZ]�h makes sure that the starting character is in the lowercase or uppercase alphabet.
�g\\w{6, 29}�h represents a check to make sure that the remaining items are word items, which includes the underscore, until it reaches the end and that is represented with $.
The �g{5, 29}�h represents the 6-30 character constraint given to us minus the predefined first character.

*/
int checkFormatNickname(std::string nickname)
{
	const regex pattern("^[aA-zZ]\\w{6,29}$");
	return regex_match(nickname, pattern);
}

int checkFormatPassword(std::string nickname)
{
	const regex pattern("^[aA-zZ]\\w{5,20}$");
	return regex_match(nickname, pattern);
}
