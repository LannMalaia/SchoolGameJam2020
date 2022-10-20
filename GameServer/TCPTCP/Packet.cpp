#include "Packet.h"

void GetProtocol(char* _ptr, UINT64& _protocol)
{
	memcpy(&_protocol, _ptr, sizeof(UINT64));
}

#pragma region PACKING

// PROTOCOL ONLY
int PackPacket(char* _buf, UINT64 _protocol)
{
	int size = 0;
	char* ptr = _buf + sizeof(int);

	memcpy(ptr, &_protocol, sizeof(UINT64));
	size = size + sizeof(UINT64);
	ptr = ptr + sizeof(UINT64);

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

int PackPacket(char* _buf, UINT64 _protocol, USHORT* _data)
{
	int size = 0;
	char* ptr = _buf + sizeof(int);

	memcpy(ptr, &_protocol, sizeof(UINT64));
	size = size + sizeof(UINT64);
	ptr = ptr + sizeof(UINT64);

	memcpy(ptr, _data, sizeof(USHORT));
	size = size + sizeof(USHORT);
	ptr = ptr + sizeof(USHORT);

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

// PROTOCOL + INT
int PackPacket(char* _buf, UINT64 _protocol, UINT64* _data)
{
	int size = 0;
	char* ptr = _buf + sizeof(int);

	memcpy(ptr, &_protocol, sizeof(UINT64));
	size = size + sizeof(UINT64);
	ptr = ptr + sizeof(UINT64);

	memcpy(ptr, _data, sizeof(UINT64));
	size = size + sizeof(UINT64);
	ptr = ptr + sizeof(UINT64);

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

int PackPacket(char* _buf, UINT64 _protocol, USHORT* _tile, USHORT* _p_position)
{
	int size = 0;
	char* ptr = _buf + sizeof(int);

	memcpy(ptr, &_protocol, sizeof(UINT64));
	size = size + sizeof(UINT64);
	ptr = ptr + sizeof(UINT64);

	memcpy(ptr, _tile, sizeof(USHORT));
	size = size + sizeof(USHORT);
	ptr = ptr + sizeof(USHORT);

	memcpy(ptr, _p_position, sizeof(USHORT));
	size = size + sizeof(USHORT);
	ptr = ptr + sizeof(USHORT);

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

int PackPacket(char* _buf, UINT64 _protocol, USHORT* _position, UINT64* _chara_index)
{
	int size = 0;
	char* ptr = _buf + sizeof(int);

	memcpy(ptr, &_protocol, sizeof(UINT64));
	size = size + sizeof(UINT64);
	ptr = ptr + sizeof(UINT64);

	memcpy(ptr, _position, sizeof(USHORT));
	size = size + sizeof(USHORT);
	ptr = ptr + sizeof(USHORT);

	memcpy(ptr, _chara_index, sizeof(UINT64));
	size = size + sizeof(UINT64);
	ptr = ptr + sizeof(UINT64);

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

// PROTOCOL + MESSAGE
int PackPacket(char* _buf, UINT64 _protocol, const wchar_t* _str1)
{
	int size = 0;
	int strsize1 = wcslen(_str1) * 2;
	char* ptr = _buf + sizeof(int);

	memcpy(ptr, &_protocol, sizeof(UINT64));
	size = size + sizeof(UINT64);
	ptr = ptr + sizeof(UINT64);

	memcpy(ptr, &strsize1, sizeof(strsize1));
	ptr = ptr + sizeof(strsize1);
	size = size + sizeof(strsize1);

	memcpy(ptr, _str1, strsize1);
	ptr = ptr + strsize1;
	size = size + strsize1;

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

// PROTOCOL + MESSAGE, X, Y
int PackPacket(char* _buf, UINT64 _protocol, const wchar_t* _str, UINT64* _x, UINT64* _y)
{
	int size = 0;
	int strsize = wcslen(_str) * 2;
	char* ptr = _buf + sizeof(int);

	memcpy(ptr, &_protocol, sizeof(UINT64));
	size = size + sizeof(UINT64);
	ptr = ptr + sizeof(UINT64);

	memcpy(ptr, &strsize, sizeof(strsize));
	ptr = ptr + sizeof(strsize);
	size = size + sizeof(strsize);

	memcpy(ptr, _str, strsize);
	ptr = ptr + strsize;
	size = size + strsize;

	memcpy(ptr, _x, sizeof(UINT64));
	ptr = ptr + sizeof(UINT64);
	size = size + sizeof(UINT64);

	memcpy(ptr, _y, sizeof(UINT64));
	ptr = ptr + sizeof(UINT64);
	size = size + sizeof(UINT64);

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

// PROTOCOL + is_me, X, Y
int PackPacket(char* _buf, UINT64 _protocol, USHORT* _is_me, UINT64* _x, UINT64* _y)
{
	int size = 0;
	char* ptr = _buf + sizeof(int);

	memcpy(ptr, &_protocol, sizeof(UINT64));
	size = size + sizeof(UINT64);
	ptr = ptr + sizeof(UINT64);

	memcpy(ptr, _is_me, sizeof(USHORT));
	ptr = ptr + sizeof(USHORT);
	size = size + sizeof(USHORT);

	memcpy(ptr, _x, sizeof(UINT64));
	ptr = ptr + sizeof(UINT64);
	size = size + sizeof(UINT64);

	memcpy(ptr, _y, sizeof(UINT64));
	ptr = ptr + sizeof(UINT64);
	size = size + sizeof(UINT64);

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

// PROTOCOL + question, answer_1, answer_2, answer_3, answer_4
int PackPacket(char* _buf, UINT64 _protocol, const wchar_t _question[512], const wchar_t _answer[64], const wchar_t _answer_2[64], const wchar_t _answer_3[64], const wchar_t _answer_4[64])
{
	int size = 0;
	int strsize1 = wcslen(_question) * 2;
	int strsize2 = wcslen(_answer) * 2;
	int strsize3 = wcslen(_answer_2) * 2;
	int strsize4 = wcslen(_answer_3) * 2;
	int strsize5 = wcslen(_answer_4) * 2;
	char* ptr = _buf + sizeof(int);

	memcpy(ptr, &_protocol, sizeof(UINT64));
	size = size + sizeof(UINT64);
	ptr = ptr + sizeof(UINT64);

	memcpy(ptr, &strsize1, sizeof(strsize1));
	ptr = ptr + sizeof(strsize1);
	size = size + sizeof(strsize1);

	memcpy(ptr, _question, strsize1);
	ptr = ptr + strsize1;
	size = size + strsize1;

	memcpy(ptr, &strsize2, sizeof(strsize2));
	ptr = ptr + sizeof(strsize2);
	size = size + sizeof(strsize2);

	memcpy(ptr, _answer, strsize2);
	ptr = ptr + strsize2;
	size = size + strsize2;

	memcpy(ptr, &strsize3, sizeof(strsize3));
	ptr = ptr + sizeof(strsize3);
	size = size + sizeof(strsize3);

	memcpy(ptr, _answer_2, strsize3);
	ptr = ptr + strsize3;
	size = size + strsize3;

	memcpy(ptr, &strsize4, sizeof(strsize4));
	ptr = ptr + sizeof(strsize4);
	size = size + sizeof(strsize4);

	memcpy(ptr, _answer_3, strsize4);
	ptr = ptr + strsize4;
	size = size + strsize4;

	memcpy(ptr, &strsize5, sizeof(strsize5));
	ptr = ptr + sizeof(strsize5);
	size = size + sizeof(strsize5);

	memcpy(ptr, _answer_4, strsize5);
	ptr = ptr + strsize5;
	size = size + strsize5;

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

// PROTOCOL + is_me + quiz + tile + combo + total_score + tile_multiplier
int PackPacket(char* _buf, UINT64 _protocol, USHORT* _is_me, UINT64* _quiz, USHORT* _tile, USHORT* _combo, UINT64* _score, UINT64* _tile_mult)
{
	int size = 0;
	char* ptr = _buf + sizeof(int);

	memcpy(ptr, &_protocol, sizeof(UINT64));
	size = size + sizeof(UINT64);
	ptr = ptr + sizeof(UINT64);

	memcpy(ptr, _is_me, sizeof(USHORT));
	ptr = ptr + sizeof(USHORT);
	size = size + sizeof(USHORT);

	memcpy(ptr, _quiz, sizeof(UINT64));
	ptr = ptr + sizeof(UINT64);
	size = size + sizeof(UINT64);

	memcpy(ptr, _tile, sizeof(USHORT));
	ptr = ptr + sizeof(USHORT);
	size = size + sizeof(USHORT);

	memcpy(ptr, _combo, sizeof(USHORT));
	ptr = ptr + sizeof(USHORT);
	size = size + sizeof(USHORT);

	memcpy(ptr, _score, sizeof(UINT64));
	ptr = ptr + sizeof(UINT64);
	size = size + sizeof(UINT64);

	memcpy(ptr, _tile_mult, sizeof(UINT64));
	ptr = ptr + sizeof(UINT64);
	size = size + sizeof(UINT64);

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

#pragma endregion


#pragma region UNPACKING

void UnPackPacket(char* _buf, wchar_t* _nick)
{
	int str1size;

	char* ptr = _buf;

	memcpy(&str1size, ptr, sizeof(str1size));
	ptr = ptr + sizeof(str1size);

	memcpy(_nick, ptr, str1size);
	ptr = ptr + str1size;
}

void UnPackPacket(char* _buf, USHORT* _data)
{
	char* ptr = _buf;

	memcpy(_data, ptr, sizeof(USHORT));
	ptr = ptr + sizeof(USHORT);
}

void UnPackPacket(char* _buf, UINT64* _data)
{
	char* ptr = _buf;

	memcpy(_data, ptr, sizeof(UINT64));
	ptr = ptr + sizeof(UINT64);
}

void UnPackPacket(char* _buf, UINT64* _x, UINT64* _y)
{
	int strsize;

	char* ptr = _buf;

	memcpy(_x, ptr, sizeof(UINT64));
	ptr = ptr + sizeof(UINT64);

	memcpy(_y, ptr, sizeof(UINT64));
	ptr = ptr + sizeof(UINT64);
}

void UnPackPacket(char* _buf, USHORT* _is_me, UINT64* _x, UINT64* _y)
{
	int strsize;

	char* ptr = _buf;

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_is_me, ptr, sizeof(USHORT));
	ptr = ptr + sizeof(USHORT);

	memcpy(_x, ptr, sizeof(UINT64));
	ptr = ptr + sizeof(UINT64);

	memcpy(_x, ptr, sizeof(UINT64));
	ptr = ptr + sizeof(UINT64);
}


#pragma endregion