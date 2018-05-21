#pragma once
#include <afxsock.h>
#include <iostream>
#include "Header.h"
using namespace std;

#define STR_LENGTH 256
#define BUFLEN strlen(buf)
#define BUFSIZE sizeof(buf)
#define PORT 21
#define IP _T("127.0.0.1")
#define	CODE_ERROR -1
#define CODE_OPEN 0
#define CODE_CD 7
#define CODE_DELETE 9
#define CODE_MDELETE 10
#define CODE_MKDIR 11
#define CODE_RMDIR 12
#define CODE_PWD 13
#define CODE_QUIT 15

class ftp_client
{
private:
	CSocket * _pSocket;
	char buf[BUFSIZ + 1];
public:
	ftp_client();
	void UserHandler();
	bool Connect(wchar_t IPAddr[] = NULL);
	bool Login(wchar_t IPAddr[STR_LENGTH] = NULL);
	bool Del(char filename[STR_LENGTH] = NULL);
	bool MDel(char filenames[STR_LENGTH] = NULL);
	bool Cd(char directory[STR_LENGTH] = NULL);
	bool Mkdir(char directory[STR_LENGTH] = NULL);
	bool Rmdir(char directory[STR_LENGTH] = NULL);
	bool Pwd();
	bool Quit();
	~ftp_client();
private:
	bool SendCommand();
	int CommandHandler(char command[]);
	bool checkFTPCode(int code);
};

