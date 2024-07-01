
#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_SpecularTexture;
texture2D g_OpacityTexture;
texture2D g_EmissiveTexture;
texture2D g_RoughnessTexture;
texture2D g_MetalicTexture;

bool g_Hit;
float g_Alpha = 1.f;

bool g_bDiffuse = false;
bool g_bNormal = false;
bool g_bSpecular = false;
bool g_bOpacity = false;
bool g_bEmissive = false;
bool g_bRoughness = false;
bool g_bMetalic = false;

/* 이 메시에게 영향을 주는 뼈들. */
matrix		g_BoneMatrices[512];

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT;

	uint4		vBlendIndices : BLENDINDICES;
	float4		vBlendWeights : BLENDWEIGHTS;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

/* 정점 셰이더 :  /* 
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct VS_OUT_LIGHTDEPTH
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

VS_OUT_LIGHTDEPTH VS_MAIN_LIGHTDEPTH(VS_IN In)
{
	VS_OUT_LIGHTDEPTH		Out = (VS_OUT_LIGHTDEPTH)0;

	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);	
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;

	return Out;

}

/* TriangleList인 경우 : 정점 세개를 받아서 w나누기를 각 정점에 대해서 수행한다. */
/* 뷰포트(윈도우좌표로) 변환. */
/* 래스터라이즈 : 정점으로 둘러쌓여진 픽셀의 정보를, 정점을 선형보간하여 만든다. -> 픽셀이 만들어졌다!!!!!!!!!!!! */


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
    vector		vSpecular : SV_TARGET3;
    vector		vEmissive : SV_TARGET4;
	vector		vRoughness : SV_TARGET5;
	vector		vMetalic : SV_TARGET6;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bOpacity) vDiffuse.a *= vOpacity.r;
    if (vDiffuse.a < 0.1f)
        discard;

	if (g_bDiffuse) Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 0.f);
	if (g_bEmissive) Out.vEmissive = vEmissive;

    if (g_Hit)
    {
        Out.vDiffuse = float4(1.f, 0.f, 0.f, 0.3f);
    }

	vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
	vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
	if (g_bEmissive) Out.vEmissive = vEmissive;
	if (g_bRoughness) Out.vRoughness = vRoughness;
	if (g_bMetalic) Out.vMetalic = vMetalic;

    return Out;
}

PS_OUT PS_WHISPERSWORD(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
	vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bOpacity) vDiffuse.a *= vOpacity.r;
	if (vDiffuse.a < 0.1f)
		discard;

	if (g_bDiffuse) Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 0.f);
	if (g_bEmissive) Out.vEmissive = vEmissive;

	vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
	vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
	if (g_bEmissive) Out.vEmissive = vEmissive;
	Out.vRoughness = vector(0.7f, 0.7f, 0.7f, 1.f);
	Out.vMetalic = vector(1.f, 1.f, 1.f, 1.f);


	if (In.vTexcoord.y > 757.f/2048.f && In.vTexcoord.x > 435.f/2048.f && In.vTexcoord.x < 821.f/2048.f)
	{
		Out.vDiffuse.a = (Out.vDiffuse.r + Out.vDiffuse.g + Out.vDiffuse.b) / 3.f;
		if (Out.vDiffuse.a < 0.25f) discard;
	}

	return Out;
}

struct PS_OUT_BLEND
{
	vector		vDiffuse : SV_TARGET0;
};

PS_OUT_BLEND PS_ALPHA(PS_IN In)
{
	PS_OUT_BLEND Out = (PS_OUT_BLEND)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
	vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bOpacity) vDiffuse.a *= vOpacity.r;

	if (g_bDiffuse) Out.vDiffuse = vDiffuse;
	if (g_bEmissive) Out.vDiffuse += vEmissive;

	if (g_Hit)
	{
		Out.vDiffuse = float4(1.f, 0.f, 0.f, 0.3f);
	}

	Out.vDiffuse.a = g_Alpha;

	return Out;
}

//PS_OUT_BLEND PS_CLONE(PS_IN In)
//{
//	PS_OUT_BLEND Out = (PS_OUT_BLEND)0;
//
//	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
//
//	if (g_bDiffuse) Out.vDiffuse = vDiffuse;
//
//	Out.vDiffuse.a = g_Alpha;
//
//	int diffCount = 0;
//	for (int i = 0; i < 3; i++)
//	{
//		for (int j = 0; j < 3; j++)
//		{
//			float2 UV = float2(In.vTexcoord.x + (i - 1) / 1280.f, In.vTexcoord.y + (i - 1) / 720.f);
//			vector vAround = g_DiffuseTexture.Sample(LinearSampler, UV);
//			if (vAround.a != 0.f)
//			{
//				diffCount++;
//			}
//		}
//	}
//	if (diffCount > 3)
//	{
//		Out.vDiffuse.rgb = float3(0.3f, 0.3f, 1.f);
//	}
//	else
//	{
//		Out.vDiffuse.a = 0.f;
//	}
//
//	return Out;
//}

PS_OUT_BLEND PS_CLONE(PS_IN In)
{
	PS_OUT_BLEND Out = (PS_OUT_BLEND)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bDiffuse) Out.vDiffuse = vDiffuse;
	Out.vDiffuse.a = g_Alpha;

	float3 LightDir = float3(0.0f, 1.0f, 0.0f); // 빛의 방향
	float3 RimColor = float3(1.0f, 1.0f, 1.0f); // 림라이트 색상
	float RimPower = 2.0f; // 림라이트 강도

	float3 normal = normalize(In.vNormal);
	float NdotL = dot(normal, LightDir);

	// 림라이트 계산
	float rim = 1.0f - saturate(NdotL);

	rim = pow(rim, RimPower);

	Out.vDiffuse.rgb = rim * RimColor;
	Out.vDiffuse.a *= rim;
	return Out;
}

struct PS_IN_LIGHTDEPTH
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT_LIGHTDEPTH
{	
	vector		vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN_LIGHTDEPTH In)
{
	PS_OUT_LIGHTDEPTH		Out = (PS_OUT_LIGHTDEPTH)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if (vDiffuse.a < 0.1f)
		discard;	
	
	Out.vLightDepth = vector(In.vProjPos.w / 3000.f, 0.0f, 0.f, 0.f);

	return Out;
}

struct PS_OUT_DISTORTION
{
	vector		vDistortion : SV_TARGET0;
};

PS_OUT_DISTORTION PS_DISTORTION(PS_IN In)
{
	PS_OUT_DISTORTION Out = (PS_OUT_DISTORTION)0;

	vector vDistortion = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if (vDistortion.a < 0.1f)
		discard;

	if (g_bDiffuse) Out.vDistortion = vDistortion;

	if (g_Hit)
	{
		Out.vDistortion = float4(1.f, 0.f, 0.f, 0.3f);
	}

	return Out;
}

technique11 DefaultTechnique
{
	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
	pass DefaultPass_0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass LightDepth_1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN_LIGHTDEPTH();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
	}

	pass Alpha_2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHA();
	}

	pass WhisperSword_3
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WHISPERSWORD();
	}

	pass Distortion_4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION();
	}

	pass Clone_5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_CLONE();
	}
}

