#include "RoomInfo.h"
#include "ClientInfo.h"

_RoomInfo* RoomInfo[50];
int room_count = 0;

// 방 추가하기
_RoomInfo* AddRoomInfo()
{
	Enter_CS(); // 배열 수정해야 하니까 치명적인 구역 좀 써야함
	_RoomInfo* ptr = new _RoomInfo;
	ZeroMemory(ptr, sizeof(_RoomInfo));

	// 서버 관련 변수
	ptr->m_State = ROOM_WAIT;
	ZeroMemory(ptr->m_Msg, BUFSIZE);

	// 플레이어 관련 변수
	ZeroMemory(ptr->m_Users, sizeof(_ClientInfo*) * 2);

	// 저장 변수
	ptr->m_RoundTimer = 0;
	for (int i = 0; i < 20; i++)
		ptr->m_Tiles[i] = new Tile(i);

	// 이벤트
	ptr->e_Matched = CreateEvent(NULL, TRUE, FALSE, NULL);
	ptr->e_GameStart = CreateEvent(NULL, TRUE, FALSE, NULL);
	ptr->e_EndGame = CreateEvent(NULL, TRUE, FALSE, NULL);

	RoomInfo[room_count++] = ptr;

	printf("\n리스트에 룸 추가\n");

	Leave_CS(); // 응 다 썼어
	return ptr;
}

// 방 정보 지우기
void RemoveRoomInfo(_RoomInfo* _ptr)
{
	printf("\n리스트에서 룸 제거\n");

	Enter_CS(); // 배열 조작해야하니까 치명적인 영역 좀 써야함
	for (int i = 0; i < room_count; i++)
	{
		if (RoomInfo[i] == _ptr) // 해당하는 방 찾았다면
		{
			// 이벤트 닫기
			CloseHandle(_ptr->e_Matched);
			CloseHandle(_ptr->e_GameStart);
			CloseHandle(_ptr->e_EndGame);

			// 타일 지우기
			for (int i = 0; i < 20; i++)
				delete _ptr->m_Tiles[i];

			// 정보 지우기
			delete RoomInfo[i];

			// 배열 땡기기
			for (int j = i; j < room_count - 1; j++)
				RoomInfo[j] = RoomInfo[j + 1];

			break; // 더 볼 거 없으니까 반복문 탈출
		}
	}
	room_count--;
	Leave_CS(); // 응 다 썼어
}

// 빈 방 찾기
_RoomInfo* SearchEmptyRoom(_ClientInfo* _client)
{
	for (int i = 0; i < room_count; i++) // 현재 등록된 방 중에서
	{
		if (RoomInfo[i]->m_State == ROOM_WAIT) // 그냥 비어 있는 방이 있고
		{
			for (int j = 0; j < 2; j++) // 클라 저장소 뒤져봐서
			{
				if (RoomInfo[i]->m_Users[j] != nullptr)
				{
					if (RoomInfo[i]->m_Users[j] == _client) // 자기 자신이 있는 경우
						break; // 이 방은 아니여

					if (RoomInfo[i]->m_Users[j]->m_CharacterIndex == _client->m_CharacterIndex) // 캐릭터 번호가 같은 경우
						break; // 이 방은 아니여
				}
				if (RoomInfo[i]->m_Users[j] == nullptr) // 거기에 아직 빈 자리 있으면
					return RoomInfo[i]; // 이 방을 주선해준다
			}
		}
	}

	// 아무 방도 없거나 방이 다 꽉 차있거나 한 경우 여기까지 온다
	return AddRoomInfo(); // 새로 방 만들어서 그거 주선해준다
}

