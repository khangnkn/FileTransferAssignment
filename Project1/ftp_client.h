#pragma once
#include <afxsock.h>
#include <iostream>
#include <conio.h>
#include <fstream>
#include <sstream>
#include <direct.h>
using namespace std;

#define STR_LENGTH 256
#define BUFLEN strlen(buf)
#define BUFSIZE sizeof(buf)
#define PORT 21
#define IP _T("127.0.0.1")
#define DOWNSTREAM 0
#define UPSTREAM  1
#define	CODE_ERROR -1
#define CODE_OPEN 0
#define CODE_LS 21
#define CODE_DIR 22
#define CODE_PUT 3
#define CODE_GET 4
#define CODE_MPUT 5
#define CODE_MGET 6
#define CODE_CD 7
#define CODE_LCD 8
#define CODE_DELETE 9
#define CODE_MDELETE 10
#define CODE_MKDIR 11
#define CODE_RMDIR 12
#define CODE_PWD 13
#define CODE_PASSIVE 14
#define CODE_ACTIVE 142
#define CODE_QUIT 15

class ftp_client
{
private:
	CSocket * _pSocket;
	char buf[BUFSIZ + 1];
	bool isPassive;
	fstream dBuffer;

public:
	ftp_client();
	void UserHandler();
	bool Connect(wchar_t IPAddr[] = NULL);
	bool Login(wchar_t IPAddr[STR_LENGTH] = NULL);
	bool Ls();
	bool Dir();
	bool Put(char src_filename[STR_LENGTH] = NULL, char dest_filename[STR_LENGTH] = NULL);
	bool Get(char src_filename[STR_LENGTH] = NULL, char dest_filename[STR_LENGTH] = NULL);
	bool MPut(char filenames[STR_LENGTH] = NULL);
	bool MGet(char filenames[STR_LENGTH] = NULL);
	bool Del(char filename[STR_LENGTH] = NULL);
	bool MDel(char filenames[STR_LENGTH] = NULL);
	bool Cd(char directory[STR_LENGTH] = NULL);
	bool Lcd(char directory[STR_LENGTH] = NULL);
	bool Mkdir(char directory[STR_LENGTH] = NULL);
	bool Rmdir(char directory[STR_LENGTH] = NULL);
	bool Pwd();
	void Passive();
	void Active();
	bool Quit();
	~ftp_client();
private:
	bool SendCommand();
	bool dataTvAM(bool stream);
	bool dataTvPM(bool stream);
	int CommandHandler(char command[]);
	bool checkFTPCode(int code);
};

