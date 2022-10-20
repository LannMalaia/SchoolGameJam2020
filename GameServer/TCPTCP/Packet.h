#pragma once
#include "global.h"

void GetProtocol(char* _ptr, UINT64& _protocol);

int PackPacket(char* _buf, UINT64 _protocol);
int PackPacket(char* _buf, UINT64 _protocol, USHORT* _data);
int PackPacket(char* _buf, UINT64 _protocol, UINT64* _data);
int PackPacket(char* _buf, UINT64 _protocol, USHORT* _tile, USHORT* _p_position);
int PackPacket(char* _buf, UINT64 _protocol, USHORT* _position, UINT64* _p_position);
int PackPacket(char* _buf, UINT64 _protocol, const wchar_t* _str1);
int PackPacket(char* _buf, UINT64 _protocol, const wchar_t* _str, UINT64* _x, UINT64* _y);
int PackPacket(char* _buf, UINT64 _protocol, USHORT* _is_me, UINT64* _x, UINT64* _y);
int PackPacket(char* _buf, UINT64 _protocol, const wchar_t _question[512], const wchar_t _answer[64], const wchar_t _answer_2[64], const wchar_t _answer_3[64], const wchar_t _answer_4[64]);
int PackPacket(char* _buf, UINT64 _protocol, USHORT* _is_me, UINT64* _quiz, USHORT* _combo, USHORT* _tile, UINT64* _score, UINT64* _tile_mult);

void UnPackPacket(char* _buf, wchar_t* _nick);
void UnPackPacket(char* _buf, USHORT* _data);
void UnPackPacket(char* _buf, UINT64* _data);
void UnPackPacket(char* _buf, UINT64* _x, UINT64* _y);
void UnPackPacket(char* _buf, USHORT* _is_me, UINT64* _x, UINT64* _y);