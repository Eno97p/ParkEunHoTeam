#pragma once
#include<windows.h>
#include <process.h>

namespace Client
{
	//Level�� �߰��Ǹ� �Ʒ� ���ڿ� ���� ���缭 �߰����ֱ�
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };


	constexpr const char* LevelNames[] = {
		"Static",
		"Loading",
		"Logo",
		"GamePlay",
		"End"
	};


	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;


}

#include "Client_Function.hpp"

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
