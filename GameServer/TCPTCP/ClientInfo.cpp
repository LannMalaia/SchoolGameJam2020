#include "ClientInfo.h"
#include "RoomInfo.h"

_ClientInfo* ClientInfo[100];
int client_count;

// 클라 정보 추가하기
_ClientInfo* AddClientInfo(SOCKET _sock, SOCKADDR_IN _addr)
{
	Enter_CS(); // 치명적인 구역 좀 쓸게

	_ClientInfo* ptr = new _ClientInfo;
	ZeroMemory(ptr, sizeof(_ClientInfo));
	ptr->sock = _sock;
	memcpy(&ptr->addr, &_addr, sizeof(SOCKADDR_IN));

	ptr->m_State = STATE_INIT;
	ZeroMemory(ptr->m_Buf, BUFSIZE);
	ptr->m_BufSize = 0;
	ptr->m_CurrentRoom = nullptr;

	ptr->m_CharacterIndex = 1;

	ptr->m_Loaded = false; // 로딩
	
	ClientInfo[client_count++] = ptr;

	Leave_CS(); // 어 다 썼어

	printf("\n리스트에 클라이언트 추가 : IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(ptr->addr.sin_addr), ntohs(ptr->addr.sin_port));

	return ptr;
}
// 클라 정보 지우기
void RemoveClientInfo(_ClientInfo* _ptr)
{
	printf("리스트에서 클라이언트 제거 : IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(_ptr->addr.sin_addr), ntohs(_ptr->addr.sin_port));

	Enter_CS(); // 치명적인 구역 좀 쓸게

	for (int i = 0; i < client_count; i++)
	{
		if (ClientInfo[i] == _ptr)
		{
			// 방에 들어가 있다면 방에서 나가기
			if (ClientInfo[i]->m_CurrentRoom != nullptr)
				ClientInfo[i]->m_CurrentRoom->Remove_User(ClientInfo[i]);
			
			// 소켓 닫기
			closesocket(ClientInfo[i]->sock);

			// 정보 지우기
			delete ClientInfo[i];

			// 배열 땡기기
			for (int j = i; j < client_count - 1; j++)
				ClientInfo[j] = ClientInfo[j + 1];
			break;
		}
	}
	client_count--;

	Leave_CS(); // 어 다 썼어
}

// 게임에 쓰이는 변수들의 초기화
void _ClientInfo::Initialize_Variables()
{
	m_Loaded = false; // 로딩
	m_QuizScore = 0;
	m_TileScore = 0;
	m_Combo = 0;
	m_FreeTile = 0;
	m_TileMultiplier = 10; // 1.0
	m_LastCoord = { 0, 0 };
}

UINT64 _ClientInfo::Get_TotalScore()
{
	// [획득 점수] + [15 * 점유한 타일의 수 * 타일 배수]
	UINT64 total = m_QuizScore;
	total += 15 * 5 * m_TileMultiplier / 10;
	return total;
}
