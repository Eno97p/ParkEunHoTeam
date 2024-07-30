
#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_BrushTexture;
texture2D	g_MaskTexture; 
texture2D	g_DiffuseTexture[3];
texture2D	g_NormalTexture[3];

vector		g_vBrushPos = float4(30.f, 0.0f, 20.f, 1.f);
float		g_fBrushRange = 5.f;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vWorldPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}


struct PS_IN
{
    float4      vPosition : SV_POSITION;
    float4      vNormal : NORMAL;
    float2      vTexcoord : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
    float4      vWorldPos : TEXCOORD2;
};

struct PS_OUT
{
    vector      vDiffuse : SV_TARGET0;
    vector      vNormal : SV_TARGET1;
    vector      vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    // 노멀값을 이용한 경사 계산
    float3 worldNormal = normalize(In.vNormal.xyz);
    float slope = 1.0f - dot(worldNormal, float3(0, 1, 0));

    // 텍스처 샘플링
    vector vLowDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vMidDiffuse = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vHighDiffuse = g_DiffuseTexture[2].Sample(LinearSampler, In.vTexcoord * 30.f);

    vector vLowNormal = g_NormalTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vMidNormal = g_NormalTexture[1].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vHighNormal = g_NormalTexture[2].Sample(LinearSampler, In.vTexcoord * 30.f);

    // 경사도에 따른 블렌딩
    vector vMtrlDiffuse, vNormalDesc;
    if (slope < 0.3) // 낮은 경사
    {
        float blend = smoothstep(0.0, 0.3, slope);
        vMtrlDiffuse = lerp(vLowDiffuse, vMidDiffuse, blend);
        vNormalDesc = lerp(vLowNormal, vMidNormal, blend);
    }
    else if (slope < 0.7) // 중간 경사
    {
        float blend = smoothstep(0.3, 0.7, slope);
        vMtrlDiffuse = lerp(vMidDiffuse, vHighDiffuse, blend);
        vNormalDesc = lerp(vMidNormal, vHighNormal, blend);
    }
    else // 높은 경사
    {
        vMtrlDiffuse = vHighDiffuse;
        vNormalDesc = vHighNormal;
    }

    // 브러시 적용 (기존 코드 유지)
    vector vBrush = float4(0.0f, 0.f, 0.f, 0.f);
    if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
        g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
    {
        float2 vTexcoord = (float2)0.f;
        vTexcoord.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
        vTexcoord.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);
        vBrush = g_BrushTexture.Sample(LinearSampler, vTexcoord);
    }

    // 최종 디퓨즈 색상 계산
    Out.vDiffuse = vMtrlDiffuse + vBrush * 0.5f;

    // 노말 계산
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    Out.vNormal = vector(worldNormal * 0.5f + 0.5f, 0.f);

    // 깊이 정보
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);

    return Out;
}


PS_OUT PS_WIRE(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    // 노멀값을 이용한 경사 계산
    float3 worldNormal = normalize(In.vNormal.xyz);
    float slope = 1.0f - dot(worldNormal, float3(0, 1, 0));

    // 텍스처 샘플링
    vector vLowDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vMidDiffuse = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vHighDiffuse = g_DiffuseTexture[2].Sample(LinearSampler, In.vTexcoord * 30.f);

    vector vLowNormal = g_NormalTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vMidNormal = g_NormalTexture[1].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vHighNormal = g_NormalTexture[2].Sample(LinearSampler, In.vTexcoord * 30.f);

    // 경사도에 따른 블렌딩
    vector vMtrlDiffuse, vNormalDesc;
    if (slope < 0.3) // 낮은 경사
    {
        float blend = smoothstep(0.0, 0.3, slope);
        vMtrlDiffuse = lerp(vLowDiffuse, vMidDiffuse, blend);
        vNormalDesc = lerp(vLowNormal, vMidNormal, blend);
    }
    else if (slope < 0.7) // 중간 경사
    {
        float blend = smoothstep(0.3, 0.7, slope);
        vMtrlDiffuse = lerp(vMidDiffuse, vHighDiffuse, blend);
        vNormalDesc = lerp(vMidNormal, vHighNormal, blend);
    }
    else // 높은 경사
    {
        vMtrlDiffuse = vHighDiffuse;
        vNormalDesc = vHighNormal;
    }

    // 브러시 적용 (기존 코드 유지)
    vector vBrush = float4(0.0f, 0.f, 0.f, 0.f);
    if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
        g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
    {
        float2 vTexcoord = (float2)0.f;
        vTexcoord.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
        vTexcoord.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);
        vBrush = g_BrushTexture.Sample(LinearSampler, vTexcoord);
    }

    // 최종 디퓨즈 색상 계산
   // Out.vDiffuse = vMtrlDiffuse + vBrush * 0.5f;
    Out.vDiffuse = vector(1.f, 0.f, 0.f, 1.f);

    // 노말 계산
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    Out.vNormal = vector(worldNormal * 0.5f + 0.5f, 0.f);

    // 깊이 정보
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);

    return Out;
}


technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

		pass WireFrame
	{
		SetRasterizerState(RS_Wireframe);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}

