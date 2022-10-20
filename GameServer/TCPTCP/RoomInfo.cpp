#include "RoomInfo.h"
#include "ClientInfo.h"

_RoomInfo* RoomInfo[50];
int room_count = 0;

// �� �߰��ϱ�
_RoomInfo* AddRoomInfo()
{
	Enter_CS(); // �迭 �����ؾ� �ϴϱ� ġ������ ���� �� �����
	_RoomInfo* ptr = new _RoomInfo;
	ZeroMemory(ptr, sizeof(_RoomInfo));

	// ���� ���� ����
	ptr->m_State = ROOM_WAIT;
	ZeroMemory(ptr->m_Msg, BUFSIZE);

	// �÷��̾� ���� ����
	ZeroMemory(ptr->m_Users, sizeof(_ClientInfo*) * 2);

	// ���� ����
	ptr->m_RoundTimer = 0;
	for (int i = 0; i < 20; i++)
		ptr->m_Tiles[i] = new Tile(i);

	// �̺�Ʈ
	ptr->e_Matched = CreateEvent(NULL, TRUE, FALSE, NULL);
	ptr->e_GameStart = CreateEvent(NULL, TRUE, FALSE, NULL);
	ptr->e_EndGame = CreateEvent(NULL, TRUE, FALSE, NULL);

	RoomInfo[room_count++] = ptr;

	printf("\n����Ʈ�� �� �߰�\n");

	Leave_CS(); // �� �� ���
	return ptr;
}

// �� ���� �����
void RemoveRoomInfo(_RoomInfo* _ptr)
{
	printf("\n����Ʈ���� �� ����\n");

	Enter_CS(); // �迭 �����ؾ��ϴϱ� ġ������ ���� �� �����
	for (int i = 0; i < room_count; i++)
	{
		if (RoomInfo[i] == _ptr) // �ش��ϴ� �� ã�Ҵٸ�
		{
			// �̺�Ʈ �ݱ�
			CloseHandle(_ptr->e_Matched);
			CloseHandle(_ptr->e_GameStart);
			CloseHandle(_ptr->e_EndGame);

			// Ÿ�� �����
			for (int i = 0; i < 20; i++)
				delete _ptr->m_Tiles[i];

			// ���� �����
			delete RoomInfo[i];

			// �迭 �����
			for (int j = i; j < room_count - 1; j++)
				RoomInfo[j] = RoomInfo[j + 1];

			break; // �� �� �� �����ϱ� �ݺ��� Ż��
		}
	}
	room_count--;
	Leave_CS(); // �� �� ���
}

// �� �� ã��
_RoomInfo* SearchEmptyRoom(_ClientInfo* _client)
{
	for (int i = 0; i < room_count; i++) // ���� ��ϵ� �� �߿���
	{
		if (RoomInfo[i]->m_State == ROOM_WAIT) // �׳� ��� �ִ� ���� �ְ�
		{
			for (int j = 0; j < 2; j++) // Ŭ�� ����� ��������
			{
				if (RoomInfo[i]->m_Users[j] != nullptr)
				{
					if (RoomInfo[i]->m_Users[j] == _client) // �ڱ� �ڽ��� �ִ� ���
						break; // �� ���� �ƴϿ�

					if (RoomInfo[i]->m_Users[j]->m_CharacterIndex == _client->m_CharacterIndex) // ĳ���� ��ȣ�� ���� ���
						break; // �� ���� �ƴϿ�
				}
				if (RoomInfo[i]->m_Users[j] == nullptr) // �ű⿡ ���� �� �ڸ� ������
					return RoomInfo[i]; // �� ���� �ּ����ش�
			}
		}
	}

	// �ƹ� �浵 ���ų� ���� �� �� ���ְų� �� ��� ������� �´�
	return AddRoomInfo(); // ���� �� ���� �װ� �ּ����ش�
}

