#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix		g_ViewMatrixInv, g_ProjMatrixInv;	// Shader에는 역행렬 구해주는 함수가 없으므로 월드, 뷰, 투영 행렬을 가지고 있더라도 역행렬 연산해서 활용 불가능 -> 외부에서 받아와야 함
												//(직교 행렬의 경우 Transpos(전치)로 역행렬 구하기 가능. 단, 위치는 직교의 개념이 없으므로 4x4가 아니라 3x3)
matrix		g_LightViewMatrix, g_LightProjMatrix; // 그림자 연산을 위한 광원 기준 뷰, 투영 행렬
vector		g_vLightDir;
vector		g_vLightPos;
float		g_fLightRange;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector		g_vCamPosition;

texture2D	g_Texture;
texture2D	g_NormalTexture;
texture2D	g_DiffuseTexture;
texture2D	g_ShadeTexture;
texture2D	g_DepthTexture;
texture2D	g_LightDepthTexture;
texture2D	g_SpecularTexture;

// 블러를 위한 변수들
texture2D	g_EffectTexture;
texture2D	g_BlurTexture;
texture2D	g_ResultTexture;

// 주변 색 가져올 때 해당 값을 곱해서 가져옴(몇 퍼센트를 먹일 지. 원하는 블러 형태에 따라 값 다름에 유의. 조정해서 사용할 것)
// 픽셀 하나당 13개씩 섞는 것(더 커지거나 작아지거나 조절 가능)
static const float g_fWeight[13] = {
	0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044 // 가우시안 배열
	/*0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1, // 코사인(가운데가 1이므로 원본 색상 그대로 가져오는 것)
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561*/
};

static const float g_fTotal = 2.f;

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

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

struct PS_OUT_LIGHT
{
	vector		vShade : SV_TARGET0;
	vector		vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In) // 방향성 광원 셰이더
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));

	//Out.vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)), 0.f);

	// 카툰 렌더링 시도
	float NdotL = max(0, dot(normalize(vNormal), normalize(g_vLightDir)));
	NdotL = ceil(NdotL * 3) / 3; // step
	float toonColor = 0.f;
	if (NdotL < 0.25f)
		toonColor = 0.3f;
	else if (NdotL < 0.75f)
		toonColor = 0.7f;
	else
		toonColor = 1.f;



	// 림라이트
	float3 viewDir = normalize(g_vCamPosition - In.vPosition);
	float3 RimColor = float3(0.f, 0.f, 0.f); // 임의로 선언
	RimColor = float3(-2.f, -2.f, -2.f); // 다른 조명에 영향 받지 않도록 -2    float3(-2.f, -2.f, -2.f);
	
	int rimpower = 5.f;
	float rim = saturate(dot(normalize(vNormal), normalize(viewDir))); // 카메라 방향과 노말 벡터를 내적

	if (rim > 0.9) // 0.3
		rim = 1; // 내적 결과가 특정 값보다 크면 1로 고정(없애주는 것)	>>> 여기 아예 안 들어오는 듯
	else
		rim = -1; // 특정 값보다 작은 애들만 

	vector vEmiss = float4(pow(1 - rim, rimpower) * RimColor, 1.f);



	//Out.vShade = g_vLightDiffuse *
	//saturate(NdotL + (g_vLightAmbient * g_vMtrlAmbient) * toonColor);
	Out.vShade = g_vLightDiffuse *
		saturate(NdotL + (g_vLightAmbient * g_vMtrlAmbient) * toonColor);

	//Out.vShade *= vEmiss;


	// vWorldPos : 화면에 그려지고 있는 픽셀들의 실제 월드 위치
	// 렌더 타겟에 객체들 그릴 때 픽셀의 월드 위치를 저장하는 방법 (월드 공간은 범위가 무제한이라는 조건 때문에 저장 힘듦)
	// 현재 상황에서 픽셀의 투영 위치(x, y)는 구할 수 있음 -> z가 없기 때문에 월드까지의 역변환 난해 -> 투영 z(0~1), 뷰스페이스의 픽셀 z(near~far)를 받아옴(무제한X)

	vector	vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);

	vector vWorldPos;

	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; // vDepthDesc.x : 투영 공간 상의 z가 담겨 있음
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f); // 얘도 far로 해야 하지 않을까 싶긴 함

	vWorldPos = mul(vWorldPos, g_ProjMatrixInv); // 뷰 스페이스 상 위치
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv); // 월드 스페이스 상 위치

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f);

	return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In) // 점 광원 셰이더
{
	PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
	vector vWorldPos;

	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

	/* 뷰스페이스 상의 위치를 구한다. */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구한다. */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLightDir = vWorldPos - g_vLightPos;
	float		fDistance = length(vLightDir);

	float		fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;

	// 카툰 렌더링 시도
	float NdotL = max(0, dot(normalize(vNormal), normalize(g_vLightDir)));
	NdotL = ceil(NdotL * 3) / 3; // step
	float toonColor = 0.f;
	if (NdotL < 0.25f)
		toonColor = 0.5f;
	else if (NdotL < 0.75f)
		toonColor = 0.7f;
	else
		toonColor = 1.f;

	//Out.vShade = g_vLightDiffuse *
	//	saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

	Out.vShade = g_vLightDiffuse *
		saturate(NdotL + (g_vLightAmbient * g_vMtrlAmbient) * toonColor) * fAtt;

	vector	vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	vector	vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f) * fAtt;

	return Out;
}

