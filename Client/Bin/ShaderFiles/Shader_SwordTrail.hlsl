#include "Engine_Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture;

struct VS_IN
{
	float3 Position0 : POSITION0;
	float3 Position1 : POSITION1;
	float3 Position2 : POSITION2;
	float3 Position3 : POSITION3;

	float2 TexCoord0 : TEXCOORD0;
	float2 TexCoord1 : TEXCOORD1;
	float2 TexCoord2 : TEXCOORD2;
	float2 TexCoord3 : TEXCOORD3;

	float2 Lifetime : COLOR0;
};

struct VS_OUT
{
	float3 Position0 : POSITION0;
	float3 Position1 : POSITION1;
	float3 Position2 : POSITION2;
	float3 Position3 : POSITION3;

	float2 TexCoord0 : TEXCOORD0;
	float2 TexCoord1 : TEXCOORD1;
	float2 TexCoord2 : TEXCOORD2;
	float2 TexCoord3 : TEXCOORD3;

	float2 Lifetime : COLOR0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT      Out = (VS_OUT)0;

	vector      Position0 = float4(In.Position0, 1.f);
	vector      Position1 = float4(In.Position1, 1.f);
	vector      Position2 = float4(In.Position2, 1.f);
	vector      Position3 = float4(In.Position3, 1.f);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.Position0 = mul(Position0, matWVP).xyz;
	Out.Position1 = mul(Position1, matWVP).xyz;
	Out.Position2 = mul(Position2, matWVP).xyz;
	Out.Position3 = mul(Position3, matWVP).xyz;

	Out.Lifetime = In.Lifetime;

	return Out;
}



struct GS_IN
{
	float3 Position0 : POSITION0;
	float3 Position1 : POSITION1;
	float3 Position2 : POSITION2;
	float3 Position3 : POSITION3;

	float2 TexCoord0 : TEXCOORD0;
	float2 TexCoord1 : TEXCOORD1;
	float2 TexCoord2 : TEXCOORD2;
	float2 TexCoord3 : TEXCOORD3;

	float2 Lifetime : COLOR0;
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
    Out[0] = (GS_OUT) 0;
    Out[1] = (GS_OUT) 0;
    Out[2] = (GS_OUT) 0;
    Out[3] = (GS_OUT) 0;


	float3			vPosition;
	matrix			matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = mul(float4(In[0].Position0, 1.f), matVP);
	Out[0].vTexcoord = In[0].TexCoord0;

	Out[1].vPosition = mul(float4(In[0].Position1, 1.f), matVP);
	Out[1].vTexcoord = In[0].TexCoord1;

	Out[2].vPosition = mul(float4(In[0].Position2, 1.f), matVP);
	Out[2].vTexcoord = In[0].TexCoord2;

	Out[3].vPosition = mul(float4(In[0].Position3, 1.f), matVP);
	Out[3].vTexcoord = In[0].TexCoord3;

	Triangles.Append(Out[0]);
	Triangles.Append(Out[3]);
	Triangles.Append(Out[1]);
	Triangles.RestartStrip();

	Triangles.Append(Out[3]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[1]);
	Triangles.RestartStrip();
}



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


PS_OUT PS_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
	/*vector vNoise = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);

	if (Out.vColor.a == 0.f)
		discard;

	if (g_Alpha)
	{
		Out.vColor.a = In.vLifeTime.x - In.vLifeTime.y;
	}
	float fRatio = In.vLifeTime.y / In.vLifeTime.x;

	if (g_Color)
	{
		Out.vColor.rgb = lerp(g_StartColor, g_EndColor, fRatio);
	}

	if (g_Desolve)
	{
		if ((vNoise.r - fRatio) < g_DesolvePower * 0.01f)
		{
			Out.vColor.rgb = g_DesolveColor;
		}

		if (vNoise.r < fRatio)
		{
			discard;
		}
	}*/
	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT();
	}
}

