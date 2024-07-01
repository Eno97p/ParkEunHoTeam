#pragma once

#pragma warning (disable : 4251)

#include <vld.h>
#include <d3d11.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>


#include"PxPhysicsAPI.h"

#include <random>

#include "fx11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK\Keyboard.h"
#include "DirectXTK\Mouse.h"
#include "DirectXTK\ScreenGrab.h"
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"
#include "DirectXTK\Effects.h"


#include "assimp\scene.h"
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tchar.h>
using namespace DirectX;
using namespace physx;

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <ctime>
#include <random>
#include <cmath>
#include <memory>
#include <filesystem>

#include <queue>
#include <stack>
#include<cstring>
#include<cassert>
#include<locale>



#include "fmod.hpp"
#pragma comment(lib, "fmod_vc.lib")
#include <io.h>
#include <vld.h>



namespace Engine
{
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum NodeStates { SUCCESS, FAILURE, RUNNING, COOLING, NONE };		//비헤이비어 노드 반환값
	enum Decorator { CoolDown, Condition, Deco_End };								//베헤비어 트리 데코레이터
	enum MOUSEWHEELSTATE { WHEEL_UP, WHEEL_DOWN, WHEEL_NONE };
	enum UISORT_PRIORITY { FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH, SEVENTH, EIGHT, NINETH, TENTH,
		ELEVENTH, TWELFTH, SORT_END }; //UI정렬

	enum CHANNELID { SOUND_EFFECT, SOUND_PLAYER, SOUND_MONSTER, SOUND_BOSS, SOUND_BGM, MAXCHANNEL };

	enum class eEVENT_TYPE
	{
		CREATE_OBJECT,		// CreateObject
		DELETE_OBJECT,		// DeleteObject
		SCENE_CHANGE,		// SceneChange
		NONE,
	};

}

#include "Engine_Typedef.h"
#include "Engine_Function.hpp"
#include"Engine_Functor.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Functor.h"



#define WORLD_CMETER 0.01f
#define WORLD_METER  1.0f
#define WORLD_SPEED 1.0f



//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//
//#ifndef DBG_NEW 
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
//#define new DBG_NEW 
//#endif
//#endif // _DEBUG


using namespace Engine;
using namespace std;

