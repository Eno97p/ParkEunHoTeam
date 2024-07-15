#include "Engine_Shader_Defines.hlsli"
#include "ShaderFunctor.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture, g_PerlinNoise;
float       g_fTime = 0.01f;
vector		g_vCamPosition;

float noise(float2 st)
{
    return frac(sin(dot(st.xy, float2(12.9898, 78.233))) * 43758.5453123);
}

struct VS_IN
{
    float3 vStartpos : STARTPOSITION;
    float3 vEndpos : ENDPOSITION;
    float2 vLifeTime : LIFETIME;
    float fThickness : THICKNESS;
    float fAmplitude : AMPLITUDE;
    int   iNumSegments : NUMSEGMENTS;
};

struct GS_IN
{
    float3 vStartpos : POSITION0;
    float3 vEndpos : POSITION1;
    float2 vLifeTime : LIFETIME;
    float fThickness : THICKNESS;
    float fAmplitude : AMPLITUDE;
    int iNumSegments : NUMSEGMENTS;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float fLifeTime : LIFETIME;
};


GS_IN VS_MAIN(VS_IN input)
{
	GS_IN output = (GS_IN)0;

    output.vStartpos =  input.vStartpos;
    output.vEndpos =    input.vEndpos;
    output.vLifeTime =  input.vLifeTime;
    output.fThickness = input.fThickness;
    output.fAmplitude = input.fAmplitude;
    output.iNumSegments = input.iNumSegments;

	return output;
}

[maxvertexcount(100)]  // 최대 세그먼트 수 * 2 + 2, 필요에 따라 조정
void GS_MAIN(point GS_IN input[1], inout TriangleStream<PS_IN> outputStream)
{
    PS_IN output = (PS_IN)0;

    float3 start = input[0].vStartpos;
    float3 end = input[0].vEndpos;
    float3 direction = normalize(end - start);
    float length = distance(start, end);

    int numSegments = input[0].iNumSegments;
    float segmentLength = length / numSegments;
    float size = input[0].fThickness * 0.5f;  // 사각형의 크기

    for (int i = 0; i < numSegments; i++)
    {
        float t = (float)i / (float)(numSegments - 1);
        float3 pos = lerp(start, end, t);

        // 빌보딩을 위한 방향 계산
        float3 look = normalize(g_vCamPosition - pos);
        float3 right = normalize(cross(float3(0, 1, 0), look));
        float3 up = normalize(cross(look, right));

        // 선의 방향을 고려한 회전 적용
        float3 adjustedRight = normalize(cross(direction, up));
        up = normalize(cross(adjustedRight, look));

        // 사각형의 네 꼭지점 계산
        float3 vertices[4] = {
            pos + (-adjustedRight - up) * size,
            pos + (adjustedRight - up) * size,
            pos + (-adjustedRight + up) * size,
            pos + (adjustedRight + up) * size
        };

        for (int j = 0; j < 4; j++)
        {
            output.vPosition = mul(float4(vertices[j], 1.0f), g_ViewMatrix);
            output.vPosition = mul(output.vPosition, g_ProjMatrix);
            output.vTexcoord = float2((j == 1 || j == 3) ? 1 : 0, (j == 2 || j == 3) ? 1 : 0);
            output.fLifeTime = 1.0f - t;  // 시작점에서 1, 끝점에서 0
            outputStream.Append(output);
        }
        outputStream.RestartStrip();
    }
}

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN input)
{
    PS_OUT output = (PS_OUT)0;
    //output.vColor = float4(1, 1, 1, 1);
    // 기본 UV 좌표
    float2 uv = input.vTexcoord;
    
    // 시간에 따라 변화하는 오프셋
    float2 timeOffset = float2(g_fTime * 0.5, g_fTime * 0.3);
    
    // 첫 번째 노이즈 레이어
    float noise1 = g_PerlinNoise.Sample(LinearSampler, uv * 2 + timeOffset).r;
    
    // 두 번째 노이즈 레이어 (다른 스케일과 속도)
    float noise2 = g_PerlinNoise.Sample(LinearSampler, uv * 4 - timeOffset * 1.3).r;
    
    // 두 노이즈를 조합
    float combinedNoise = (noise1 * 0.6 + noise2 * 0.4);
    
    // 전기 효과를 위한 임계값
    float threshold = 0.7 + sin(uv.x * 10 + g_fTime * 3) * 0.1;
    
    // 임계값을 넘는 부분만 표시
    float electricMask = step(threshold, combinedNoise);
    
    // 전기 색상 (파란색 계열)
    float4 electricColor = float4(0.3, 0.7, 1.0, 1.0);
    
    // 가장자리 부분을 부드럽게 처리
    float softEdge = smoothstep(threshold - 0.05, threshold + 0.05, combinedNoise);
    
    // 최종 색상 계산
    float4 finalColor = electricColor * softEdge;
    
    // 알파 값 조정 (수명 및 텍스처 기반)
    float4 texColor = g_Texture.Sample(LinearSampler, uv);
    finalColor.a *= texColor.r * smoothstep(0, 0.2, input.fLifeTime);
    
    output.vColor = finalColor;
    return output;
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
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}

