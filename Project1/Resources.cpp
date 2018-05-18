#include "Header.h"
#include <iostream>

using namespace std;

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

void SendCommand(CSocket * _pSocket, char buf[BUFSIZ + 1])
{
	_pSocket->Send(buf, BUFLEN, 0);

	memset(buf, 0, BUFSIZE);
	_pSocket->Receive(buf, BUFSIZ, 0);
}