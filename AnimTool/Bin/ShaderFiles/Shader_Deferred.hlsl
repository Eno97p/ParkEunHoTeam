#include "Engine_Shader_Defines.hlsli"

/* ������Ʈ ���̺�(������̺�) */
matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix      g_ViewMatrixInv, g_ProjMatrixInv;
matrix      g_LightViewMatrix, g_LightProjMatrix;
vector      g_vLightDir;
vector      g_vLightPos;
float      g_fLightRange;

float      g_fInnerAngle;
float      g_fOuterAngle;

vector      g_vLightDiffuse;
vector      g_vLightAmbient;
vector      g_vLightSpecular;

//�̹ο� �߰� 240621 1638
float      g_fShadowThreshold;

vector      g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector      g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector      g_vCamPosition;

texture2D   g_Texture;
texture2D   g_NormalTexture;
texture2D   g_DiffuseTexture;
texture2D   g_ShadeTexture;
texture2D   g_DepthTexture;
texture2D   g_LightDepthTexture;
texture2D   g_SpecularTexture;

texture2D   g_EmissiveTexture;
texture2D   g_RoughnessTexture;
texture2D   g_MetalicTexture;

texture2D   g_SpecularMapTexture;

texture2D   g_EffectTexture;
texture2D   g_DistortionTexture;

texture2D   g_LUTTexture;
texture2D   g_ResultTexture;
float      g_Time = 0.f;
float      g_Value = 0.1f;

float g_fTexW = 1280.0f;
float g_fTexH = 720.0f;
int     g_BlurNum = 1;

static const float g_fWeight[13] =
{
   0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044
   //0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1,
   //0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

static const float g_fWeight2[52] =
{
   0.0001, 0.0003, 0.0007, 0.0012, 0.0020, 0.0031, 0.0044, 0.0062, 0.0084, 0.0111, 0.0143, 0.0175,
0.0220, 0.0271, 0.0329, 0.0396, 0.0471, 0.0540, 0.0630, 0.0731, 0.0844, 0.0969, 0.1104, 0.1295,
0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.1104, 0.0969, 0.0844, 0.0731, 0.0630, 0.0540,
0.0471, 0.0396, 0.0329, 0.0271, 0.0220, 0.0175, 0.0143, 0.0111, 0.0084, 0.0062, 0.0044, 0.0031,
0.0020, 0.0012, 0.0007, 0.0003
};


static const float g_fWeight3[260] =
{
   0.0000, 0.0000, 0.0000, 0.0000, 0.0001, 0.0001, 0.0001, 0.0001, 0.0002, 0.0002,
   0.0003, 0.0003, 0.0004, 0.0005, 0.0006, 0.0007, 0.0008, 0.0009, 0.0011, 0.0012,
   0.0014, 0.0016, 0.0018, 0.0021, 0.0023, 0.0026, 0.0029, 0.0033, 0.0037, 0.0041,
   0.0045, 0.0050, 0.0055, 0.0061, 0.0067, 0.0073, 0.0080, 0.0087, 0.0095, 0.0103,
   0.0112, 0.0121, 0.0131, 0.0142, 0.0153, 0.0165, 0.0178, 0.0191, 0.0205, 0.0220,
   0.0236, 0.0252, 0.0269, 0.0287, 0.0306, 0.0326, 0.0347, 0.0368, 0.0391, 0.0414,
   0.0439, 0.0464, 0.0491, 0.0518, 0.0547, 0.0576, 0.0607, 0.0638, 0.0671, 0.0704,
   0.0739, 0.0774, 0.0811, 0.0849, 0.0887, 0.0927, 0.0968, 0.1009, 0.1052, 0.1095,
   0.1140, 0.1185, 0.1231, 0.1278, 0.1326, 0.1375, 0.1424, 0.1475, 0.1526, 0.1578,
   0.1631, 0.1684, 0.1738, 0.1793, 0.1848, 0.1904, 0.1960, 0.2017, 0.2074, 0.2132,
   0.2190, 0.2249, 0.2308, 0.2367, 0.2426, 0.2486, 0.2546, 0.2606, 0.2666, 0.2726,
   0.2786, 0.2846, 0.2906, 0.2965, 0.3025, 0.3084, 0.3143, 0.3201, 0.3259, 0.3316,
   0.3373, 0.3429, 0.3485, 0.3540, 0.3594, 0.3647, 0.3699, 0.3751, 0.3801, 0.3851,
   0.3899, 0.3947, 0.3989, 0.3989, 0.3947, 0.3899, 0.3851, 0.3801, 0.3751, 0.3699,
   0.3647, 0.3594, 0.3540, 0.3485, 0.3429, 0.3373, 0.3316, 0.3259, 0.3201, 0.3143,
   0.3084, 0.3025, 0.2965, 0.2906, 0.2846, 0.2786, 0.2726, 0.2666, 0.2606, 0.2546,
   0.2486, 0.2426, 0.2367, 0.2308, 0.2249, 0.2190, 0.2132, 0.2074, 0.2017, 0.1960,
   0.1904, 0.1848, 0.1793, 0.1738, 0.1684, 0.1631, 0.1578, 0.1526, 0.1475, 0.1424,
   0.1375, 0.1326, 0.1278, 0.1231, 0.1185, 0.1140, 0.1095, 0.1052, 0.1009, 0.0968,
   0.0927, 0.0887, 0.0849, 0.0811, 0.0774, 0.0739, 0.0704, 0.0671, 0.0638, 0.0607,
   0.0576, 0.0547, 0.0518, 0.0491, 0.0464, 0.0439, 0.0414, 0.0391, 0.0368, 0.0347,
   0.0326, 0.0306, 0.0287, 0.0269, 0.0252, 0.0236, 0.0220, 0.0205, 0.0191, 0.0178,
   0.0165, 0.0153, 0.0142, 0.0131, 0.0121, 0.0112, 0.0103, 0.0095, 0.0087, 0.0080,
   0.0073, 0.0067, 0.0061, 0.0055, 0.0050, 0.0045, 0.0041, 0.0037, 0.0033, 0.0029,
   0.0026, 0.0023, 0.0021, 0.0018, 0.0016, 0.0014, 0.0012, 0.0011, 0.0009, 0.0008,
   0.0007, 0.0006, 0.0005, 0.0004, 0.0003, 0.0003, 0.0002, 0.0002, 0.0001, 0.0001
};


static const float g_fTotal = 1.5f;

struct VS_IN
{
    float3      vPosition : POSITION;
    float2      vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4      vPosition : SV_POSITION;
    float2      vTexcoord : TEXCOORD0;
};

/* ���� ���̴� :  /*
/* 1. ������ ��ġ ��ȯ(����, ��, ����).*/
/* 2. ������ ���������� �����Ѵ�. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT      Out = (VS_OUT)0;

    matrix      matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

/* TriangleList�� ��� : ���� ������ �޾Ƽ� w�����⸦ �� ������ ���ؼ� �����Ѵ�. */
/* ����Ʈ(��������ǥ��) ��ȯ. */
/* �����Ͷ����� : �������� �ѷ��׿��� �ȼ��� ������, ������ ���������Ͽ� �����. -> �ȼ��� ���������!!!!!!!!!!!! */
struct PS_IN
{
    float4      vPosition : SV_POSITION;
    float2      vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector      vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);


    return Out;
}

