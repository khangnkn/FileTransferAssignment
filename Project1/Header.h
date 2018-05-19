#pragma once
#include <afxsock.h>

#define STR_LENGTH 256
#define BUFLEN strlen(buf)
#define BUFSIZE sizeof(buf)
#define PORT 21

void replylogcode(int code);
bool login(CSocket* _Socket);
bool ls(CSocket* _pSocket, CSocket* _pDataSocket);
bool del(CSocket * _pSocket, char filename[STR_LENGTH] = NULL);
bool mdel(CSocket * _pSocket, char filenames[STR_LENGTH] = NULL);
bool cd(CSocket * _pSocket, char dir[STR_LENGTH] = NULL);
bool lcd(char dir[STR_LENGTH] = NULL);
void SendCommand(CSocket * _pSocket, char buf[BUFSIZ + 1]);