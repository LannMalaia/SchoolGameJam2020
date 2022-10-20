#pragma once
#include "packet.h"
#include "Tile.h"

struct _ClientInfo;

struct _RoomInfo
{
	// 서버 관련
	ROOM_STATE m_State;
	char m_Msg[BUFSIZE]; // 방송 메시지 버퍼

	// 플레이어 관련
	_ClientInfo* m_Users[2];

	// 저장 변수
	long m_RoundTimer; // 타이머
	Tile* m_Tiles[20]; // 타일 리스트

	// 이벤트
	HANDLE e_Matched; // 게임 매칭시 발동하는 이벤트
	HANDLE e_GameStart; // 게임 시작시 발동하는 이벤트
	HANDLE e_EndGame; // 게임 끝 이벤트

	// 스레드
	HANDLE t_Timer; // 게임 시작시 생기는 타이머 스레드

	_RoomInfo()
	{
		m_State = ROOM_WAIT;
		ZeroMemory(m_Msg, BUFSIZE);
	}

	// 방에 유저를 추가, 꽉 찼거나 플레이 중인 경우 false 반환
	bool Add_User(_ClientInfo* _data);
	// 방에서 유저를 제거
	void Remove_User(_ClientInfo* _data);

	// 해당 플레이어의 타일 점유 상황 획득
	USHORT Get_Player_Tiles(USHORT _player_position);

	// 인게임 준비
	void Ingame_Play_Ready(_ClientInfo* _data);
	// 인게임 시작
	void Ingame_Start();
	// 룸 내 타일에 접근, 가능한 경우 문제까지 푼다
	void Ingame_Tile_Access(_ClientInfo* _data, POINT _coord);
};

_RoomInfo* AddRoomInfo();
_RoomInfo* SearchEmptyRoom(_ClientInfo* _client);
void RemoveRoomInfo(_RoomInfo* _ptr);

DWORD WINAPI ProcessTimer(LPVOID _ptr);
