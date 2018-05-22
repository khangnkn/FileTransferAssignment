#include "ftp_client.h"



ftp_client::ftp_client()
{
	_pSocket = new CSocket;
	memset(buf, 0, BUFSIZE);
	_pSocket->Create();
	isPassive = 0;
	//dBuffer.open("dataBuffer.txt", ios::in | ios::out | ios::trunc | ios::binary);
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
		case CODE_LS:
			this->Ls();
			break;
		case CODE_CD:
			this->Cd();
			break;
		case CODE_DIR:
			this->Dir();
			break;
		case CODE_GET:
			this->Get();
			break;
		case CODE_PUT:
			this->Put();
			break;
		case CODE_MGET:
			this->MGet();
			break;
		case CODE_MPUT:
			this->MPut();
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
		case CODE_PASSIVE:
			this->Passive();
			break;
		case CODE_ACTIVE:
			this->Active();
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
	if (!strcmp(command, "ls"))
		return CODE_LS;
	if (!strcmp(command, "dir"))
		return CODE_DIR;
	if (!strcmp(command, "cd"))
		return CODE_CD;
	if (!strcmp(command, "get"))
		return CODE_GET;
	if (!strcmp(command, "put"))
		return CODE_PUT;
	if (!strcmp(command, "mget"))
		return CODE_MGET;
	if (!strcmp(command, "mput"))
		return CODE_MPUT;
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
	if (!strcmp(command, "passive"))
		return CODE_PASSIVE;
	if (!strcmp(command, "active"))
		return CODE_ACTIVE;
	if (!strcmp(command, "quit"))
		return CODE_QUIT;
	return CODE_ERROR;
}

