#include "ClientInfo.h"
#include "RoomInfo.h"

_ClientInfo* ClientInfo[100];
int client_count;

// Ŭ�� ���� �߰��ϱ�
_ClientInfo* AddClientInfo(SOCKET _sock, SOCKADDR_IN _addr)
{
	Enter_CS(); // ġ������ ���� �� ����

	_ClientInfo* ptr = new _ClientInfo;
	ZeroMemory(ptr, sizeof(_ClientInfo));
	ptr->sock = _sock;
	memcpy(&ptr->addr, &_addr, sizeof(SOCKADDR_IN));

	ptr->m_State = STATE_INIT;
	ZeroMemory(ptr->m_Buf, BUFSIZE);
	ptr->m_BufSize = 0;
	ptr->m_CurrentRoom = nullptr;

	ptr->m_CharacterIndex = 1;

	ptr->m_Loaded = false; // �ε�
	
	ClientInfo[client_count++] = ptr;

	Leave_CS(); // �� �� ���

	printf("\n����Ʈ�� Ŭ���̾�Ʈ �߰� : IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(ptr->addr.sin_addr), ntohs(ptr->addr.sin_port));

	return ptr;
}
// Ŭ�� ���� �����
void RemoveClientInfo(_ClientInfo* _ptr)
{
	printf("����Ʈ���� Ŭ���̾�Ʈ ���� : IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(_ptr->addr.sin_addr), ntohs(_ptr->addr.sin_port));

	Enter_CS(); // ġ������ ���� �� ����

	for (int i = 0; i < client_count; i++)
	{
		if (ClientInfo[i] == _ptr)
		{
			// �濡 �� �ִٸ� �濡�� ������
			if (ClientInfo[i]->m_CurrentRoom != nullptr)
				ClientInfo[i]->m_CurrentRoom->Remove_User(ClientInfo[i]);
			
			// ���� �ݱ�
			closesocket(ClientInfo[i]->sock);

			// ���� �����
			delete ClientInfo[i];

			// �迭 �����
			for (int j = i; j < client_count - 1; j++)
				ClientInfo[j] = ClientInfo[j + 1];
			break;
		}
	}
	client_count--;

	Leave_CS(); // �� �� ���
}

// ���ӿ� ���̴� �������� �ʱ�ȭ
void _ClientInfo::Initialize_Variables()
{
	m_Loaded = false; // �ε�
	m_QuizScore = 0;
	m_TileScore = 0;
	m_Combo = 0;
	m_FreeTile = 0;
	m_TileMultiplier = 10; // 1.0
	m_LastCoord = { 0, 0 };
}

UINT64 _ClientInfo::Get_TotalScore()
{
	// [ȹ�� ����] + [15 * ������ Ÿ���� �� * Ÿ�� ���]
	UINT64 total = m_QuizScore;
	total += 15 * 5 * m_TileMultiplier / 10;
	return total;
}
