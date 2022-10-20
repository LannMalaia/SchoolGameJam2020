#pragma once
#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <time.h>

using namespace std;

#define SERVERPORT 10000
#define BUFSIZE 4096

enum PROTOCOL : UINT64
{
	// ETC
	DEBUG = 1,
	NOTICE,

	// TITLE
	CONNECTED, // �����
	MATCHED, // ��ġ �����
	READY, // ������ �� �غ� �Ϸ��

	// INGAME
	GAME_START, // ���� üũ �� ���� ����
	TIMER, // Ÿ�̸� ����
	END_GAME, // ���� ��
	TILE_ACCESSABLE, // Ŭ��: ���� ���� ����? ����
	TILE_UNACCEPTABLE, // ����: �� �ȵ�
	TILE_MARK, // ����: �� ���⿡ ��ũ ����
	TILE_MARK_ERASE, // ���� : ��ũ �����
	TILE_MOVE, // ����: �� ����� ������
	TILE_CAPTURE, // ����: �� ���� ������
	TILE_QUESTION, // ����: ���� ������
	ANSWER, // Ŭ��: ���� �ٰ�
	SCORE, // x�� �÷��̾��� ���� ����
	SKILL, // ��ų
	SURRENDER, // ��ų

	// DISCONNECT
	ESCAPED, // �� ������ �ȵ�
	DISCONNECT // �� ������
};

enum STATE : int
{
	STATE_INIT = 1,				// ���ӽ� �ʱ�ȭ
	STATE_WAITING,				// ��Ī ���
	STATE_GAME_START,		// ���� ����
	STATE_PLAY,					// ���� ��
	STATE_RESULT,				// ���� ���
	STATE_END,					// ���� ����
	STATE_DISCONNECT		// ����
};

enum ROOM_STATE : int
{
	ROOM_WAIT = 1, // ��ٸ��� �ܰ�
	ROOM_PLAY, // ������
	ROOM_END // ���� ��
};

enum DIFFICULTY : int
{
	EASY = 1,
	NORMAL,
	HARD
};

#pragma region NETWORK MSG



#pragma endregion

#pragma region NETWORK UTILITY

int recvn(SOCKET sock, char* buf, int len, int flags);
void err_quit(const char* msg);
void err_display(const char* msg);

void PacketSend(SOCKET _sock, const char* _buf, int _size, int _flags, const char* _err_msg);
bool PacketRecv(SOCKET _sock, char* _buf, const char* _err_msg);
PROTOCOL GetProtocol(const char* _buf);

void Initialize_CS();
void Delete_CS();
void Enter_CS();
void Leave_CS();

#pragma endregion