// 블러 적용 후 찐 최종
struct PS_OUT_FINAL
{
	vector vResult : SV_TARGET0;
};

PS_OUT_FINAL PS_MAIN_DEFERRED_RESULT(PS_IN In) // 후처리 셰이더 -> 이 반환 값이 PS_OUT_FINAL이 되어야 하는 거 같음 !!!! 기존 : PS_OUT (근데 그러면 Color는?)
{
	//PS_OUT		Out = (PS_OUT)0;
	PS_OUT_FINAL		Out = (PS_OUT_FINAL)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (0.f == vDiffuse.a)
		discard;

	vector		vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
	vector		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

	//Out.vColor = vDiffuse * vShade + vSpecular * 0.3f; // 기존 코드
	//Out.vColor = vDiffuse * vShade; // Specular 없앤 최종 코드(블러 넣기 전)
	Out.vResult = vDiffuse * vShade;


	// 그림자 연산
	vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
	vector vWorldPos;

	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector	vLightPos = mul(vWorldPos, g_LightViewMatrix);
	vLightPos = mul(vLightPos, g_LightProjMatrix);

	float2		vTexcoord;
	vTexcoord.x = (vLightPos.x / vLightPos.w) * 0.5f + 0.5f;
	vTexcoord.y = (vLightPos.y / vLightPos.w) * -0.5f + 0.5f;

	vector	vLightDepthDesc = g_LightDepthTexture.Sample(PointSampler, vTexcoord);

	float		fLightOldDepth = vLightDepthDesc.x * 1000.f;

	/*if (fLightOldDepth + 0.1f < vLightPos.w)
		Out.vColor = vector(Out.vColor.rgb * 0.5f, 1.f);*/ // Color가 어디로 가야하지? 잠깐만 주석 처리
	if (fLightOldDepth + 0.1f < vLightPos.w)
		Out.vResult = vector(Out.vResult.rgb * 0.5f, 1.f); // Color를 Result로 바꿔봄

	return Out;
}

// 블러를 먹이는 픽셀 쉐이더
PS_OUT PS_MAIN_RESULT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector		vResult = g_ResultTexture.Sample(LinearSampler, In.vTexcoord);

	vector		vBlur = g_BlurTexture.Sample(LinearSampler, In.vTexcoord);
	vector		vEffect = g_EffectTexture.Sample(LinearSampler, In.vTexcoord);

	Out.vColor = vResult + vBlur + vEffect;

	return Out;
}

// 블러 쉐이더
float4 Blur_X(float2 vTexCoord)
{
	float4		vOut = (float4)0;

	float2		vUV = (float2)0;

	for (int i = -6; i < 7; ++i)
	{
		vUV = vTexCoord + float2(1.f / 1280.f * i, 0);
		vOut += g_fWeight[6 + i] * g_EffectTexture.Sample(LinearSampler, vUV);
	}

	vOut /= g_fTotal;

	return vOut;
}

float4 Blur_Y(float2 vTexCoord)
{
	float4		vOut = (float4)0;

	float2		vUV = (float2)0;

	for (int i = -6; i < 7; ++i)

	{
		vUV = vTexCoord + float2(0, 1.f / 720.f * i);
		vOut += g_fWeight[6 + i] * g_EffectTexture.Sample(LinearSampler, vUV);
	}

	vOut /= g_fTotal;

	return vOut;
}

PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = Blur_X(In.vTexcoord);

	return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = Blur_Y(In.vTexcoord);

	return Out;
}

technique11 DefaultTechnique
{
	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
	pass DebugRender
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
	}

	pass Deferred_Result
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_RESULT();
	}

	//4 
	pass Blur_X
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
	}
	pass Blur_Y
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
	}

	pass Result
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RESULT();
	}

}