struct PS_OUT_LIGHT
{
    vector      vShade : SV_TARGET0;
    vector      vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);

    // ��� ���Ͱ� ���������� ���ǰ� �ִ��� Ȯ��
    if (length(vNormal.xyz) == 0)
        return Out;

    // ���� ���
    float3 lightDir = normalize(g_vLightDir);
    float3 normal = normalize(vNormal.xyz);
    float3 lightAmbient = g_vLightAmbient * g_vMtrlAmbient;
    float3 lightDiffuse = g_vLightDiffuse * saturate(max(dot(-lightDir, normal), 0.f));
    Out.vShade = float4(lightDiffuse + lightAmbient, 1.f);

    // ���� �ؽ�ó ���ø�
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);

    // ���� ��ǥ ���
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;
    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    // ����ŧ�� ���
    vector vReflect = float4(reflect(-lightDir, normal), 1.f);
    vector vLook = normalize(vWorldPos - g_vCamPosition);

    vector vSpecularMap = g_SpecularMapTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
    vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    float roughness = vRoughness.r; // �����Ͻ� ��

    float NdotV = max(dot(normal, vLook), 0.f);
    float NdotL = max(dot(normal, lightDir), 0.f);
    float a = roughness * roughness;
    float a2 = a * a;

    float D = a2 / (3.14f * pow(NdotL * NdotL * (a2 - 1.f) + 1.f, 2.f));
    float F = pow(1.f - max(dot(vReflect, vLook), 0.f), 5.f);
    float G = min(1.f, min((2.f * NdotV * NdotL) / (NdotV + NdotL), (2.f * NdotL * (1.f - roughness)) / (NdotV * NdotL)));

    float specular = (D * F * G) / (4.f * NdotV * NdotL + 0.001f);

    float metallic = vMetalic.r;

    float kS = metallic * specular + (1.f - metallic) * 0.04f;
    float kD = (1.f - kS) * (1.f - metallic);

    Out.vSpecular = kS * pow(max(dot(-vReflect, vLook), 0.f), 5.f);

    return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT      Out = (PS_OUT_LIGHT)0;

    vector   vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector   vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    vector   vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector  vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

    /* �佺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    /* ���彺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);


    vector      vLightDir = vWorldPos - g_vLightPos;
    float      fDistance = length(vLightDir);

    float      fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;

    Out.vShade = g_vLightDiffuse *
        saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;


    /* vWorldPos:ȭ�鿡 �׷����� �ִ� �ȼ����� ���� ���� ��ġ�� �޾ƿ;��ϳ�.  */
    /* 1. ����Ÿ�ٿ� ��ü���� �׸��� �ȼ��� ������ġ�� �����ϴ� ���.(���� �������̶�� ���Ƕ����� ������ �����) */
    /* 2. ���� ��Ȳ���� �ȼ��� ������ġ(x, y)���� �������ϴ� �۾��� ���� -> z�� ���⶧���� ��������� ����ȯ�� �����.-> ���� z( 0 ~ 1), ViewSpace`s Pixel`s Z��(near ~ far) �޾ƿ���.(�������̾ƴϴ�.)  */
    vector   vReflect = reflect(normalize(vLightDir), normalize(vNormal));
    vector   vLook = vWorldPos - g_vCamPosition;

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f) * fAtt;

    return Out;
}

