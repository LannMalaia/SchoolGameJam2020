#pragma once
#include "packet.h"
#include "Tile.h"

struct _ClientInfo;

struct _RoomInfo
{
	// ���� ����
	ROOM_STATE m_State;
	char m_Msg[BUFSIZE]; // ��� �޽��� ����

	// �÷��̾� ����
	_ClientInfo* m_Users[2];

	// ���� ����
	long m_RoundTimer; // Ÿ�̸�
	Tile* m_Tiles[20]; // Ÿ�� ����Ʈ

	// �̺�Ʈ
	HANDLE e_Matched; // ���� ��Ī�� �ߵ��ϴ� �̺�Ʈ
	HANDLE e_GameStart; // ���� ���۽� �ߵ��ϴ� �̺�Ʈ
	HANDLE e_EndGame; // ���� �� �̺�Ʈ

	// ������
	HANDLE t_Timer; // ���� ���۽� ����� Ÿ�̸� ������

	_RoomInfo()
	{
		m_State = ROOM_WAIT;
		ZeroMemory(m_Msg, BUFSIZE);
	}

	// �濡 ������ �߰�, �� á�ų� �÷��� ���� ��� false ��ȯ
	bool Add_User(_ClientInfo* _data);
	// �濡�� ������ ����
	void Remove_User(_ClientInfo* _data);

	// �ش� �÷��̾��� Ÿ�� ���� ��Ȳ ȹ��
	USHORT Get_Player_Tiles(USHORT _player_position);

	// �ΰ��� �غ�
	void Ingame_Play_Ready(_ClientInfo* _data);
	// �ΰ��� ����
	void Ingame_Start();
	// �� �� Ÿ�Ͽ� ����, ������ ��� �������� Ǭ��
	void Ingame_Tile_Access(_ClientInfo* _data, POINT _coord);
};

_RoomInfo* AddRoomInfo();
_RoomInfo* SearchEmptyRoom(_ClientInfo* _client);
void RemoveRoomInfo(_RoomInfo* _ptr);

DWORD WINAPI ProcessTimer(LPVOID _ptr);
