#pragma once

#include<cuda.h>
#include<cuda_runtime.h>
#include <device_atomic_functions.h>
#include <device_launch_parameters.h>

struct Cu_VTXMATRIX
{
	float4 vRight;
	float4 vUp;
	float4 vLook;
	float4 vTranslation;
	float2 vLifeTime;
	float vGravity;
};

//XMFLOAT4			vRight;
//XMFLOAT4			vUp;
//XMFLOAT4			vLook;
//XMFLOAT4			vTranslation;
//XMFLOAT2			vLifeTime;
//_float			   vGravity;