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
	CONNECTED, // 연결됨
	MATCHED, // 매치 성사됨
	READY, // 게임을 할 준비 완료됨

	// INGAME
	GAME_START, // 레디 체크 후 게임 시작
	TIMER, // 타이머 갱신
	END_GAME, // 게임 끝
	TILE_ACCESSABLE, // 클라: 여기 접근 가능? 묻기
	TILE_UNACCEPTABLE, // 서버: 응 안돼
	TILE_MARK, // 서버: 얘 여기에 마크 박음
	TILE_MARK_ERASE, // 서버 : 마크 지우셈
	TILE_MOVE, // 서버: 얘 여기로 움직임
	TILE_CAPTURE, // 서버: 얘 여기 정복함
	TILE_QUESTION, // 서버: 여기 문제야
	ANSWER, // 클라: 답을 줄게
	SCORE, // x번 플레이어의 현재 점수
	SKILL, // 스킬
	SURRENDER, // 스킬

	// DISCONNECT
	ESCAPED, // 얘 접속이 안됨
	DISCONNECT // 얘 끊어짐
};

enum STATE : int
{
	STATE_INIT = 1,				// 접속시 초기화
	STATE_WAITING,				// 매칭 대기
	STATE_GAME_START,		// 게임 시작
	STATE_PLAY,					// 게임 중
	STATE_RESULT,				// 게임 결과
	STATE_END,					// 게임 종료
	STATE_DISCONNECT		// 끊김
};

enum ROOM_STATE : int
{
	ROOM_WAIT = 1, // 기다리는 단계
	ROOM_PLAY, // 게임중
	ROOM_END // 게임 끝
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