// 방에 유저 추가하기
bool _RoomInfo::Add_User(_ClientInfo* _data)
{
	for (int i = 0; i < 2; i++) // 빈 자리 찾는 반복문
	{
		if (m_Users[i] == nullptr) // 빈 자리 발견
		{
			m_Users[i] = _data; // 그 자리를 지금 온 클라의 자리로 한다

			if (i == 1) // 모든 인원이 모였다면
			{
				// 기다리고 있던 애 접속상태 확인
				try
				{
					PacketSend(m_Users[0]->sock, m_Users[0]->m_Buf,
						PackPacket(m_Users[0]->m_Buf, PROTOCOL::DEBUG), 0,
						"매칭할 사람이 못 버티고 나갔음");
					PacketRecv(m_Users[0]->sock, m_Users[0]->m_Buf, "매칭할 사람이 못 버티고 나갔음");
				}
				catch (runtime_error & e) // 못 기다리고 나간 경우
				{
					cout << e.what() << endl;
					Remove_User(m_Users[0]);
					return true;
				}
				m_Users[0]->m_PlayerPosition = 1;
				m_Users[1]->m_PlayerPosition = 2;
				cout << "룸이 플레이 상태로 전환됨" << endl;
				cout << "플레이어 1 포트 : " << ntohs(m_Users[0]->addr.sin_port) << endl;
				cout << "플레이어 2 포트 : " << ntohs(m_Users[1]->addr.sin_port) << endl;

				this->m_State = ROOM_PLAY;	// 룸 상태 전환

				SetEvent(e_Matched);		// 게임 시작 이벤트 발동
			}
			return true;
		}
	}
	// 정말 만에 하나 못 찾았으면 false
	return false;
}

// 방에서 유저 제거
void _RoomInfo::Remove_User(_ClientInfo* _data)
{
	_data->m_State = STATE_DISCONNECT;

	if (m_State == ROOM_PLAY)
	{
		for (int i = 0; i < 2; i++)
		{
			if (m_Users[i] != _data)
			{
				try
				{
					PacketSend(m_Users[i]->sock, m_Users[i]->m_Buf,
						PackPacket(m_Users[i]->m_Buf, PROTOCOL::SURRENDER),
						0, "surrender send()");
				}
				catch (runtime_error & e)
				{
					m_Users[i]->m_State = STATE_DISCONNECT;
				}
			}
		}
	}

	// 지우기
	for (int i = 0; i < 2; i++)
	{
		if (m_Users[i] == _data)
		{
			// 배열 땡기기
			for (int j = i; j < 1; j++)
			{
				m_Users[j] = m_Users[j + 1];
				m_Users[j + 1] = nullptr;
			}
			break;
		}
	}

	// 만약 게임 진행 중
	if (m_State == ROOM_PLAY)
	{
		if(m_Users[0] == nullptr || m_Users[1] == nullptr)
			m_State = ROOM_END;
	}

	// 사람 기다리는 중도 아닌데 방에 아무도 없는 경우
	if (m_Users[0] == nullptr && m_State != ROOM_WAIT)
		RemoveRoomInfo(this); // 스스로를 제거
}

USHORT _RoomInfo::Get_Player_Tiles(USHORT _player_position)
{
	USHORT tile_count = 4;

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (Map[j][i] >= 0 && Map[j][i] < 20)
			{
				Tile* tile = m_Tiles[Map[j][i]];
				tile_count += tile->m_Owner == _player_position ? 1 : 0;
			}
			
		}
	}

	return tile_count;
}

// 클라이언트들의 인게임 플레이 준비
void _RoomInfo::Ingame_Play_Ready(_ClientInfo* _data)
{
	int loaded = 0;
	for (int i = 0; i < 2; i++)
	{
		if (m_Users[i] == _data)
		{
			_data->m_Loaded = true;
			loaded++;
		}
		else if (m_Users[i]->m_Loaded)
			loaded++;
	}

	if (loaded >= 2)
		Ingame_Start();
}

// 게임 시작
void _RoomInfo::Ingame_Start()
{
	// 인게임 데이터 초기화
	m_RoundTimer = 60000; // 3분

	// 시작 처리
	SetEvent(e_GameStart);

	// 타이머 스레드 생성
	t_Timer = CreateThread(NULL, 0, ProcessTimer, this, 0, NULL);
	if (t_Timer == NULL) // 스레드 못 만들겄슈
		throw runtime_error("Cannot Create Thread");
	else
		CloseHandle(t_Timer); // 야 나 메인스레드인데 이제 클라이언트 스레드에 접근 안할게
}

