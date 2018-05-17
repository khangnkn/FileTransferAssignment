#pragma once
#include <afxsock.h>
void replylogcode(int code);
bool login(CSocket* _Socket);
bool ls(CSocket* _pSocket);
bool establishDataConnection(CSocket* _pSocket, bool mode);