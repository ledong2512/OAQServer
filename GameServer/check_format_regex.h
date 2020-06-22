#include <string>
int checkFormatEmail(std::string email);
/*
“^” represents that starting character of the string.
“[aA-zZ]” makes sure that the starting character is in the lowercase or uppercase alphabet.
“\\w{6, 29}” represents a check to make sure that the remaining items are word items, which includes the underscore, until it reaches the end and that is represented with $.
The “{6, 29}” represents the 6-30 character constraint given to us minus the predefined first character.

*/
int checkFormatNickname(std::string nickname);

int checkFormatPassword(std::string nickname);