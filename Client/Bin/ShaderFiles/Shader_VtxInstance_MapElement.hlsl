#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_SpecularTexture;
texture2D g_OpacityTexture;
texture2D g_EmissiveTexture;
texture2D g_RoughnessTexture;
texture2D g_MetalicTexture;
texture2D g_DisolveTexture;
texture2D g_BlurTexture;

float g_DisolveValue = 1.f;
float g_TexcoordY = 1.f;
float4 g_fColor = { 1.f, 1.f, 1.f, 1.f };

bool g_bDiffuse = false;
bool g_bNormal = false;
bool g_bSpecular = false;
bool g_bOpacity = false;
bool g_bEmissive = false;
bool g_bRoughness = false;
bool g_bMetalic = false;

struct VS_IN
{
    float3      vPosition : POSITION;
    float3      vNormal : NORMAL;
    float2      vTexcoord : TEXCOORD0;
    float3      vTangent : TANGENT;

    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vLocalPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;

    matrix TransformMatrix = matrix(In.vRight, In.vUp, In.vLook, In.vTranslation);

    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), TransformMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vLocalPos = float4(In.vPosition, 1.f);
    Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), TransformMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);

    return Out;
}


struct VS_OUT_LIGHTDEPTH
{
    float4      vPosition : SV_POSITION;
    float2      vTexcoord : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
};

VS_OUT_LIGHTDEPTH VS_MAIN_LIGHTDEPTH(VS_IN In)
{
    VS_OUT_LIGHTDEPTH Out = (VS_OUT_LIGHTDEPTH)0;

    matrix TransformMatrix = matrix(In.vRight, In.vUp, In.vLook, In.vTranslation);
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vLocalPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vSpecular : SV_TARGET3;
    vector vEmissive : SV_TARGET4;
    vector vRoughness : SV_TARGET5;
    vector vMetalic : SV_TARGET6;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a < 0.1f)
        discard;

    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    if (g_bDiffuse) Out.vDiffuse = vDiffuse;

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
    if (g_bSpecular) Out.vSpecular = vSpecular;

    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
    if (g_bEmissive) Out.vEmissive = vEmissive;
    if (g_bRoughness) Out.vRoughness = vRoughness;
    if (g_bMetalic) Out.vMetalic = vMetalic;

    return Out;
}

struct PS_OUT_COLOR
{
    vector vColor : SV_TARGET0;
};

PS_OUT_COLOR PS_COLOR(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a < 0.1f)
        discard;

    if (g_bDiffuse) Out.vColor = vDiffuse;

    return Out;
}


PS_OUT PS_TILING(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float2 vTilingTexcoord = In.vTexcoord * 40.f;
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, vTilingTexcoord);
    if (vDiffuse.a < 0.1f)
        discard;

    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, vTilingTexcoord);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, vTilingTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    if (g_bDiffuse) Out.vDiffuse = vDiffuse;

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
    if (g_bSpecular) Out.vSpecular = vSpecular;

    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, vTilingTexcoord);
    vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, vTilingTexcoord);
    vector vMetalic = g_MetalicTexture.Sample(LinearSampler, vTilingTexcoord);
    if (g_bEmissive) Out.vEmissive = vEmissive;
    if (g_bRoughness) Out.vRoughness = vRoughness;
    if (g_bMetalic) Out.vMetalic = vMetalic;

    return Out;
}


struct PS_IN_LIGHTDEPTH
{
    float4      vPosition : SV_POSITION;
    float2      vTexcoord : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
};

struct PS_OUT_LIGHTDEPTH
{
    vector      vLightDepth : SV_TARGET0;
};


PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN_LIGHTDEPTH In)
{
    PS_OUT_LIGHTDEPTH      Out = (PS_OUT_LIGHTDEPTH)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vDiffuse.a < 0.1f)
        discard;

    Out.vLightDepth = vector(In.vProjPos.w / 3000.f, 0.0f, 0.f, 0.f);

    return Out;
}

technique11 DefaultTechnique
{

    /* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Color_1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR();
    }

      pass Tmp_2 // 안씀
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR();
    }

      pass Tiling_3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_TILING();
    }

        pass LightDepth
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

}