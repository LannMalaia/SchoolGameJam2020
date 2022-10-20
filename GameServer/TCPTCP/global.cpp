#include "global.h"
#include "Packet.h"

CRITICAL_SECTION cs;

#pragma region NETWORK UTILITY

int recvn(SOCKET sock, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(sock, ptr, left, flags);

		if (received == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		else if (received == 0)
		{
			break;
		}

		left -= received;
		ptr += received;
	}

	return (len - left);
}
void err_quit(const char* msg)
{
	LPVOID lpmsgbuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpmsgbuf, 0, NULL);

	MessageBox(NULL, (LPCSTR)lpmsgbuf,
		msg, MB_ICONERROR);
	LocalFree(lpmsgbuf);
	exit(-1);
}
void err_display(const char* msg)
{
	LPVOID lpmsgbuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpmsgbuf, 0, NULL);

	printf("[%s]%s\n", msg, (LPCSTR)lpmsgbuf);
	LocalFree(lpmsgbuf);
}

void PacketSend(SOCKET _sock, const char* _buf, int _size, int _flags, const char* _err_msg)
{
	int retval = send(_sock, _buf, _size, _flags);
	if (retval == SOCKET_ERROR)
		throw runtime_error(_err_msg);
}

bool PacketRecv(SOCKET _sock, char* _buf, const char* _err_msg)
{
	int size;
	int retval = recvn(_sock, (char*)&size, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
		throw runtime_error(_err_msg);
	else if (retval == 0)
		throw runtime_error(_err_msg);

	cout << "packet size = " << retval << endl;

	retval = recvn(_sock, _buf, size, 0);
	if (retval == SOCKET_ERROR)
		throw runtime_error(_err_msg);
	else if (retval == 0)
		return false;

	return true;
}
PROTOCOL GetProtocol(const char* _buf)
{
	PROTOCOL protocol;
	memcpy(&protocol, _buf, sizeof(PROTOCOL));

	return protocol;
}

void Initialize_CS()
{
	InitializeCriticalSection(&cs);
}
void Delete_CS()
{
	DeleteCriticalSection(&cs);
}
void Enter_CS()
{
	EnterCriticalSection(&cs);
}
void Leave_CS()
{
	LeaveCriticalSection(&cs);
}

#pragma endregion