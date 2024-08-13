#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_NormalTexture1;
texture2D	g_NormalTexture2;
texture2D	g_CausticTexture;
texture2D	g_SpecularTexture;

texture2D g_RoughnessTexture;
texture2D g_MetalicTexture;
texture2D g_EmissiveTexture;
//FOR DISSOLVE
texture2D	g_NoiseTexture;
float		g_fAccTime;
float		g_fRoughness;
float		g_fFresnelStrength;
float		g_fNormalStrength0;
float		g_fNormalStrength1;
float		g_fNormalStrength2;
float		g_fCausticStrength;
float4		g_vSkyColor;
vector		g_vCamPosition;

TextureCube g_ReflectionTexture; // 반사 텍스처 큐브맵

float4		g_vLightDiffuse = float4(0.5f, 0.f, 1.f, 1.f);
float4		g_vLightAmbient = float4(1.f, 1.f, 1.f, 1.f);
float4		g_vLightSpecular = float4(1.f, 1.f, 1.f, 1.f);
float4		g_vLightDir;
float4		g_vLightPosition;
float		 g_fLightRange;
float		 g_fWaterDepth;
float		 g_fWaterAlpha;
float4		g_vMtrlAmbient = float4(0.4f, 0.4f, 0.4f, 1.f); /* 객체당 엠비언트(모든 픽셀이 같은 엠비언트로 연산한다.) */
float4		g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);
//bloom
float g_fBloomThreshold;
float g_fBloomIntensity;
unsigned int g_Red;
unsigned int g_Test;


bool g_bDiffuse = false;
bool g_bNormal = false;
bool g_bSpecular = false;
bool g_bOpacity = false;
bool g_bEmissive = false;
bool g_bRoughness = false;
bool g_bMetalic = false;

float g_fFlowSpeed = 10.f;


//바람 시뮬레이션용
float g_fTime;
float3 g_vWindDirection;
float g_fWindStrength;
float3 g_LeafCol;


//for Card
uint g_TextureNum;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vLocalPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
	float4		vWorldPos : TEXCOORD3;

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

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;
	Out.vLocalPos = float4(In.vPosition, 1.f);
	Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix));
	Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

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

	//float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	//matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
	//	g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
	//	g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
	//	g_BoneMatrices[In.vBlendIndices.w] * fWeightW;



	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;

	return Out;

}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vLocalPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
	float4		vWorldPos : TEXCOORD3;
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
	PS_OUT Out = (PS_OUT)0;
	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a < 0.1f)
		discard;
	vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

	float3 vNormal;
	if (g_bNormal)
	{
		vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	}
	else
	{
		vNormal = In.vNormal.xyz * 2.f - 1.f;
	}

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);
	if (g_bDiffuse) Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
	if (g_bSpecular) Out.vSpecular = vSpecular;

	// 디퓨즈 맵을 사용하여 러프니스 계산
	float intensity = dot(vDiffuse.rgb, float3(0.299, 0.587, 0.114));
	float calculatedRoughness = 1.0 - intensity;

	// 아티스트 조정 가능한 파라미터 (셰이더 상수로 설정 가능)
	float baseRoughness = 0.8;
	float roughnessContrast = 2.0;

	// 최종 러프니스 계산
	float finalRoughness = saturate(baseRoughness + (calculatedRoughness - 0.5) * roughnessContrast);

	vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
	vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bEmissive) Out.vEmissive = vEmissive;
	if (g_bRoughness) Out.vRoughness = 0.01f/*vector(finalRoughness, finalRoughness, finalRoughness, 1.0)*/;
	if (g_bMetalic) Out.vMetalic = 1.f - vMetalic;

	//if (g_Red == g_Test)
	//{
	//	Out.vDiffuse = vector(1.f, 0.f, 0.f, 1.f);
	//}


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

PS_OUT PS_DISSOLVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vNoise.r < g_fAccTime)
	{
		discard;
	}

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a < 0.1f)
		discard;

	vector		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);
	


	Out.vDiffuse = vDiffuse/* * 0.9f*/;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
	Out.vSpecular = g_vMtrlSpecular;

	//Out.vEmissive = 0.5f;
	/*if (g_Red == 0 || g_Red == 29)
	{
		Out.vDiffuse = vector(1.f, 0.f, 0.f, 1.f);
	}*/
	return Out;
}


PS_OUT PS_WIREFRAME(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse = vector(0.f, 0.f, 1.f, 1.f);

	return Out;
}

struct PS_OUT_AB
{
	vector		vColor : SV_TARGET0;

};

PS_OUT_AB PS_ALPHABLEND(PS_IN In)
{
	PS_OUT_AB Out = (PS_OUT_AB)0;
	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a < 0.1f)
		discard;

	Out.vColor = vDiffuse;
	Out.vColor.a = 0.2f;

	return Out;

}

struct PS_OUT_BLOOM
{
	vector		vColor : SV_TARGET0;

};