//SPOTLIGHT ���� ���� �ڵ�
PS_OUT_LIGHT PS_MAIN_LIGHT_SPOTLIGHT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);
    /* �佺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    /* ���彺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    vector vLightDir = vWorldPos - g_vLightPos;


    float fDistance = length(vLightDir);
    vLightDir = normalize(vLightDir);
    float fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;
    float fSpotAngle = dot(vLightDir, g_vLightDir);

    if (fSpotAngle > cos(g_fOuterAngle))
    {
        float fSpotAtt = saturate((fSpotAngle - cos(g_fOuterAngle)) / (cos(g_fInnerAngle) - cos(g_fOuterAngle)));
        fAtt *= fSpotAtt; // ���̶��� ���谪

        // ����Ʈ ����� �븻 ������ ���� ���
        float ndotl = dot(vLightDir * -1.f, normalize(vNormal));
        // ���� ���� 0�� 1 ���̷� ����
        ndotl = saturate(ndotl);

        // ���� ���� ���� ���
        Out.vShade = (g_vLightDiffuse * ndotl + g_vLightAmbient * g_vMtrlAmbient) * fAtt;
        vector vReflect = reflect(vLightDir, normalize(vNormal));
        vector vLook = normalize(vWorldPos - g_vCamPosition);
        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(vReflect * -1.f, vLook), 0.f), 30.f) * fAtt;
    }

    return Out;
}

PS_OUT PS_MAIN_DEFERRED_RESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;

    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    vector vMetalicDesc = g_MetalicTexture.Sample(PointSampler, In.vTexcoord);

    Out.vColor = vDiffuse * vShade + vSpecular * lerp(float4(1.f, 1.f, 1.f, 1.f), vDiffuse, vMetalicDesc);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

    /* �佺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    /* ���彺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightPos = mul(vWorldPos, g_LightViewMatrix);
    vLightPos = mul(vLightPos, g_LightProjMatrix);

    float2 vTexcoord;
    vTexcoord.x = (vLightPos.x / vLightPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (vLightPos.y / vLightPos.w) * -0.5f + 0.5f;

    vector vLightDepthDesc = g_LightDepthTexture.Sample(PointSampler, vTexcoord);

    float fLightOldDepth = vLightDepthDesc.x * 3000.f;

    if (fLightOldDepth + g_fShadowThreshold < vLightPos.w)
        Out.vColor = vector(Out.vColor.rgb * 0.5f, 1.f);

    ////�Ȱ�
  //   float3 fog = float3(0.1f, 0.1f, 0.1f);
  //   float dist = min(max((distance(vWorldPos, g_vCamPosition) - 5.f), 0.f), 100.f) / 100.f;
  //   Out.vColor.rgb *= (1.f - dist);
  //   Out.vColor.rgb += dist * fog;

    vector vEmissiveDesc = g_EmissiveTexture.Sample(PointSampler, In.vTexcoord);
    Out.vColor.rgb += vEmissiveDesc.rgb;

    return Out;
}

PS_OUT PS_DISTORTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float2 objectCenter = float2(0.5f, 0.5f); // ��ü �߽��� �ؽ�ó ��ǥ (ȭ�� �߽�)
    float waveFrequency = 1.f; // �ĵ� ���ļ�
    float waveAmplitude = 0.005f; // �ĵ� ����
    float distortionRadius = 0.1f; // �ϱ׷��� �ݰ�

    if (g_EffectTexture.Sample(LinearSampler, In.vTexcoord).a == 0.f) return Out;

    // ��ü �߽����κ����� �Ÿ� ���
    float2 distance = In.vTexcoord - objectCenter;
    float dist = length(distance);

    // �ϱ׷��� ȿ�� ����
    float2 distortedTex = In.vTexcoord;
    //if (dist < distortionRadius)
    //{
        // �ĵ� ȿ�� ����: ���� �Լ��� ����Ͽ� �ؽ�ó ��ǥ�� ����
        distortedTex = float2(
            In.vTexcoord.x + sin(dist * waveFrequency + /*g_Time +*/ g_DistortionTexture.Sample(LinearSampler, float2(In.vTexcoord.x + g_Time, In.vTexcoord.y + g_Time)).r) * waveAmplitude,
            In.vTexcoord.y + sin(dist * waveFrequency + /*g_Time +*/ g_DistortionTexture.Sample(LinearSampler, float2(In.vTexcoord.x + g_Time, In.vTexcoord.y + g_Time)).r) * waveAmplitude
        );
    //}

    // ������ �ؽ�ó ��ǥ�� �⺻ �ؽ�ó ���ø�
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, distortedTex);
    // ����� ������ �� �� ��Ȯ�� ���̵��� �� ����
    Out.vColor.rgb += float3(0.02f, 0.02f, 0.02f);

    return Out;
}

