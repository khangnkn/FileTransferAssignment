#include "ftp_client.h"



ftp_client::ftp_client()
{
	_pSocket = new CSocket;
	memset(buf, 0, BUFSIZE);
	_pSocket->Create();
}

void ftp_client::UserHandler()
{
	char command[STR_LENGTH];
	this->Login(_T("127.0.0.1"));
	do
	{
		cout << "ftp> ";
		cin >> command;
		if (command == NULL) continue;
		int flag = this->CommandHandler(command);
		switch (flag)
		{
		case CODE_ERROR:
			cout << "Command invalid!\n";
			continue;
			break;
		case CODE_OPEN:
			this->_pSocket->Close();
			this->_pSocket->Create();
			this->Login();
			break;
		case CODE_CD:
			this->Cd();
			break;
		case CODE_DELETE:
			this->Del();
			break;
		case CODE_MDELETE:
			this->MDel();
			break;
		case CODE_MKDIR:
			this->Mkdir();
			break;
		case CODE_RMDIR:
			this->Rmdir();
			break;
		case CODE_PWD:
			this->Pwd();
			break;
		case CODE_QUIT:
			this->Quit();
			return;
		}
	} while (TRUE);
}

int ftp_client::CommandHandler(char command[])
{
	
	if (!strcmp(command, "open"))
		return CODE_OPEN;
	if (!strcmp(command, "cd"))
		return CODE_CD;
	if (!strcmp(command, "delete"))
		return CODE_DELETE;
	if (!strcmp(command, "mdelete"))
		return CODE_MDELETE;
	if (!strcmp(command, "mkdir"))
		return CODE_MKDIR;
	if (!strcmp(command, "rmdir"))
		return CODE_RMDIR;
	if (!strcmp(command, "pwd"))
		return CODE_PWD;
	if (!strcmp(command, "quit"))
		return CODE_QUIT;
	return CODE_ERROR;
}


bool ftp_client::Connect(wchar_t IPAddr[])
{
	if (_pSocket->Connect(IPAddr, PORT) != 0)
	{
		cout << "Ket noi den Server thanh cong!\n";
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool ftp_client::Login(wchar_t IPAddr[STR_LENGTH])
{
	if (IPAddr == NULL)
	{
		IPAddr = new wchar_t[STR_LENGTH];
		cout << "To: ";
		wcin >> IPAddr;
	}
	if (this->Connect(IPAddr) == FALSE)
	{
		return FALSE;
	}
	else
	{
		int tmpresult;
		int codeftp;
		char* str;
		memset(buf, 0, BUFSIZE);
		while ((tmpresult = _pSocket->Receive(buf, BUFSIZ, 0)) > 0)
		{
			sscanf(buf, "%d", &codeftp);
			//printf("%s", buf);
			cout << buf;
			if (codeftp != 220) //120, 240, 421: something wrong
			{
				//replylogcode(codeftp);
				return FALSE;
			}

			str = strstr(buf, "220");//DK dung
			if (str != NULL) {
				break;
			}
			memset(buf, 0, tmpresult);
		}

		//Send username
		char username[50];
		cout<<"User name: ";
		cin >> username;

		memset(buf, 0, BUFSIZE);
		sprintf(buf, "USER %s\r\n", username);
		if (this->SendCommand() == FALSE)
			return FALSE;

		if (!checkFTPCode(331)) return FALSE;
		
		//Send password
		char password[50];
		cout << "Password: ";
		cin >> password;

		memset(buf, 0, BUFSIZE);
		sprintf(buf, "PASS %s\r\n", password);
		if (this->SendCommand() == FALSE)
			return FALSE;
		if (!checkFTPCode(230)) return FALSE;
	}
	return TRUE;
	delete IPAddr;
}

bool ftp_client::Del(char filename[STR_LENGTH])
{
	if (filename == NULL)
	{
		filename = new char[STR_LENGTH];
		memset(filename, 0, sizeof filename);
		cout << "Filename: ";
		cin >> filename;
	}

	memset(buf, 0, BUFSIZE);
	sprintf(buf, "DELE %s\r\n", filename);
	if (this->SendCommand() == FALSE)
		return FALSE;
	if (!checkFTPCode(250)) return FALSE;
	delete filename;
	return TRUE;
}

bool ftp_client::MDel(char filenames[STR_LENGTH])
{
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
			cout << endl;
			if (this->SendCommand() == FALSE) continue;
			if (!checkFTPCode(250)) return FALSE;
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

bool ftp_client::Cd(char directory[STR_LENGTH])
{
	if (directory == NULL)
	{
		directory = new char[STR_LENGTH];
		cout << "Directory ";
		cin >> directory;
	}

	memset(buf, 0, BUFSIZE);
	sprintf(buf, "CWD %s\r\n", directory);
	if (this->SendCommand() == FALSE)
		return FALSE;
	if (!checkFTPCode(250)) return FALSE;
	return false;
}

bool ftp_client::Mkdir(char directory[STR_LENGTH])
{
	if (directory == NULL)
	{
		directory = new char[STR_LENGTH];
		cout << "Directory name ";
		cin >> directory;
	}

	memset(buf, 0, BUFSIZE);
	sprintf(buf, "MKD %s\r\n", directory);
	if (this->SendCommand() == FALSE)
		return FALSE;
	if (!checkFTPCode(257)) return FALSE;
	return TRUE;
}

bool ftp_client::Rmdir(char directory[STR_LENGTH])
{
	if (directory == NULL)
	{
		directory = new char[STR_LENGTH];
		cout << "Directory name ";
		cin >> directory;
	}

	memset(buf, 0, BUFSIZE);
	sprintf(buf, "RMD %s\r\n", directory);
	if (this->SendCommand() == FALSE)
		return FALSE;
	if (!checkFTPCode(250)) return FALSE;
	return TRUE;
}

bool ftp_client::Pwd()
{
	memset(buf, 0, BUFSIZE);
	sprintf(buf, "PWD\r\n");
	if (this->SendCommand() == FALSE)
		return FALSE;
	if (!checkFTPCode(257)) return FALSE;
	return TRUE;
}

bool ftp_client::Quit()
{
	sprintf(buf, "QUIT\r\n");
	if (this->SendCommand() == FALSE)
		return FALSE;
	return TRUE;
}

ftp_client::~ftp_client()
{
	_pSocket->Close();
	delete _pSocket;
}

bool ftp_client::SendCommand()
{
	if (_pSocket->Send(buf, BUFLEN, 0) == 0)
		return FALSE;
	memset(buf, 0, BUFSIZE);
	if (_pSocket->Receive(buf, BUFSIZ, 0) == 0)
		return FALSE;
	cout << buf;
	return TRUE;
}

bool ftp_client::checkFTPCode(int code)
{
	int codeftp;
	sscanf(buf, "%d", &codeftp);
	if (codeftp != code)
	{
		return FALSE;
	}
	return TRUE;
}