PS_OUT_BLOOM PS_BLOOM(PS_IN In)
{
	PS_OUT_BLOOM Out = (PS_OUT_BLOOM)0;

	// 노멀 맵 샘플링 및 블렌딩
	float3 vNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz * 2.0f - 1.0f;
	float3x3 TBN = float3x3(normalize(In.vTangent.xyz), normalize(In.vBinormal.xyz), normalize(In.vNormal.xyz));
	vNormal = mul(vNormal, TBN);

	// 포인트 라이트 계산
	float3 lightDir = g_vLightPosition.xyz - In.vWorldPos.xyz;
	float fDistance = length(lightDir);
	lightDir = normalize(lightDir);
	float fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;

	// 뷰 방향 계산
	float3 viewDir = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);

	// PBR 계산을 위한 변수들
	float roughness = g_fRoughness;
	float metallic = 0.02;
	float3 halfVec = normalize(lightDir + viewDir);
	float NdotV = max(dot(vNormal, viewDir), 0.001);
	float NdotL = max(dot(vNormal, lightDir), 0.001);
	float NdotH = max(dot(vNormal, halfVec), 0.001);
	float HdotV = max(dot(halfVec, viewDir), 0.001);

	// PBR 스페큘러 계산
	float a = roughness * roughness;
	float D = a / (3.141592 * pow(NdotH * NdotH * (a - 1.0) + 1.0, 2.0));
	float F0 = lerp(0.04, 1.0, metallic);
	float F = F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
	float specular = D * F * fAtt;

	// 물의 특성을 고려한 추가적인 스페큘러
	float waterSpecular = pow(max(dot(reflect(-lightDir, vNormal), viewDir), 0.0), 256.0) * 0.5;

	// 프레넬 효과
	float fresnel = pow(1.0 - max(dot(vNormal, viewDir), 0.0), 5.0);
	fresnel = saturate(fresnel * g_fFresnelStrength);

	// 스페큘러 블룸 계산
	float3 specularBloom = (specular + waterSpecular) * g_vLightSpecular.rgb * g_fBloomIntensity * 5.0; // 스페큘러 강조

	// 나머지 부분의 블룸 계산
	float3 otherBloom = fresnel * g_vLightSpecular.rgb * g_fBloomIntensity;

	// 최종 블룸 색상 계산
	float3 bloomColor = specularBloom + otherBloom;

	// 스페큘러 강도에 따른 알파값 계산
	float specularIntensity = specular + waterSpecular;
	float specularAlpha = saturate(specularIntensity * 10.0); // 스페큘러에 대한 알파값 강조

	// 전체적인 휘도에 따른 알파값 계산
	float luminance = dot(bloomColor, float3(0.299, 0.587, 0.114));
	float overallAlpha = saturate(luminance);

	// 최종 알파값 (스페큘러 알파와 전체 알파 중 큰 값 선택)
	float finalAlpha = max(specularAlpha, overallAlpha);

	// 스페큘러 값에 따라 RGB 2배 증가
	if (specularIntensity > g_fBloomThreshold)
	{
		bloomColor *= 2.0;
	}

	// 최종 출력
	Out.vColor = float4(bloomColor, finalAlpha);
	return Out;
}
PS_OUT PS_CARD(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	// 텍스처 좌표 조정
	float2 adjustedTexcoord = In.vTexcoord;

	// g_TextureNum에 따라 텍스처의 다른 부분 선택
	float startY, endY;
	if (g_TextureNum == 0)
	{
		startY = 0.0f;
		endY = 0.37f;
	}
	else if (g_TextureNum == 1)
	{
		startY = 0.37f;
		endY = 0.68f;
	}
	else // g_TextureNum == 2 또는 그 외의 경우
	{
		startY = 0.68f;
		endY = 1.0f;
	}

	// 선택된 범위 내에서 텍스처 좌표 조정
	adjustedTexcoord.y = startY + (endY - startY) * adjustedTexcoord.y;

	vector vColor = g_DiffuseTexture.Sample(PointSampler, adjustedTexcoord);

	if (vColor.a < 0.1f)
		discard;

	if (g_bDiffuse)
	{
		Out.vDiffuse = vColor;
	}

	float3 vNormal;
	vNormal = In.vNormal.xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);

	return Out;
}

PS_OUT_BLOOM PS_MIRROR(PS_IN In)
{
	PS_OUT_BLOOM Out = (PS_OUT_BLOOM)0;

	vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	Out.vColor = vColor;

	return Out;
}

