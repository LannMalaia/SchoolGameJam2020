#pragma once
#include "global.h"

struct _RoomInfo;

struct _ClientInfo
{
	// ����
	SOCKET sock;
	SOCKADDR_IN addr;

	// ��&���� ���� ����
	STATE m_State; // ���� ������
	char m_Buf[BUFSIZE]; // ����
	int m_BufSize; // ����ũ��
	_RoomInfo* m_CurrentRoom; // ���� �� ��ġ

	// ������
	wchar_t m_Name[30]; // �̸�
	UINT16 m_PlayerPosition; // �÷��̾� ������ (1, 2)
	UINT64 m_CharacterIndex; // ĳ���� �ε���
	
	// ����
	bool m_Loaded; // �ε���
	int m_QuizScore; // ���� ���ھ�
	int m_TileScore; // Ÿ�� ���ھ�
	USHORT m_Combo; // �޺�
	USHORT m_FreeTile; // ���� ��Ǯ�� Ÿ�� ����
	UINT64 m_TileMultiplier; // Ÿ�� ���, �� ���ÿ��� 10.0���� ������� ��
	POINT m_LastCoord; // �������� ������ ��ǥ

	void Initialize_Variables(); // �ΰ��� ���� �ʱ�ȭ
	UINT64 Get_TotalScore(); // ��Ż ���ھ� ���
};

_ClientInfo* AddClientInfo(SOCKET _sock, SOCKADDR_IN _addr);
void RemoveClientInfo(_ClientInfo* _ptr);
