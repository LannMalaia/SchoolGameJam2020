#pragma once
#include "global.h"

extern int Map[8][8];

struct Tile
{
	int m_Index; // 타일 인덱스
	bool m_Using; // 현재 사용중
	int m_Owner; // 소유자 (0은 없음, 1,2 는 각 플레이어)

	Tile(int _index)
	{
		m_Index = _index;
		m_Using = false;
		m_Owner = 0;
	}
};

struct Quiz
{
	wchar_t m_Question[512];

	wchar_t m_Answer_1[64];
	wchar_t m_Answer_2[64];
	wchar_t m_Answer_3[64];
	wchar_t m_Answer_4[64];

	int answer_index;

	Quiz()
	{
		ZeroMemory(m_Question, 512);
		ZeroMemory(m_Answer_1, 64);
		ZeroMemory(m_Answer_2, 64);
		ZeroMemory(m_Answer_3, 64);
		ZeroMemory(m_Answer_4, 64);

		answer_index = 0;
	}

	Quiz(const wchar_t _question[512], const wchar_t _answer_1[64], const wchar_t _answer_2[64], const wchar_t _answer_3[64], const wchar_t _answer_4[64], int _answer_index)
	{
		ZeroMemory(m_Question, 512);
		ZeroMemory(m_Answer_1, 64);
		ZeroMemory(m_Answer_2, 64);
		ZeroMemory(m_Answer_3, 64);
		ZeroMemory(m_Answer_4, 64);

		memcpy_s(m_Question, 512, _question, 512);
		memcpy_s(m_Answer_1, 64, _answer_1, 64);
		memcpy_s(m_Answer_2, 64, _answer_2, 64);
		memcpy_s(m_Answer_3, 64, _answer_3, 64);
		memcpy_s(m_Answer_4, 64, _answer_4, 64);

		answer_index = _answer_index;
	}

	static Quiz* Get_Quiz(const DIFFICULTY _difficulty);
};

extern Quiz* EasyQuizs[100];
extern Quiz* NormalQuizs[100];
extern Quiz* HardQuizs[50];