PS_OUT PS_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    Out.vColor = g_ResultTexture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

PS_OUT PS_FINAL2(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    Out.vColor = g_ResultTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = pow(Out.vColor, g_Value);

    Out.vColor.rgb /= (Out.vColor.rgb + 1.f);
    Out.vColor = pow(Out.vColor, 1 / g_Value);

    return Out;
}

PS_OUT PS_FINAL3(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    Out.vColor = g_ResultTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = pow(Out.vColor, g_Value);

    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;

    Out.vColor = saturate((Out.vColor * (a * Out.vColor + b)) / (Out.vColor * (c * Out.vColor + d) + e));
    Out.vColor = pow(Out.vColor, 1 / g_Value);
    Out.vColor.a = 1.f;

    return Out;
}

PS_OUT PS_LUT(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vResult = g_ResultTexture.Sample(LinearSampler, In.vTexcoord);

    vector vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexcoord);

    if (vDistortion.a > 0.f) Out.vColor = vDistortion;
    else Out.vColor = vResult + vDistortion;

    float2 LutSize = float2(1.0 / 256.0, 1.0 / 16.0);
    float4 LutUV;
    Out.vColor = saturate(Out.vColor) * 15.0;
    LutUV.w = floor(Out.vColor.b);
    LutUV.xy = (Out.vColor.rg + 0.5) * LutSize;
    LutUV.x += LutUV.w * LutSize.y;
    LutUV.z = LutUV.x + LutSize.y;
    Out.vColor = lerp(g_LUTTexture.SampleLevel(LinearSampler, LutUV.xy, 0), g_LUTTexture.SampleLevel(LinearSampler, LutUV.zy, 0), Out.vColor.b - LutUV.w);
    Out.vColor.a = 1.f;

    return Out;
}

PS_OUT PS_NOLUT(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vResult = g_ResultTexture.Sample(LinearSampler, In.vTexcoord);

    vector vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexcoord);

    if (vDistortion.a > 0.f) Out.vColor = vDistortion;
    else Out.vColor = vResult + vDistortion;

    return Out;
}

PS_OUT PS_DOWNSAMPLE4X4(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    //Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    for (int i = 0; i < 4; ++i)
    {
       for (int j = 0; j < 4; ++j)
       {
          Out.vColor += g_DiffuseTexture.Sample(LinearSampler, saturate(In.vTexcoord + float2(-1.5f + i, -1.5f + j) / float2(g_fTexW, g_fTexH)));
       }
    }
    Out.vColor /= 16.f;
    return Out;
}

