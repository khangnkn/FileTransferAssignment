#include "ftp_client.h"
#include <afxsock.h>
#include <iostream>
#include <cstring>
#include <conio.h>
using namespace std;


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
			ftp_client Client;
			Client.UserHandler();
		}
	}
	else	//Error
	{
		cout << "Fatal Error: GetModuleHandle failed\n";
		nRetCode = 0;
	}
	return nRetCode;
}

