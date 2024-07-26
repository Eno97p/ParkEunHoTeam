#include "Engine_Shader_Defines.hlsli"

/* ������Ʈ ���̺�(������̺�) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture , g_DesolveTexture;
float		g_Ratio;

//������
float		g_CurTime, g_Speed;
float3		g_Color;


struct VS_IN
{
	float3		vPosition : POSITION;
	float3      vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3      vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;

};

/* ���� ���̴� :  /* 
/* 1. ������ ��ġ ��ȯ(����, ��, ����).*/
/* 2. ������ ���������� �����Ѵ�. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float3 startPos = g_WorldMatrix[3].xyz;

	// �������� ���� (���� ����� Look ����)
	float3 laserDir = normalize(g_WorldMatrix[2].xyz);

	// ��ü ������ ���� (Look ���� ������ ������)
	float totalLength = length(g_WorldMatrix[2].xyz);

	// ������ ���� ��ġ�� ���� ��ġ�� ��ȯ
	float3 worldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;

	// ���� �������� ���������� �Ÿ� ���
	float distanceFromStart = dot(worldPos - startPos, laserDir);

	// �Ÿ��� ���� ������ ��� (0.1���� �����Ͽ� 1.0����, 5% �Ÿ����� �ִ�)
	float rawScale = saturate(distanceFromStart / (totalLength * 0.05f));

	// ���� ������ ���� (�� ������ Ŀ������)
	float scale = lerp(0.2, 1.0, pow(rawScale, 0.2));

	// x�� y ��ǥ�� ������ ����
	float2 scaledXY = In.vPosition.xy * scale;

	float3 newPosition = float3(scaledXY, In.vPosition.z);

	Out.vPosition = mul(float4(newPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

VS_OUT VS_CYLINDER(VS_IN In)
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


PS_OUT PS_LazerBase(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;

	uv.y += g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, uv);

	Color.a = Color.r;

	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;
	
	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // �ȼ� ���
		}

	}
	Out.vColor = Color;
	return Out;
}

PS_OUT PS_Cylinder(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;

	uv.y -= g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, uv);

	Color.a = 1- Color.r;

	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;

	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // �ȼ� ���
		}

	}
	Out.vColor = Color;
	return Out;
}


PS_OUT PS_Distortion(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;
	vector Color = g_Texture.Sample(LinearSampler, uv);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, uv);

	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;

	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // �ȼ� ���
		}

	}
	Out.vColor = Color;
	return Out;
}




technique11 DefaultTechnique
{
	/* Ư�� �������� ������ �� �����ؾ��� ���̴� ����� ��Ʈ���� ���̰� �ִ�. */
	pass LazerPass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_LazerBase();
	}

	pass Distortion
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Distortion();
	}

	pass Cylinder
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Cylinder();
	}

	pass Cylinder_Distortion
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Distortion();
	}


	
}