// �濡 ���� �߰��ϱ�
bool _RoomInfo::Add_User(_ClientInfo* _data)
{
	for (int i = 0; i < 2; i++) // �� �ڸ� ã�� �ݺ���
	{
		if (m_Users[i] == nullptr) // �� �ڸ� �߰�
		{
			m_Users[i] = _data; // �� �ڸ��� ���� �� Ŭ���� �ڸ��� �Ѵ�

			if (i == 1) // ��� �ο��� �𿴴ٸ�
			{
				// ��ٸ��� �ִ� �� ���ӻ��� Ȯ��
				try
				{
					PacketSend(m_Users[0]->sock, m_Users[0]->m_Buf,
						PackPacket(m_Users[0]->m_Buf, PROTOCOL::DEBUG), 0,
						"��Ī�� ����� �� ��Ƽ�� ������");
					PacketRecv(m_Users[0]->sock, m_Users[0]->m_Buf, "��Ī�� ����� �� ��Ƽ�� ������");
				}
				catch (runtime_error & e) // �� ��ٸ��� ���� ���
				{
					cout << e.what() << endl;
					Remove_User(m_Users[0]);
					return true;
				}
				m_Users[0]->m_PlayerPosition = 1;
				m_Users[1]->m_PlayerPosition = 2;
				cout << "���� �÷��� ���·� ��ȯ��" << endl;
				cout << "�÷��̾� 1 ��Ʈ : " << ntohs(m_Users[0]->addr.sin_port) << endl;
				cout << "�÷��̾� 2 ��Ʈ : " << ntohs(m_Users[1]->addr.sin_port) << endl;

				this->m_State = ROOM_PLAY;	// �� ���� ��ȯ

				SetEvent(e_Matched);		// ���� ���� �̺�Ʈ �ߵ�
			}
			return true;
		}
	}
	// ���� ���� �ϳ� �� ã������ false
	return false;
}

// �濡�� ���� ����
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

	// �����
	for (int i = 0; i < 2; i++)
	{
		if (m_Users[i] == _data)
		{
			// �迭 �����
			for (int j = i; j < 1; j++)
			{
				m_Users[j] = m_Users[j + 1];
				m_Users[j + 1] = nullptr;
			}
			break;
		}
	}

	// ���� ���� ���� ��
	if (m_State == ROOM_PLAY)
	{
		if(m_Users[0] == nullptr || m_Users[1] == nullptr)
			m_State = ROOM_END;
	}

	// ��� ��ٸ��� �ߵ� �ƴѵ� �濡 �ƹ��� ���� ���
	if (m_Users[0] == nullptr && m_State != ROOM_WAIT)
		RemoveRoomInfo(this); // �����θ� ����
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

// Ŭ���̾�Ʈ���� �ΰ��� �÷��� �غ�
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

// ���� ����
void _RoomInfo::Ingame_Start()
{
	// �ΰ��� ������ �ʱ�ȭ
	m_RoundTimer = 60000; // 3��

	// ���� ó��
	SetEvent(e_GameStart);

	// Ÿ�̸� ������ ����
	t_Timer = CreateThread(NULL, 0, ProcessTimer, this, 0, NULL);
	if (t_Timer == NULL) // ������ �� ����ν�
		throw runtime_error("Cannot Create Thread");
	else
		CloseHandle(t_Timer); // �� �� ���ν������ε� ���� Ŭ���̾�Ʈ �����忡 ���� ���Ұ�
}

