#include "Engine_Shader_Defines.hlsli"

/* ������Ʈ ���̺�(������̺�) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float3 g_StartColor, g_EndColor, g_BloomColor;
texture2D	g_Texture, g_DesolveTexture;
bool g_Desolve , g_Alpha;
float g_BloomPower = 0.f;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
	float2		vLifeTime : COLOR0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;

	float2		vLifeTime : COLOR0;
};

/* ���� ���̴� :  /* 
/* 1. ������ ��ġ ��ȯ(����, ��, ����).*/
/* 2. ������ ���������� �����Ѵ�. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		TransformMatrix = matrix(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vLifeTime = In.vLifeTime;

	return Out;
}

/* TriangleList�� ��� : ���� ������ �޾Ƽ� w�����⸦ �� ������ ���ؼ� �����Ѵ�. */
/* ����Ʈ(��������ǥ��) ��ȯ. */
/* �����Ͷ����� : �������� �ѷ��׿��� �ȼ��� ������, ������ ���������Ͽ� �����. -> �ȼ��� ���������!!!!!!!!!!!! */


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float2		vLifeTime : COLOR0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};


PS_OUT PS_SWORDTRAIL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	//Out.vColor
	float fRatio = In.vLifeTime.y / In.vLifeTime.x;
	float2 adjustedTexcoord = In.vTexcoord;
	adjustedTexcoord.x = lerp(1.0, 0.0, fRatio);

	float4 Color = g_Texture.Sample(LinearSampler, adjustedTexcoord);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);

	if (Color.a == 0.f)
		discard;

	float brightness = 0.299 * Color.r + 0.587 * Color.g + 0.114 * Color.b;
	float newAlpha = 1.0 - brightness;

	Out.vColor = float4(Color.rgb, newAlpha);

	if (g_Alpha)
	{
		Out.vColor.a = In.vLifeTime.x - In.vLifeTime.y;
	}

	

	if (g_Desolve)
	{
		if (vNoise.r < fRatio)
		{
			discard;
		}
	}
	Out.vColor.rgb = lerp(g_StartColor, g_EndColor, fRatio);

	return Out;
}

PS_OUT PS_BLOOM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);
	if (Out.vColor.a == 0.f)
		discard;

	float fRatio = In.vLifeTime.y / In.vLifeTime.x;

	if (g_Desolve)
	{
		if (vNoise.r < fRatio)
		{
			discard;
		}
	}

	Out.vColor = float4(g_BloomColor, g_BloomPower);

	return Out;
}


struct GS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : TEXCOORD0;

	float2		vLifeTime : COLOR0;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;

	float2		vLifeTime : COLOR0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT Out[4];
	Out[0] = (GS_OUT)0;
	Out[1] = (GS_OUT)0;
	Out[2] = (GS_OUT)0;
	Out[3] = (GS_OUT)0;


	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();

}

technique11 DefaultTechnique
{
	
	pass SwordTrail
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SWORDTRAIL();
	}

	pass Trail_Bloom
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOOM();
	}

}

