#include "Header.h"
#include <afxsock.h>
#include <iostream>
#include <cstring>
#include <conio.h>



using namespace std;


DWORD WINAPI DataThread(LPVOID, CSocket* s)
{
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

			CSocket* ClientSocket = new CSocket;
			CSocket* DataSocket = new CSocket;
			
			login(ClientSocket);
			del(ClientSocket, "n.jpg");
			mdel(ClientSocket);

			ClientSocket->Close();
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
bool ls(CSocket* _pSocket, CSocket* _pDataSocket)
{
	char buf[BUFSIZ + 1];
	int tmpres, codeftp;

	//Find port of _pSocket
	CString SockAdrr;
	UINT SockPort;
	_pSocket->GetSockName(SockAdrr, SockPort);
	int x = SockPort / 255;
	int y = SockPort % 255 + 1;

	//Send PORT command
	char IP[BUFSIZ + 1];
	memset(IP, 0, sizeof IP);
	sprintf(IP, "127,0,0,1,%d,%d", x, y);
	memset(buf, 0, sizeof buf);
	sprintf(buf, "PORT %s\r\n", IP);
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
	
	//Listen on port SockPort + 1
	_pDataSocket->Create(SockPort + 1);
	if (_pDataSocket->Listen(1) == FALSE)
	{
		cout << "Khong the lang nghe tren port nay !!!" << endl;
		_pDataSocket->Close();
		return FALSE;
	}
	//Accept on port SockPort + 1

	//Send NLTS command
	memset(buf, 0, sizeof buf);
	sprintf(buf, "NLST\r\n");

	_pSocket->Send(buf, strlen(buf), 0);

	memset(buf, 0, sizeof buf);
	_pSocket->Receive(buf, BUFSIZ, 0);

	sscanf(buf, "%d", &codeftp);
	if (codeftp != 150)
	{
		replylogcode(codeftp);
		return FALSE;
		//exit(1);
	}
	cout << buf;

	CSocket tmpPort;
	_pDataSocket->Accept(tmpPort);
	memset(buf, 0, sizeof buf);
	tmpPort.Receive(buf, BUFSIZ, 0);
	cout << buf;


	sscanf(buf, "%d", &codeftp);
	if (codeftp != 226)
	{
		replylogcode(codeftp);
		return FALSE;
		//exit(1);
	}
	cout << buf;
	_pDataSocket->Close();
	return TRUE;
}

bool del(CSocket * _pSocket, char filename[STR_LENGTH])
{
	char buf[BUFSIZ + 1];
	int tmpres, ftpcode;

	//Chuan bi lenh de gui
	if (filename == NULL)
	{
		filename = new char[STR_LENGTH];
		memset(filename, 0, sizeof filename);
		cout << "Filename: ";
		cin >> filename;
	}

	//Gui lenh DELE <filename>\r\n
	memset(buf, 0, sizeof buf);
	sprintf(buf, "DELE %s\r\n", filename);
	SendCommand(_pSocket, buf);
	sscanf(buf, "%d", &ftpcode);
	cout << buf;
	if (ftpcode != 250)
	{
		replylogcode(ftpcode);
		return FALSE;
	}
	delete filename;
	return TRUE;
}
bool mdel(CSocket * _pSocket, char filenames[STR_LENGTH])
{
	char buf[BUFSIZ + 1];
	int tmpres, ftpcode;
	
	char* filename[5];

	if (filenames == NULL)
	{
		//char filenames[STR_LENGTH];
		filenames = new char[STR_LENGTH];
		cin.ignore();
		cout << "Filenames: ";
		cin.getline(filenames, 256);
	}

	filename[0] = strtok(filenames, " ,-");

	int i = 1;
	while (i < 5)
	{
		filename[i] = strtok(NULL, " ,-");
		
		if (filename[i] == NULL) break;
		i++;
	}
	
	for (int j = 0; j < i; j++)
	{
		char permission;
		cout << "Delete " << filename[j] << "? (Y/n): ";
		permission = _getch();
		if (permission == 'Y' || permission == 'y')
		{
			memset(buf, 0, BUFSIZE);
			sprintf(buf, "DELE %s\r\n", filename[j]);
			SendCommand(_pSocket, buf);
			cout << endl << buf;
			memset(buf, 0, BUFSIZE);
		}
		else
		{
			continue;
		}
	}
	delete filenames;
	return FALSE;
}
bool cd(CSocket * _pSocket)
{
	return TRUE;
}
