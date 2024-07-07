#pragma once
#include<windows.h>
#include <process.h>

namespace Client
{
	//Level�� �߰��Ǹ� �Ʒ� ���ڿ� ���� ���缭 �߰����ֱ�
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

#define EFFECTMGR 						CEffectManager::GetInstance()
	//�Ʒ� enum, �ǵ��� ���� - ����ȣ
#pragma region EFFECT_ENUM
	enum EFFECTTYPE {
		SPREAD, DROP, GROWOUT, SPREAD_SIZEUP,
		SPREAD_NONROTATION, TORNADO, SPREAD_SPEED_DOWN,
		SLASH_EFFECT, SPREAD_SPEED_DOWN_SIZE_UP, GATHER, EXTINCTION,
		GROWOUTY, EFFECT_END
	};
	enum PARTICLETYPE { PART_POINT, PART_MESH, PART_RECT, PART_END };
	enum EFFECTMODELTYPE { CUBE, CIRCLE, SLASH, TYPE_END };
	enum TRAILFUNCTYPE { TRAIL_EXTINCT, TRAIL_ETERNAL, TRAIL_END };
	enum TRAIL_USAGE { USAGE_SWORD, USAGE_EYE, USAGE_END };
#pragma endregion EFFECT_ENUM

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
