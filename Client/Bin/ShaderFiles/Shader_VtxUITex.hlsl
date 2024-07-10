#include "Engine_Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture;
texture2D	g_MaskTexture;
texture2D	g_DisolveTexture;

bool		g_bIsFadeIn;
float		g_fAlphaTimer;
float		g_DisolveValue = 1.f;

float		g_fFlowTime;
float		g_CurrentRatio;
float		g_PastRatio;

matrix		g_RotationMatrix; // 회전 행렬

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

VS_OUT VS_ROTATION(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	float3 texCoord3 = float3(In.vTexcoord - 0.5, 1.0);
	float3 rotatedTexcoord = mul(texCoord3, g_RotationMatrix);
	Out.vTexcoord = rotatedTexcoord.xy + 0.5;

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

	if (g_bIsFadeIn)
	{
		Out.vColor.a = 1.f - g_fAlphaTimer;
	}
	else
	{
		Out.vColor.a = g_fAlphaTimer;
	}

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

PS_OUT PS_FADE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	if (Out.vColor.a < 0.1f)
		discard;

	float fAlpha = Out.vColor.a;

	if (g_bIsFadeIn)
	{
		Out.vColor.a = 1.f - g_fAlphaTimer;
	}
	else
	{
		float fResultAlpha = g_fAlphaTimer;

		if (fAlpha < fResultAlpha)
			fResultAlpha = fAlpha;

		Out.vColor.a = fResultAlpha;
	}

	return Out;
}

PS_OUT PS_FADE_DISSOLVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a < 0.1f)
		discard;

	vector vDissolve = g_DisolveTexture.Sample(LinearSampler, In.vTexcoord);
	float dissolveValue = (vDissolve.r + vDissolve.g + vDissolve.b) / 3.f;

	if (g_bIsFadeIn)
	{
		if ((g_DisolveValue - dissolveValue) < 0.05f)
		{
			Out.vColor = vDiffuse;
			return Out;
		}
		else if (dissolveValue < g_DisolveValue)
		{
			discard;
		}
		else
		{
			Out.vColor.rgb = float3(0.f, 0.f, 0.f);
			Out.vColor.a = vDiffuse.a;
		}
	}
	else
	{
		if ((g_DisolveValue - dissolveValue) > 0.05f)
		{
			Out.vColor = vDiffuse;
			return Out;
		}
		else if (dissolveValue > g_DisolveValue)
		{
			discard;
		}
		else
		{
			Out.vColor.rgb = float3(0.f, 0.f, 0.f);
			Out.vColor.a = vDiffuse.a;
		}
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

	pass FadeInOut
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FADE();
	}

		pass Fade_Dissolve
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FADE_DISSOLVE();
	}

		pass RotationPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_ROTATION();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}

