#pragma once
#include<windows.h>
#include <process.h>

namespace Client
{
	//Level이 추가되면 아래 문자열 순서 맞춰서 추가해주기
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_ACKBAR, LEVEL_JUGGLAS, LEVEL_ANDRASARENA, LEVEL_GRASSLAND, LEVEL_END };

	enum CAMERA_INDEX { CAM_FREE = 0, CAM_THIRDPERSON, CAM_SIDEVIEW, CAM_TRANSITION, CAM_END };

	enum FRUSTUM { FRUSTUM_NEAR, FRUSTUM_MIDDLE, FRUSTUM_FAR, FRUSTUM_END };

#define EFFECTMGR 						CEffectManager::GetInstance()
	//아래 enum, 건들지 말것 - 박은호
#pragma region EFFECT_ENUM
	enum EFFECTTYPE {
		SPREAD, DROP, GROWOUT, SPREAD_SIZEUP,
		SPREAD_NONROTATION, TORNADO, SPREAD_SPEED_DOWN,
		SLASH_EFFECT, SPREAD_SPEED_DOWN_SIZE_UP, GATHER, EXTINCTION,
		GROWOUTY, GROWOUT_SPEEDDOWN, LEAF_FALL, SPIRAL_EXTINCTION, SPIRAL_SPERAD, LENZ_FLARE, BLOW, EFFECT_END
	};
	enum PARTICLETYPE { PART_POINT, PART_MESH, PART_RECT, PART_END };
	enum EFFECTMODELTYPE { CUBE, CIRCLE, SLASH, LEAF0, LEAF1, GRASS, ROCK0, ROCK1, BLADE_SLASH, BLADE_SLASH_LONG, NEEDLE, TYPE_END };
	enum TRAILFUNCTYPE { TRAIL_EXTINCT, TRAIL_ETERNAL, TRAIL_END };
	enum TRAIL_USAGE { USAGE_SWORD, USAGE_EYE, USAGE_END };
#pragma endregion EFFECT_ENUM

	constexpr const char* LevelNames[] = {
		"Static",
		"Loading",
		"Logo",
		"GamePlay",
		"Ackbar",
		"Jugglas",
		"AndrasArena",
		"GrassLand",
		"End"
	};


	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
	float fSlowValue = 1.f;


}

#include "Client_Function.hpp"

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