PS_OUT PS_DOWNSAMPLE5X5(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    //Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    for (int i = 0; i < 5; ++i)
    {
       for (int j = 0; j < 5; ++j)
       {
          Out.vColor += g_DiffuseTexture.Sample(LinearSampler, saturate(In.vTexcoord + float2(-2.f + i, -2.f + j) / float2(g_fTexW, g_fTexH)));
       }
    }
    Out.vColor /= 25.f;
    return Out;
}


PS_OUT PS_UPSAMPLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vOriginalColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDownSampledColor = g_EffectTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = (vOriginalColor + vDownSampledColor * 0.5f);

    return Out;
}

PS_OUT PS_BLURX(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float2 vUV = (float2)0;

    if (g_BlurNum == 1)
    {
       for (int i = -6; i < 7; ++i)
       {
          vUV = saturate(In.vTexcoord + float2(1.f / g_fTexW * i, 0));
          Out.vColor += g_fWeight[6 + i] * g_EffectTexture.Sample(LinearSampler, vUV);
       }

       Out.vColor /= g_fTotal;
    }
    // ����Ƚ�� 1/2 ����
    else if (g_BlurNum == 2)
    {
       for (int i = -13; i < 13; ++i)
       {
          vUV = saturate(In.vTexcoord + float2(1.f / g_fTexW * i * 2.f, 0));
          Out.vColor += g_fWeight2[26 + i * 2] * g_EffectTexture.Sample(LinearSampler, vUV);
       }

       Out.vColor /= g_fTotal/* * 2.f*/;
    }
    // ����Ƚ�� 1/10 ����
    else if (g_BlurNum == 3)
    {
       for (int i = -13; i < 13; ++i)
       {
          vUV = saturate(In.vTexcoord + float2(1.f / g_fTexW * i * 10.f, 0));
          Out.vColor += g_fWeight3[130 + i * 10] * g_EffectTexture.Sample(LinearSampler, vUV);
       }

       Out.vColor /= g_fTotal * 0.8f/* * 8.f*/;
    }
    //Out.vColor = g_EffectTexture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

PS_OUT PS_BLURY(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float2 vUV = (float2)0;

    if (g_BlurNum == 1)
    {
       for (int i = -6; i < 7; ++i)
       {
          vUV = saturate(In.vTexcoord + float2(0, 1.f / g_fTexH * i));
          Out.vColor += g_fWeight[6 + i] * g_EffectTexture.Sample(LinearSampler, vUV);
       }

       Out.vColor /= g_fTotal;
    }
    else if (g_BlurNum == 2)
    {
       for (int i = -26; i < 26; ++i)
       {
          vUV = saturate(In.vTexcoord + float2(0, 1.f / g_fTexH * i));
          Out.vColor += g_fWeight2[26 + i] * g_EffectTexture.Sample(LinearSampler, vUV);
       }

       Out.vColor /= g_fTotal * 2;
    }
    else if (g_BlurNum == 3)
    {
       for (int i = -130; i < 130; ++i)
       {
          vUV = saturate(In.vTexcoord + float2(0, 1.f / g_fTexH * i));
          Out.vColor += g_fWeight3[130 + i] * g_EffectTexture.Sample(LinearSampler, vUV);
       }

       Out.vColor /= g_fTotal * 8;
    }
    //Out.vColor = g_EffectTexture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

PS_OUT PS_BLOOM(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    //float4 color = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    //float brightness = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));

    //if (brightness > 0.8f)
    //   Out.vColor = color;
    //else
    //   Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

technique11 DefaultTechnique
{
    /* Ư�� �������� ������ �� �����ؾ��� ���̴� ����� ��Ʈ���� ���̰� �ִ�. */
    pass DebugRender_0
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_Default, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional_1
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    pass Light_Point_2
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }

    pass Deferred_Result_3
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_RESULT();
    }

    pass Distortion_4
    {
       SetRasterizerState(RS_NoCull);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);



       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;


       PixelShader = compile ps_5_0 PS_DISTORTION();
    }

    pass Final_5
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_FINAL();

    }

    pass Light_Spotlight_6
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_MAIN_LIGHT_SPOTLIGHT();
    }

    pass Reinhard_7
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_FINAL2();

    }

    pass ACES_Filmic_8
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_FINAL3();
    }

    pass DownSampling4x4_9
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_DOWNSAMPLE4X4();
    }

    pass DownSampling5x5_10
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_DOWNSAMPLE5X5();
    }

    pass UpSampling_11
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_UPSAMPLE();
    }

    pass BlurX_12
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_BLURX();
    }

    pass BlurY_13
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_BLURY();
    }

    pass Bloom_14
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_BLOOM();
    }

    pass LUT_15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_LUT();
    }

    pass NOLUT_16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_NOLUT();
    }
}