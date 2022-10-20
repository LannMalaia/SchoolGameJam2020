#include "Process.h"

// 초기화
bool Process_Init(_ClientInfo* _client)
{
	// 이름 받기
	PacketRecv(_client->sock, _client->m_Buf, "Init Recv()");
	ZeroMemory(_client->m_Name, 30);
	UnPackPacket(_client->m_Buf + 8, &_client->m_CharacterIndex);

	// Debug
	cout << "클라이언트 선택 캐릭터 - " << _client->m_CharacterIndex << endl;

	// 인트로 보내기
	PacketSend(_client->sock, _client->m_Buf,
		PackPacket(_client->m_Buf, PROTOCOL::CONNECTED, L"연결 성공!\n매칭 대기중..."),
		0, "msg_intro send()");

	_client->m_CurrentRoom = SearchEmptyRoom(_client); // 빈 방의 주소를 찾기. 없으면 알아서 만들어진다
	_client->m_CurrentRoom->Add_User(_client); // 빈 방에 자기 자신을 넣어달라하기, 이 때 방이 꽉 차면 방의 상태가 ROOM_PLAY로 전환됨
	_client->m_State = STATE_WAITING; // 웨이팅 상태로

	/*
	// 받기 테스트
	wprintf(L"Receiving...\n");
	PacketRecv(_client->sock, _client->m_Buf, "Receiving Test");
	wprintf(L"Received.");
	GetProtocol(_client->m_Buf, protocol);
	wprintf(L"%d == %d\n", protocol, PROTOCOL::DEBUG);
	if (protocol == PROTOCOL::DEBUG)
	{
		wprintf(L"Start Unpacking...\n");
		wchar_t msg[4096];
		ZeroMemory(msg, 4096);
		UnPackPacket(_client->m_Buf + 8, msg);
		wprintf(L"%ws\n", msg);
	}
	*/

	return false;
}

// 타인 입장 기다리기
bool Process_Wait(_ClientInfo* _client)
{
	// 룸의 매칭 이벤트를 기다리기
	WaitForSingleObject(_client->m_CurrentRoom->e_Matched, 1000);
	try
	{
		PacketSend(_client->sock, _client->m_Buf,
			PackPacket(_client->m_Buf, PROTOCOL::DEBUG),
			0, "msg_heartbit send()");
		PacketRecv(_client->sock, _client->m_Buf, "msg_heartbit recv()");
	}
	catch (runtime_error & e)
	{
		_client->m_State = STATE_DISCONNECT;
		return true;
	}
	if (_client->m_State == STATE_DISCONNECT)
		return true;

	if (_client->m_CurrentRoom->m_State == ROOM_PLAY)
	{
		cout << "start" << endl;
		_client->m_State = STATE_GAME_START; // 상태 변경
	}
	else
		cout << "waiting..." << endl;

	return false;
}

// 게임 시작 겸 세팅
bool Process_GameStart(_ClientInfo* _client)
{
	cout << "send" << endl;

	// 상대의 캐릭터 인덱스 번호 취득
	UINT64 other_player_number = 0;
	for (int i = 0; i < 2; i++)
	{
		if (_client->m_CurrentRoom->m_Users[i] != _client)
			other_player_number = _client->m_CurrentRoom->m_Users[i]->m_CharacterIndex;
	}

	// 매칭됐다고 알려주고 상대의 캐릭터 넘버도 같이 보내기
	PacketSend(_client->sock, _client->m_Buf,
		PackPacket(_client->m_Buf, PROTOCOL::MATCHED, &_client->m_PlayerPosition, &other_player_number),
		0, "process_game_start send()");

	_client->Initialize_Variables(); // 클라의 인게임 자료 초기화

	// 인게임 로딩 끝났나 확인
	UINT64 protocol = PROTOCOL::DEBUG;
	while (protocol == PROTOCOL::DEBUG)
	{
		cout << "game start - wait ready" << endl;
		PacketRecv(_client->sock, _client->m_Buf, "process_game_start Recv");
		GetProtocol(_client->m_Buf, protocol);
		cout << "protocol - " << protocol << endl;

		if (!(protocol == PROTOCOL::READY || protocol == PROTOCOL::DEBUG))
			throw runtime_error("bad protocol");
	}

	_client->m_CurrentRoom->Ingame_Play_Ready(_client); // 방에게 나 레디됨 보내기

	// 룸의 게임 시작 이벤트를 기다리기
	WaitForSingleObject(_client->m_CurrentRoom->e_GameStart, INFINITE);

	// 게임이 시작됐으면 이제 3,2,1 땡하면 시작하라고 보내기
	PacketSend(_client->sock, _client->m_Buf,
		PackPacket(_client->m_Buf, PROTOCOL::GAME_START),
		0, "process_game_start send()");

	_client->m_State = STATE_PLAY; // 플레이 턴으로 상태 변경

	return false;
}

// 인게임
bool Process_Play(_ClientInfo* _client)
{
	UINT64 protocol;

	while (_client->m_State != STATE_DISCONNECT && _client->m_State != STATE_RESULT)
	{
		// 일단 프로토콜부터 받아야지
		PacketRecv(_client->sock, _client->m_Buf, "process_game_start Recv");
		GetProtocol(_client->m_Buf, protocol);

		UINT64 x = 0, y = 0;
		switch (protocol)
		{
		case PROTOCOL::TILE_ACCESSABLE: // 여기로 가고 싶은데
			UnPackPacket(_client->m_Buf + 8, &x, &y);
			_client->m_CurrentRoom->Ingame_Tile_Access(_client, { (LONG)x, (LONG)y });
			break;
		case PROTOCOL::END_GAME: // 끝난 거 확인했어
			_client->m_State = STATE_RESULT;
			break;
		}
	}

	_client->m_State = STATE_RESULT; // 결과 보러 가자고잉
	return false;
}


// 입력에 대한 결과 단계
bool Process_Result(_ClientInfo* _client)
{
	int size = 0, retval = 0;


	_client->m_State = STATE_END;
	return false;
}

// 끝
bool Process_End(_ClientInfo* _client)
{
	_client->m_State = STATE_DISCONNECT;
	return true;
}

// 클라이언트 스레드
DWORD WINAPI ProcessClient(LPVOID _ptr)
{
	_ClientInfo* _client = (_ClientInfo*)_ptr;
	UINT64 protocol;

	_wsetlocale(LC_ALL, L"korean");

	bool end_flag = false;
	int size = 0;

	while (1)
	{
		try
		{
			switch (_client->m_State)
			{
			case STATE_INIT: // 첫 접속 & 초기화
				Process_Init(_client);
				break;
			case STATE_WAITING: // 매칭 대기
				Process_Wait(_client);
				break;
			case STATE_GAME_START: // 게임 시작
				Process_GameStart(_client);
				break;
			case STATE_PLAY: // 게임중
				Process_Play(_client);
				break;
			case STATE_RESULT: // 결과
				Process_Result(_client);
				break;
			case STATE_END: // 끝
				Process_End(_client);
				break;
			case STATE_DISCONNECT: // 연결이 해제됨
				end_flag = true;
				break;
			}

			if (end_flag)
				break;

		}
		catch (runtime_error & e)
		{
			cout << e.what() << endl;
			_client->m_State = STATE_DISCONNECT;
		}
	}

	printf("클라이언트 연결 해제 : IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(_client->addr.sin_addr), ntohs(_client->addr.sin_port));
	RemoveClientInfo(_client);

	return 0;
}