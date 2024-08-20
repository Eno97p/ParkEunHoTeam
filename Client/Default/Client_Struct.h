#pragma once

#include "Client_Defines.h"
#include"Engine_Defines.h"
#include "GameObject.h"


namespace Client
{

	struct _tagMonsterInit_Property
	{
		_float4 vPos = { 0.f,0.f,0.f,0.f };
		wstring strMonsterTag = TEXT("");
		void* pArgDesc = nullptr;
		size_t uArgDescSize = 0;
	};




}