// 유저의 타일 액세스
void _RoomInfo::Ingame_Tile_Access(_ClientInfo* _data, POINT _coord)
{
	Tile* tile = m_Tiles[Map[_coord.y][_coord.x]];
	int client_id = _data->m_PlayerPosition;

	cout << "맵 좌표 - " << _coord.x << ", " << _coord.y << " : " << Map[_coord.y][_coord.x] << endl;

	if (Map[_coord.y][_coord.x] > 99 && Map[_coord.y][_coord.x] / 100 == client_id) // 플레이어 전용 구획에 접근하는 경우
	{
		cout << "전용 지역 접근" << endl;
		UINT64 x = _coord.x;
		UINT64 y = _coord.y;
		// 바로 움직임
		for (int i = 0; i < 2; i++)
		{
			try
			{
				PacketSend(m_Users[i]->sock, m_Users[i]->m_Buf,
					PackPacket(m_Users[i]->m_Buf, PROTOCOL::TILE_MOVE, &_data->m_PlayerPosition, &x, &y), 0,
					"Ingame_Tile_Access - Tile move");
			}
			catch (runtime_error & e)
			{
				m_Users[i]->m_State = STATE_DISCONNECT;
			}
		}
		return;
	}
	else if (Map[_coord.y][_coord.x] < 0 || Map[_coord.y][_coord.x] > 99)
	{
		cout << "통행 불가지역 접근" << endl;
		// 불가능
		PacketSend(_data->sock, _data->m_Buf,
			PackPacket(_data->m_Buf, PROTOCOL::TILE_UNACCEPTABLE), 0,
			"Ingame_Tile_Access - Unacceptable");
		return;
	}

	// 이미 소유한 타일에 접근하는지, 타일을 누가 사용중인지 체크
	if (tile->m_Owner == client_id) // 자신이 이미 타일을 점유한 경우
	{
		cout << "이미 점령한 지역 접근" << endl;
		UINT64 x = _coord.x;
		UINT64 y = _coord.y;
		// 바로 움직임
		for (int i = 0; i < 2; i++)
		{
			try
			{
				PacketSend(m_Users[i]->sock, m_Users[i]->m_Buf,
					PackPacket(m_Users[i]->m_Buf, PROTOCOL::TILE_MOVE, &_data->m_PlayerPosition, &x, &y), 0,
					"Ingame_Tile_Access - Tile move");
			}
			catch (runtime_error & e)
			{
				m_Users[i]->m_State = STATE_DISCONNECT;
			}
		}
		return;
	}
	else if (tile->m_Using) // 타인이 해당 타일 사용중인 경우
	{
		cout << "현재 타인이 사용중인 타일" << endl;
		// 불가능
		PacketSend(_data->sock, _data->m_Buf,
			PackPacket(_data->m_Buf, PROTOCOL::TILE_UNACCEPTABLE), 0,
			"Ingame_Tile_Access - Unacceptable");
		return;
	}

	// 소유자 자신 아님 + 아무도 사용중이지 않은 경우 여기까지 올 수 있음
	
	// 타일 사용중 표시
	Enter_CS();
	tile->m_Using = true;
	Leave_CS();

	_data->m_LastCoord = _coord;

	DIFFICULTY diff = m_RoundTimer > 120000 ? DIFFICULTY::HARD : m_RoundTimer > 60000 ? DIFFICULTY::NORMAL : DIFFICULTY::EASY;
	Quiz* quiz = Quiz::Get_Quiz(diff); // 퀴즈 취득
	USHORT answer = -1;

	if (_data->m_FreeTile <= 0)
	{
		// 양측에 마킹했다 알려주기
		for (int i = 0; i < 2; i++)
		{
			cout << "마킹 전송" << endl;
			UINT64 x = _coord.x;
			UINT64 y = _coord.y;
			try
			{
				PacketSend(m_Users[i]->sock, m_Users[i]->m_Buf,
					PackPacket(m_Users[i]->m_Buf, PROTOCOL::TILE_MARK, &_data->m_PlayerPosition, &x, &y),
					0, "marking send()");
			}
			catch (runtime_error & e)
			{
				m_Users[i]->m_State = STATE_DISCONNECT;
			}
		}
		// 문제 풀기 시작, 어려움 -> 보통 -> 쉬움 1분 간격

		cout << "문제 전송" << endl;
		// 문제 보내기
		PacketSend(_data->sock, _data->m_Buf,
			PackPacket(_data->m_Buf, PROTOCOL::TILE_QUESTION, quiz->m_Question, quiz->m_Answer_1, quiz->m_Answer_2, quiz->m_Answer_3, quiz->m_Answer_4),
			0, "question send()");

		cout << "답안 기다리기" << endl;
		PacketRecv(_data->sock, _data->m_Buf, "answer recv()");
		UINT64 protocol;
		GetProtocol(_data->m_Buf, protocol);
		if (protocol != PROTOCOL::ANSWER) // 답안을 안주면 끊긴것으로 간주
			throw runtime_error("answer recv - bad protocol");
		UnPackPacket(_data->m_Buf + 8, &answer);
	}
	// 정답을 맞춘 경우
	if (answer == quiz->answer_index || _data->m_FreeTile > 0) 
	{
		cout << "정답" << endl;
		if (_data->m_FreeTile <= 0)
		{
			_data->m_Combo += 1;
			_data->m_QuizScore += 10 * (int)diff;
		}
		else
			_data->m_FreeTile -= 1;

		UINT16 skill = 0;

		switch (_data->m_Combo)
		{
		case 3:
			skill = 1;
			for (int i = 0; i < 2; i++)
			{
				try
				{
					PacketSend(m_Users[i]->sock, m_Users[i]->m_Buf,
						PackPacket(m_Users[i]->m_Buf, PROTOCOL::SKILL, &_data->m_PlayerPosition, &skill), 0,
						"Ingame_Tile_Access - SKILL");
				}
				catch (runtime_error & e)
				{
					m_Users[i]->m_State = STATE_DISCONNECT;
				}
			}
			_data->m_TileMultiplier += 1;
			break;
		case 6:
			skill = 2;
			for (int i = 0; i < 2; i++)
			{
				try
				{
					PacketSend(m_Users[i]->sock, m_Users[i]->m_Buf,
						PackPacket(m_Users[i]->m_Buf, PROTOCOL::SKILL, &_data->m_PlayerPosition, &skill), 0,
						"Ingame_Tile_Access - SKILL");
				}
				catch (runtime_error & e)
				{
					m_Users[i]->m_State = STATE_DISCONNECT;
				}
			}
			_data->m_TileMultiplier += 1;
			break;
		case 9:
			skill = 3;
			for (int i = 0; i < 2; i++)
			{
				try
				{
					PacketSend(m_Users[i]->sock, m_Users[i]->m_Buf,
						PackPacket(m_Users[i]->m_Buf, PROTOCOL::SKILL, &_data->m_PlayerPosition, &skill), 0,
						"Ingame_Tile_Access - SKILL");
				}
				catch (runtime_error & e)
				{
					m_Users[i]->m_State = STATE_DISCONNECT;
				}
			}
			if (_data->m_CharacterIndex == 1)
				_data->m_FreeTile = 3;
			_data->m_Combo = 0;
			break;
		}

		tile->m_Owner = _data->m_PlayerPosition;
		USHORT tile_index = Map[_coord.y][_coord.x];
		for (int i = 0; i < 2; i++)
		{
			UINT64 x = _coord.x;
			UINT64 y = _coord.y;
			try
			{
				PacketSend(m_Users[i]->sock, m_Users[i]->m_Buf,
					PackPacket(m_Users[i]->m_Buf, PROTOCOL::TILE_MOVE, &_data->m_PlayerPosition, &x, &y), 0,
					"Ingame_Tile_Access - Tile move");
				PacketSend(m_Users[i]->sock, m_Users[i]->m_Buf,
					PackPacket(m_Users[i]->m_Buf, PROTOCOL::TILE_CAPTURE, &tile_index, &_data->m_PlayerPosition), 0,
					"Ingame_Tile_Access - Tile capture");


			}
			catch (runtime_error & e)
			{
				m_Users[i]->m_State = STATE_DISCONNECT;
			}
		}
	}
	else // 아닌 경우
	{
		cout << "오답" << endl;
		_data->m_Combo = 0;
		for (int i = 0; i < 2; i++)
		{
			try
			{
				PacketSend(m_Users[i]->sock, m_Users[i]->m_Buf,
					PackPacket(m_Users[i]->m_Buf, PROTOCOL::TILE_MARK_ERASE, &_data->m_PlayerPosition), 0,
					"Ingame_Tile_Access - Tile erase");
			}
			catch (runtime_error & e)
			{
				m_Users[i]->m_State = STATE_DISCONNECT;
			}
		}
	}

	cout << "스코어 전송" << endl;
	// 양측에 현재 문제를 푼 사람의 스코어 보내주기
	for (int i = 0; i < 2; i++)
	{
		USHORT is_me = (USHORT)(i + 1 == client_id ? 1 : 0);
		UINT64 quiz_score = _data->m_QuizScore;
		UINT64 total = _data->Get_TotalScore();
		USHORT tiles = Get_Player_Tiles(_data->m_PlayerPosition);

		try
		{
			PacketSend(m_Users[i]->sock, m_Users[i]->m_Buf,
				PackPacket(m_Users[i]->m_Buf, PROTOCOL::SCORE, &_data->m_PlayerPosition, &quiz_score, &tiles, &_data->m_Combo, &total, &_data->m_TileMultiplier),
				0, "score send()");
		}
		catch (runtime_error & e)
		{
			m_Users[i]->m_State = STATE_DISCONNECT;
		}
	}

	// 문제 풀기 종료
	delete quiz;
	tile->m_Using = false;

	// 

	return;
}

