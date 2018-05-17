#include "Header.h"
#include <afxsock.h>
#include <iostream>


using namespace std;

DWORD WINAPI CommandThread(LPVOID)
{
	CSocket* ClientSocket = new CSocket;
	login(ClientSocket);
	ls(ClientSocket);
	ClientSocket->Close();
	return 0;
}

DWORD WINAPI DataThread(LPVOID)
{
	CSocket* DataSocket = new CSocket;
	DataSocket->Create(52380, 1, (LPCTSTR)0);
	establishDataConnection(DataSocket, 1);

	char buf[BUFSIZ + 1];
	memset(buf, 0, sizeof buf);
	DataSocket->Receive(buf, BUFSIZ, 0);
	cout << buf;

	DataSocket->Close();
	return 0;
}

int main()
{
	int nRetCode = 0;
	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			if (AfxSocketInit() == FALSE)
			{
				cout << "Khong the khoi tao Socket Libraray";
				return FALSE;
			}
			//
			//Code from here
			DWORD dwCommandThread;
			DWORD dwDataThread;

			HANDLE hCommandThread;
			HANDLE hDataThread;

			hDataThread = CreateThread(NULL, 0, DataThread, NULL, 0, &dwDataThread);
			hCommandThread = CreateThread(NULL, 0, CommandThread, NULL, 0, &dwCommandThread);




			//Code above
		}
	}
	else	//Error
	{
		cout << "Fatal Error: GetModuleHandle failed\n";
		nRetCode = 0;
	}
	return nRetCode;
}

bool login(CSocket* _pSocket)
{
	_pSocket->Create();
	if (_pSocket->Connect(_T("127.0.0.1"), 21) != 0)
	{
		cout << "Ket noi toi Server thanh cong !!!\n\n";
	}
	else
		return FALSE;
	char buf[BUFSIZ + 1]; //buffer
	int tmpres;
	char * str;
	int codeftp;
	cout << "Connection established, waiting for welcome message...\n";
	memset(buf, 0, sizeof buf);

	while ((tmpres = _pSocket->Receive(buf, BUFSIZ, 0)) > 0)
	{
		sscanf(buf, "%d", &codeftp);
		printf("%s", buf);
		if (codeftp != 220) //120, 240, 421: something wrong
		{
			replylogcode(codeftp);
			return FALSE;
		}

		str = strstr(buf, "220");//DK dung
		if (str != NULL) {
			break;
		}
		memset(buf, 0, tmpres);
	}

	//Send user name
	char username[50];
	cout << "User name: ";
	memset(buf, 0, sizeof buf);
	cin >> username;
	sprintf(buf, "USER %s\r\n", username);
	tmpres = _pSocket->Send(buf, strlen(buf), 0);

	memset(buf, 0, sizeof buf);
	tmpres = _pSocket->Receive(buf, BUFSIZ, 0);

	sscanf(buf, "%d", &codeftp);
	if (codeftp != 331)
	{
		replylogcode(codeftp);
		return FALSE;
		//exit(1);
	}
	cout << buf;

	//Send password
	char password[50];

	cout << "Password: ";
	cin >> password;

	memset(buf, 0, sizeof buf);
	sprintf(buf, "PASS %s\r\n", password);
	tmpres = _pSocket->Send(buf, BUFSIZ, 0);

	memset(password, 0, sizeof password);
	tmpres = _pSocket->Receive(buf, BUFSIZ, 0);

	sscanf(buf, "%d", &codeftp);
	if (codeftp != 230)
	{
		replylogcode(codeftp);
		return FALSE;
		//exit(1);
	}
	cout << buf;
}
bool ls(CSocket* _pSocket)
{
	char buf[BUFSIZ + 1];
	int tmpres, codeftp;

	//Send PORT command
	memset(buf, 0, sizeof buf);
	sprintf(buf, "PORT %s\r\n", "127,0,0,1,205,105");
	_pSocket->Send(buf, strlen(buf), 0);
	//receive command
	memset(buf, 0, sizeof buf);
	_pSocket->Receive(buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeftp);
	if (codeftp != 200)
	{
		replylogcode(codeftp);
		return FALSE;
		//exit(1);
	}
	cout << buf;

	//Send NLTS command
	memset(buf, 0, sizeof buf);
	sprintf(buf, "NLST\r\n");
	_pSocket->Send(buf, strlen(buf), 0);
	memset(buf, 0, sizeof buf);
	_pSocket->Receive(buf, BUFSIZ, 0);
	cout << buf;
	sscanf(buf, "%d", &codeftp);
	if (codeftp != 150)
	{
		replylogcode(codeftp);
		return FALSE;
		//exit(1);
	}
	memset(buf, 0, sizeof buf);
	_pSocket->Receive(buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeftp);
	if (codeftp != 226)
	{
		replylogcode(codeftp);
		return FALSE;
		//exit(1);
	}
	cout << buf;
	return TRUE;
}

void replylogcode(int code)
{
	switch (code) {
	case 200:
		printf("Command okay");
		break;
	case 425:
		printf("425 Can't open data connection for transfer of /");
		break;
	case 500:
		printf("Syntax error, command unrecognized.");
		printf("This may include errors such as command line too long.");
		break;
	case 501:
		printf("Syntax error in parameters or arguments.");
		break;
	case 202:
		printf("Command not implemented, superfluous at this site.");
		break;
	case 502:
		printf("Command not implemented.");
		break;
	case 503:
		printf("Bad sequence of commands.");
		break;
	case 530:
		printf("Not logged in!");
		break;
	}
	printf("\n");
}
bool establishDataConnection(CSocket* _pSocket, bool mode)
{
	if (mode)
	{
		if (_pSocket->Listen(1) == FALSE)
		{
			cout << "Cannot listen on this Socket!\n";
			return FALSE;
		}
		_pSocket->Accept(*_pSocket);
		cout << "Connection established!\n";
		return TRUE;
	}
	else
	{
		return TRUE;
	}
}