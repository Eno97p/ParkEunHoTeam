#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture , g_DesolveTexture;
float		g_Ratio;

//레이져
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

/* 정점 셰이더 :  /* 
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float3 startPos = g_WorldMatrix[3].xyz;

	// 레이저의 방향 (월드 행렬의 Look 방향)
	float3 laserDir = normalize(g_WorldMatrix[2].xyz);

	// 전체 레이저 길이 (Look 방향 벡터의 스케일)
	float totalLength = length(g_WorldMatrix[2].xyz);

	// 정점의 로컬 위치를 월드 위치로 변환
	float3 worldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;

	// 시작 지점에서 정점까지의 거리 계산
	float distanceFromStart = dot(worldPos - startPos, laserDir);

	// 거리에 따른 스케일 계산 (0.1에서 시작하여 1.0까지, 5% 거리에서 최대)
	float rawScale = saturate(distanceFromStart / (totalLength * 0.05f));

	// 비선형 스케일 적용 (더 빠르게 커지도록)
	float scale = lerp(0.2, 1.0, pow(rawScale, 0.2));

	// x와 y 좌표에 스케일 적용
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
			discard; // 픽셀 폐기
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
			discard; // 픽셀 폐기
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
			discard; // 픽셀 폐기
		}

	}
	Out.vColor = Color;
	return Out;
}




technique11 DefaultTechnique
{
	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
	pass LazerPass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
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

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
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

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
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

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Distortion();
	}


	
}