// ������ Ÿ�� �׼���
void _RoomInfo::Ingame_Tile_Access(_ClientInfo* _data, POINT _coord)
{
	Tile* tile = m_Tiles[Map[_coord.y][_coord.x]];
	int client_id = _data->m_PlayerPosition;

	cout << "�� ��ǥ - " << _coord.x << ", " << _coord.y << " : " << Map[_coord.y][_coord.x] << endl;

	if (Map[_coord.y][_coord.x] > 99 && Map[_coord.y][_coord.x] / 100 == client_id) // �÷��̾� ���� ��ȹ�� �����ϴ� ���
	{
		cout << "���� ���� ����" << endl;
		UINT64 x = _coord.x;
		UINT64 y = _coord.y;
		// �ٷ� ������
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
		cout << "���� �Ұ����� ����" << endl;
		// �Ұ���
		PacketSend(_data->sock, _data->m_Buf,
			PackPacket(_data->m_Buf, PROTOCOL::TILE_UNACCEPTABLE), 0,
			"Ingame_Tile_Access - Unacceptable");
		return;
	}

	// �̹� ������ Ÿ�Ͽ� �����ϴ���, Ÿ���� ���� ��������� üũ
	if (tile->m_Owner == client_id) // �ڽ��� �̹� Ÿ���� ������ ���
	{
		cout << "�̹� ������ ���� ����" << endl;
		UINT64 x = _coord.x;
		UINT64 y = _coord.y;
		// �ٷ� ������
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
	else if (tile->m_Using) // Ÿ���� �ش� Ÿ�� ������� ���
	{
		cout << "���� Ÿ���� ������� Ÿ��" << endl;
		// �Ұ���
		PacketSend(_data->sock, _data->m_Buf,
			PackPacket(_data->m_Buf, PROTOCOL::TILE_UNACCEPTABLE), 0,
			"Ingame_Tile_Access - Unacceptable");
		return;
	}

	// ������ �ڽ� �ƴ� + �ƹ��� ��������� ���� ��� ������� �� �� ����
	
	// Ÿ�� ����� ǥ��
	Enter_CS();
	tile->m_Using = true;
	Leave_CS();

	_data->m_LastCoord = _coord;

	DIFFICULTY diff = m_RoundTimer > 120000 ? DIFFICULTY::HARD : m_RoundTimer > 60000 ? DIFFICULTY::NORMAL : DIFFICULTY::EASY;
	Quiz* quiz = Quiz::Get_Quiz(diff); // ���� ���
	USHORT answer = -1;

	if (_data->m_FreeTile <= 0)
	{
		// ������ ��ŷ�ߴ� �˷��ֱ�
		for (int i = 0; i < 2; i++)
		{
			cout << "��ŷ ����" << endl;
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
		// ���� Ǯ�� ����, ����� -> ���� -> ���� 1�� ����

		cout << "���� ����" << endl;
		// ���� ������
		PacketSend(_data->sock, _data->m_Buf,
			PackPacket(_data->m_Buf, PROTOCOL::TILE_QUESTION, quiz->m_Question, quiz->m_Answer_1, quiz->m_Answer_2, quiz->m_Answer_3, quiz->m_Answer_4),
			0, "question send()");

		cout << "��� ��ٸ���" << endl;
		PacketRecv(_data->sock, _data->m_Buf, "answer recv()");
		UINT64 protocol;
		GetProtocol(_data->m_Buf, protocol);
		if (protocol != PROTOCOL::ANSWER) // ����� ���ָ� ��������� ����
			throw runtime_error("answer recv - bad protocol");
		UnPackPacket(_data->m_Buf + 8, &answer);
	}
	// ������ ���� ���
	if (answer == quiz->answer_index || _data->m_FreeTile > 0) 
	{
		cout << "����" << endl;
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
	else // �ƴ� ���
	{
		cout << "����" << endl;
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

	cout << "���ھ� ����" << endl;
	// ������ ���� ������ Ǭ ����� ���ھ� �����ֱ�
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

	// ���� Ǯ�� ����
	delete quiz;
	tile->m_Using = false;

	// 

	return;
}

#define TIMER_MS 20 // ���� �ð���
#define TIMER_SEND_INTERVAL 1000 // Ÿ�̸� ������ �ֱ�

// �� Ÿ�̸� ������
DWORD WINAPI ProcessTimer(LPVOID _ptr)
{
	_RoomInfo* _room = (_RoomInfo*)_ptr;

	int timer_stack = 0; // Ÿ�̸� ����, 1000�� �Ǹ� ����

	Sleep(3000);

	cout << "timer - room state = " << (_room != nullptr) << ", " << _room->m_State << endl;

	// �÷��������� ������ ���� �� �ʿ� ����
	while (_room != nullptr && _room->m_State == ROOM_PLAY)
	{
		_room->m_RoundTimer -= TIMER_MS;
		timer_stack += TIMER_MS;

		if (timer_stack >= TIMER_SEND_INTERVAL) // Ÿ�̸� ���� ���� ���� �Ǿ���
		{
			timer_stack -= TIMER_SEND_INTERVAL;
			cout << "Timer Sended" << endl;

			// �� Ŭ�󿡰� ���� �ð� �����ֱ�
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

		if (_room->m_RoundTimer <= 0) // Ÿ�̸Ӱ� ������ �ҿ��
		{
			// �� Ŭ�󿡰� ���� ���� �����ֱ�
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