PS_OUT_BLOOM PS_LAGOON(PS_IN In)
{
	PS_OUT_BLOOM Out = (PS_OUT_BLOOM)0;

	// 노멀 맵 샘플링 및 블렌딩 (기존 코드 유지)
	float2 flowTexCoord0 = In.vTexcoord + float2(g_fAccTime * g_fFlowSpeed * 0.05, g_fAccTime * g_fFlowSpeed * 0.07);
	float2 flowTexCoord1 = In.vTexcoord + float2(g_fAccTime * g_fFlowSpeed * 0.03, -g_fAccTime * g_fFlowSpeed * 0.06);
	float2 flowTexCoord2 = In.vTexcoord + float2(-g_fAccTime * g_fFlowSpeed * 0.04, g_fAccTime * g_fFlowSpeed * 0.08);
	float3 vNormal0 = g_NormalTexture.Sample(LinearSampler, flowTexCoord0).xyz * 2.0f - 1.0f;
	float3 vNormal1 = g_NormalTexture1.Sample(LinearSampler, flowTexCoord1).xyz * 2.0f - 1.0f;
	float3 vNormal2 = g_NormalTexture2.Sample(LinearSampler, flowTexCoord2).xyz * 2.0f - 1.0f;
	vNormal0 *= g_fNormalStrength0;
	vNormal1 *= g_fNormalStrength1;
	vNormal2 *= g_fNormalStrength2;
	float3 vNormal = normalize(vNormal0 + vNormal1 + vNormal2);
	float3x3 TBN = float3x3(normalize(In.vTangent.xyz), normalize(In.vBinormal.xyz), normalize(In.vNormal.xyz));
	vNormal = mul(vNormal, TBN);

	// 디퓨즈 텍스처 샘플링
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	// Caustic 효과 적용
	float2 causticTexCoord = In.vTexcoord * 0.5 + float2(g_fAccTime * 0.03, g_fAccTime * 0.02);
	float3 caustic = g_CausticTexture.Sample(LinearSampler, causticTexCoord).rgb;

	// 포인트 라이트 계산
	float3 lightDir = g_vLightPosition.xyz - In.vWorldPos.xyz;
	float fDistance = length(lightDir);
	lightDir = normalize(lightDir);
	float fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;

	// 뷰 방향 계산
	float3 viewDir = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);

	// PBR 계산을 위한 변수들
	float roughness = g_fRoughness;
	float metallic = 0.02; // 물은 약간의 금속성을 가질 수 있습니다.

	float3 halfVec = normalize(lightDir + viewDir);
	float NdotV = max(dot(vNormal, viewDir), 0.001);
	float NdotL = max(dot(vNormal, lightDir), 0.001);
	float NdotH = max(dot(vNormal, halfVec), 0.001);
	float HdotV = max(dot(halfVec, viewDir), 0.001);

	// PBR 공식 적용
	float a = roughness * roughness;
	float a2 = a * a;
	float D = a2 / (3.141592 * pow(NdotH * NdotH * (a2 - 1.0) + 1.0, 2.0));

	float G1 = 2.0 * NdotH / (NdotH + sqrt(a2 + (1.0 - a2) * NdotH * NdotH));
	float G = G1 * G1;

	float F0 = lerp(0.04, 1.0, metallic);
	float F = F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);

	float specular = (D * F * G) / (4.0 * NdotV * NdotL + 0.001);

	// 최종 라이팅 계산
	float3 ambientLight = g_vLightAmbient.rgb * g_vMtrlAmbient.rgb;
	float3 diffuseLight = g_vLightDiffuse.rgb * NdotL * (1.0 - metallic);
	float3 specularLight = g_vLightSpecular.rgb * specular;

	float3 finalLight = (ambientLight + (diffuseLight + specularLight) * fAtt);
	float3 finalColor = vMtrlDiffuse.rgb * finalLight;

	// 물의 특성을 고려한 추가적인 스페큘러
	float waterSpecular = pow(max(dot(reflect(-lightDir, vNormal), viewDir), 0.0), 256.0) * 0.5;
	finalColor += waterSpecular * g_vLightSpecular.rgb;

	// 깊이에 따른 색상 변화
	float depthFactor = saturate(g_fWaterDepth / 5.0f);
	float3 deepColor = float3(0.0f, 0.2f, 0.3f);  // 깊은 물의 색상
	finalColor = lerp(finalColor, deepColor, depthFactor);

	// Caustic 효과를 최종 색상에 적용
	finalColor *= (1 + caustic * g_fCausticStrength);

	// 프레넬 효과 (선택적)
	float fresnel = pow(1.0 - max(dot(vNormal, viewDir), 0.0), 5.0);
	fresnel = saturate(fresnel * g_fFresnelStrength);
	finalColor = lerp(finalColor, g_vLightSpecular.rgb, fresnel);

	// 최종 색상을 Out.vColor에 저장
	Out.vColor = float4(finalColor, g_fWaterAlpha);

	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass_0
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

	pass LightDepth_1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_LIGHTDEPTH();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
	}

		pass Dissolve_2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVE();
	}

		pass WireFrame_3
	{
		SetRasterizerState(RS_Wireframe);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WIREFRAME();
	}

		pass AlphaBlend_4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHABLEND();

	}

		pass Bloom_5
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOOM();
	}

	pass Card_6
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_CARD();
	}

	pass Lagoon_7
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_LAGOON();
	}

		pass Mirror
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MIRROR();
	}
}

