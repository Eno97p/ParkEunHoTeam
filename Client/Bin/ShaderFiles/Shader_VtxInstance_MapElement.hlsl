#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;


bool g_bDiffuse = false;
bool g_bNormal = false;
bool g_bSpecular = false;
bool g_bOpacity = false;
bool g_bEmissive = false;
bool g_bRoughness = false;
bool g_bMetalic = false;

struct VS_IN
{
    float3		vPosition : POSITION;
    float3		vNormal : NORMAL;
    float2		vTexcoord : TEXCOORD0;
    float3		vTangent : TANGENT;

    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
};

struct VS_OUT
{
    float4      vPosition : SV_POSITION;
    float4		vNormal : NORMAL;
    float2      vTexcoord : TEXCOORD0;
    float4		vTangent : TANGENT;

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
    Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix));

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4		vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4		vTangent : TANGENT;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    Out.vDiffuse = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);

    if (Out.vDiffuse.a < 0.1f)
        discard;

    //float fRatio = In.vLifeTime.y / In.vLifeTime.x;
    //Out.vColor.rgb = lerp(g_StartColor, g_EndColor, fRatio);
  //  Out.vDiffuse = float4(In.vTexcoord, 0, 1);

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


}

