#pragma once
#include "global.h"

struct _RoomInfo;

struct _ClientInfo
{
	// 소켓
	SOCKET sock;
	SOCKADDR_IN addr;

	// 방&상태 관련 정보
	STATE m_State; // 현재 페이즈
	char m_Buf[BUFSIZE]; // 버퍼
	int m_BufSize; // 버퍼크기
	_RoomInfo* m_CurrentRoom; // 현재 방 위치

	// 설정값
	wchar_t m_Name[30]; // 이름
	UINT16 m_PlayerPosition; // 플레이어 포지션 (1, 2)
	UINT64 m_CharacterIndex; // 캐릭터 인덱스
	
	// 변수
	bool m_Loaded; // 로딩됨
	int m_QuizScore; // 문제 스코어
	int m_TileScore; // 타일 스코어
	USHORT m_Combo; // 콤보
	USHORT m_FreeTile; // 문제 안풀고 타일 점령
	UINT64 m_TileMultiplier; // 타일 배수, 실 계산시에는 10.0으로 나눠줘야 함
	POINT m_LastCoord; // 마지막에 접근한 좌표

	void Initialize_Variables(); // 인게임 변수 초기화
	UINT64 Get_TotalScore(); // 토탈 스코어 취득
};

_ClientInfo* AddClientInfo(SOCKET _sock, SOCKADDR_IN _addr);
void RemoveClientInfo(_ClientInfo* _ptr);