bool ftp_client::Connect(wchar_t IPAddr[])
{
	if (_pSocket->Connect(IPAddr, PORT) != 0)
	{
		cout << "Successfully connect to FTP Server!\n";
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

bool ftp_client::Ls()
{
	if (dBuffer.is_open() == FALSE)
		dBuffer.open("dataBuffer.txt", ios::in | ios::out | ios::trunc | ios::binary);
	else
	{
		dBuffer.close();
		dBuffer.open("dataBuffer.txt", ios::in | ios::out | ios::trunc | ios::binary);
	}
	memset(buf, 0, BUFSIZE);
	sprintf(buf, "NLST\r\n");
	if (isPassive == 0)
	{
		if (!this->dataTvAM(0)) return FALSE;
	}
	else
	{
		if (!this->dataTvPM(0)) return FALSE;
	}

	dBuffer.clear();
	dBuffer.seekg(0, ios::beg);
	stringstream buffer;
	buffer << dBuffer.rdbuf();
	string contents(buffer.str());
	cout << contents;

	dBuffer.close();
	return TRUE;
}

bool ftp_client::Dir()
{
	if(dBuffer.is_open() == FALSE)
		dBuffer.open("dataBuffer.txt", ios::in | ios::out | ios::trunc | ios::binary);
	else
	{
		dBuffer.close();
		dBuffer.open("dataBuffer.txt", ios::in | ios::out | ios::trunc | ios::binary);
	}
	cout << "\nGet here!\n";
	memset(buf, 0, BUFSIZE);
	sprintf(buf, "LIST\r\n");
	if (isPassive == 0)
	{
		if (!this->dataTvAM(0)) return FALSE;
	}
	else
	{
		if (!this->dataTvPM(0)) return FALSE;
	}
	dBuffer.clear();
	dBuffer.seekg(0, ios::beg);
	stringstream buffer;
	buffer << dBuffer.rdbuf();
	string contents(buffer.str());
	cout << contents;

	dBuffer.close();
	return TRUE;
}

bool ftp_client::Put(char src_filename[STR_LENGTH], char dest_filename[STR_LENGTH])
{
	if (dBuffer.is_open() == FALSE)
		dBuffer.open("dataBuffer.txt", ios::in | ios::out | ios::trunc | ios::binary);
	else
	{
		dBuffer.close();
		dBuffer.open("dataBuffer.txt", ios::in | ios::out | ios::trunc | ios::binary);
	}
	if (src_filename == NULL)
	{
		src_filename = new char[STR_LENGTH];
		memset(src_filename, 0, sizeof src_filename);
		cout << "Source filename: ";
		cin >> src_filename;
	}
	if (dest_filename == NULL)
	{
		dest_filename = new char[STR_LENGTH];
		memset(dest_filename, 0, sizeof dest_filename);
		cout << "Filename: ";
		cin >> dest_filename;
	}

	fstream fout;
	fout.open(src_filename, ios::in | ios::binary);

	char buff[1024];
	memset(buff, 0, BUFSIZE);
	while (fout.peek() != EOF)
	{
		fout.read(buff, 1024);
		dBuffer.write(buff, 1024);
		memset(buff, 0, BUFSIZE);
	}
	cout << "Filesize " << dBuffer.tellg() << "bytes\n";
	memset(buf, 0, BUFSIZE);
	sprintf(buf, "STOR %s\r\n", dest_filename);
	if (isPassive == 0)
	{
		if (!this->dataTvAM(1)) return FALSE;
	}
	else
	{
		if (!this->dataTvPM(1)) return FALSE;
	}
	fout.close();
	dBuffer.close();
	return TRUE;
}

bool ftp_client::Get(char src_filename[STR_LENGTH], char dest_filename[STR_LENGTH])
{
	if (dBuffer.is_open() == FALSE)
		dBuffer.open("dataBuffer.txt", ios::in | ios::out | ios::trunc | ios::binary);
	else
	{
		dBuffer.close();
		dBuffer.open("dataBuffer.txt", ios::in | ios::out | ios::trunc | ios::binary);
	}
	if (src_filename == NULL)
	{
		src_filename = new char[STR_LENGTH];
		memset(src_filename, 0, sizeof src_filename);
		cout << "Source filename: ";
		cin >> src_filename;
	}
	if (dest_filename == NULL)
	{
		dest_filename = new char[STR_LENGTH];
		memset(dest_filename, 0, sizeof dest_filename);
		cout << "Filename: ";
		cin >> dest_filename;
	}
	memset(buf, 0, BUFSIZE);
	sprintf(buf, "RETR %s\r\n", src_filename);
	if (isPassive == 0)
	{
		if (!this->dataTvAM(0)) return FALSE;
	}
	else
	{
		if (!this->dataTvPM(0)) return FALSE;
	}
	fstream fin;
	fin.open(dest_filename, ios::out | ios::binary);
	dBuffer.clear();
	dBuffer.seekg(0, ios::beg);

	fin << dBuffer.rdbuf();
	fin.close();

	dBuffer.close();
	return TRUE;
}

bool ftp_client::MPut(char filenames[STR_LENGTH])
{
	char* filename[8];

	if (filenames == NULL)
	{
		filenames = new char[STR_LENGTH];
		cin.ignore();
		cout << "Filenames: ";
		cin.getline(filenames, 256);
	}

	filename[0] = strtok(filenames, " ,-");

	int i = 1;
	while (i < 8)
	{
		filename[i] = strtok(NULL, " ,-");
		if (filename[i] == NULL) break;
		i++;
	}

	for (int j = 0; j < i; j++)
	{
		char dst_filename[STR_LENGTH];
		cout << "Destination file for " << filename[j] << ": ";
		cin >> dst_filename;
		this->Put(filename[j], dst_filename);
	}
	delete filenames;	
	return FALSE;
}

bool ftp_client::MGet(char filenames[STR_LENGTH])
{
	char* filename[8];

	if (filenames == NULL)
	{
		filenames = new char[STR_LENGTH];
		cin.ignore();
		cout << "Filenames: ";
		cin.getline(filenames, 256);
	}

	filename[0] = strtok(filenames, " ,-");

	int i = 1;
	while (i < 8)
	{
		filename[i] = strtok(NULL, " ,-");
		if (filename[i] == NULL) break;
		i++;
	}

	for (int j = 0; j < i; j++)
	{
		char dst_filename[STR_LENGTH];
		cout << "Destination file for " << filename[j] << ": ";
		cin >> dst_filename;
		this->Get(filename[j], dst_filename);
	}
	delete filenames;
	return FALSE;
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
	char* filename[8];

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
	while (i < 8)
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

void ftp_client::Passive()
{
	this->isPassive = 1;
}

void ftp_client::Active()
{
	this->isPassive = 0;
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

bool ftp_client::dataTvAM(bool stream)
{
	CSocket dtSocket, tmpSock;
	CString dtIPAddr; UINT dtPort;
	
	char t_buf[BUFSIZ + 1];
	sprintf(t_buf, "%s", buf);

	if (dtSocket.Create(0U, SOCK_STREAM, _T("127.0.0.1")) == 0) return FALSE;
	if (dtSocket.Listen(5) == 0) return FALSE;

	if (dtSocket.GetSockName(dtIPAddr, dtPort) == 0) return FALSE;
	int x = dtPort / 256;
	int y = dtPort % 256;
	memset(buf, 0, BUFSIZE);
	sprintf(buf, "PORT 127,0,0,1,%d,%d\r\n", x, y);
	if (!this->SendCommand()) return FALSE;
	if (!checkFTPCode(200)) return FALSE;

	memset(buf, 0, BUFSIZE);
	sprintf(buf, "%s", t_buf);

	if (!this->SendCommand()) return FALSE;
	if (!checkFTPCode(150)) return FALSE;

	memset(buf, 0, BUFSIZE);
	if (dtSocket.Accept(tmpSock) == 0) return FALSE;

	if (stream == 0)
	{
		int buf_count;
		buf_count = tmpSock.Receive(buf, BUFSIZ, 0);
		while (buf_count != 0)
		{
			dBuffer.write(buf, buf_count);
			memset(buf, 0, BUFSIZE);
			buf_count = tmpSock.Receive(buf, BUFSIZ, 0);
		}
	}
	else
	{
		int buf_count;
		memset(buf, 0, BUFSIZE);
		do
		{
			dBuffer.read(buf, BUFSIZ);
			buf_count = tmpSock.Send(buf, BUFSIZ, 0);
			memset(buf, 0, BUFSIZE);
		} while (buf_count <= 0 && buf_count != SOCKET_ERROR);
	}

	tmpSock.Close();
	dtSocket.Close();

	memset(buf, 0, BUFSIZE);
	if (this->_pSocket->Receive(buf, BUFSIZ, 0) == 0) return FALSE;
	if (!checkFTPCode(226)) return FALSE;

	cout << buf;
	return TRUE;
}

bool ftp_client::dataTvPM(bool stream)
{
	CSocket dtSocket;
	int IPAddr[4];
	int Port[2];

	char t_buf[BUFSIZ + 1];
	sprintf(t_buf, "%s", buf);

	sprintf(buf, "PASV\r\n");
	if (!this->SendCommand()) return FALSE;
	if (!checkFTPCode(227)) return FALSE;

	sscanf(buf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n", &IPAddr[0], &IPAddr[1], &IPAddr[2], &IPAddr[3], &Port[0], &Port[1]);

	int nPort = Port[0] * 256 + Port[1];

	if (dtSocket.Create() == 0) return FALSE;
	if (dtSocket.Connect(_T("127.0.0.1"), nPort) == 0) return FALSE;

	memset(buf, 0, BUFSIZE);
	sprintf(buf, "%s", t_buf);
	if (!this->SendCommand()) return FALSE;
	//if (!checkFTPCode(150)) return FALSE;
	if (stream == 0)
	{
		int buf_count;
		buf_count = dtSocket.Receive(buf, BUFSIZ, 0);
		while (buf_count != 0)
		{
			dBuffer.write(buf, buf_count);
			memset(buf, 0, BUFSIZE);
			buf_count = dtSocket.Receive(buf, BUFSIZ, 0);
		}
	}
	else
	{
		int buf_count;
		memset(buf, 0, BUFSIZE);
		do
		{
			dBuffer.read(buf, BUFSIZ);
			buf_count = dtSocket.Send(buf, BUFSIZ, 0);
			memset(buf, 0, BUFSIZE);
		} while (buf_count <= 0 && buf_count != SOCKET_ERROR);
	}
	dtSocket.Close();

	memset(buf, 0, BUFSIZE);
	if (!this->_pSocket->Receive(buf, BUFSIZ, 0)) return FALSE;
	if (!checkFTPCode(226)) return FALSE;
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