#define TIMER_MS 20 // 갱신 시간초
#define TIMER_SEND_INTERVAL 1000 // 타이머 보내는 주기

// 룸 타이머 스레드
DWORD WINAPI ProcessTimer(LPVOID _ptr)
{
	_RoomInfo* _room = (_RoomInfo*)_ptr;

	int timer_stack = 0; // 타이머 스택, 1000이 되면 전송

	Sleep(3000);

	cout << "timer - room state = " << (_room != nullptr) << ", " << _room->m_State << endl;

	// 플레이중이지 않으면 굳이 할 필요 없음
	while (_room != nullptr && _room->m_State == ROOM_PLAY)
	{
		_room->m_RoundTimer -= TIMER_MS;
		timer_stack += TIMER_MS;

		if (timer_stack >= TIMER_SEND_INTERVAL) // 타이머 값을 보낼 때가 되었음
		{
			timer_stack -= TIMER_SEND_INTERVAL;
			cout << "Timer Sended" << endl;

			// 양 클라에게 현재 시각 보내주기
			for (int i = 0; i < 2; i++)
			{
				ULONG64 time = _room->m_RoundTimer;
				try
				{
					PacketSend(_room->m_Users[i]->sock, _room->m_Users[i]->m_Buf,
						PackPacket(_room->m_Users[i]->m_Buf, PROTOCOL::TIMER, &time),
						0, "marking send()");
				}
				catch (runtime_error & e)
				{
					_room->m_Users[i]->m_State = STATE_DISCONNECT;
				}
			}
		}

		if (_room->m_RoundTimer <= 0) // 타이머가 완전히 소요됨
		{
			// 양 클라에게 게임 종료 보내주기
			for (int i = 0; i < 2; i++)
			{
				try
				{
					PacketSend(_room->m_Users[i]->sock, _room->m_Users[i]->m_Buf,
						PackPacket(_room->m_Users[i]->m_Buf, PROTOCOL::END_GAME),
						0, "ending send()");
				}
				catch (runtime_error & e)
				{
					_room->m_Users[i]->m_State = STATE_DISCONNECT;
				}
			}
			_room->m_State = ROOM_END;
			break;
		}

		Sleep(TIMER_MS);
	}

	return 0;
}