#include "Engine_Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture;
texture2D	g_MaskTexture;

float		g_fFlowTime;
float		g_CurrentRatio;
float		g_PastRatio;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);		
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_FLOW(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vDetailUV = In.vTexcoord;
	vDetailUV.y -= g_fFlowTime;

	vector		vDestDiffuse = g_Texture.Sample(LinearSampler, vDetailUV);
	vector		vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);

	if (0.1f > vMask.r && 0.1f > vMask.g && 0.1f > vMask.b)
		discard;

	vector		vResult = vDestDiffuse * vMask;

	Out.vColor = vResult;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_HUD(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	if (Out.vColor.a < 0.1f)
		discard;
	if (g_PastRatio > g_CurrentRatio)
	{
		if (In.vTexcoord.x > g_PastRatio * 0.8f + 0.1f) discard;
		else if (In.vTexcoord.x > g_CurrentRatio * 0.8f + 0.1f) Out.vColor = float4(1.f, 1.f, 0.5f, Out.vColor.a);
	}
	else
	{
		if (In.vTexcoord.x > g_PastRatio * 0.8f + 0.1f) discard;
	}


	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass FlowPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FLOW();
	}

	pass HUD
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_HUD();
	}
}

