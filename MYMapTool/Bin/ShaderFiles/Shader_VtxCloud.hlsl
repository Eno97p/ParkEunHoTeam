#include "Engine_Shader_Defines.hlsli"
#define UI0 1597334673U
#define UI1 3812015801U
#define UIF (1.0 / float(0xffffffffU))

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vCamPosition;

// Cloud specific variables
float g_CloudDensity = 0.8f;
float g_CloudScale = 0.01f;
float g_CloudSpeed = 0.1f;
float g_CloudHeight = 100.0f;
float3 g_CloudColor = float3(0.6f, 0.6f, 0.6f);
float g_fAccTime;

float4 g_vLightDiffuse = float4(0.76f, 0.8f, 0.95f, 1.0f);
float4 g_vLightAmbient = float4(0.2f, 0.2f, 0.3f, 1.0f);
float4 g_vLightSpecular = float4(1.0f, 1.0f, 1.0f, 1.0f);
float4 g_vLightDir = normalize(float4(-1.0f, -1.0f, -1.0f, 0.0f));

float4 g_vLightPosition;
float g_fLightRange;

// 셰이더 코드에 추가
float g_fCoarseStepSize;
float g_fFineStepSize;
int g_iMaxCoarseSteps;
int g_iMaxFineSteps;
float g_fDensityThreshold;
float g_fAlphaThreshold;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vWorldPos : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vLocalPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;
    Out.vProjPos = Out.vPosition;
    Out.vLocalPos = In.vPosition;  // 로컬 위치 전달

    return Out;
}

float3 hash33(float3 p)
{
    uint3 q = uint3(int3(p)) * uint3(UI0, UI1, 2798796415U);
    q = (q.x ^ q.y ^ q.z) * uint3(UI0, UI1, 2798796415U);
    return -1.0 + 2.0 * float3(q) * UIF;
}

float remap(float x, float a, float b, float c, float d)
{
    return (((x - a) / (b - a)) * (d - c)) + c;
}

float gradientNoise(float3 x, float freq)
{
    float3 p = floor(x);
    float3 w = frac(x);

    float3 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);

    float3 ga = hash33(fmod(p + float3(0.0, 0.0, 0.0), freq));
    float3 gb = hash33(fmod(p + float3(1.0, 0.0, 0.0), freq));
    float3 gc = hash33(fmod(p + float3(0.0, 1.0, 0.0), freq));
    float3 gd = hash33(fmod(p + float3(1.0, 1.0, 0.0), freq));
    float3 ge = hash33(fmod(p + float3(0.0, 0.0, 1.0), freq));
    float3 gf = hash33(fmod(p + float3(1.0, 0.0, 1.0), freq));
    float3 gg = hash33(fmod(p + float3(0.0, 1.0, 1.0), freq));
    float3 gh = hash33(fmod(p + float3(1.0, 1.0, 1.0), freq));

    float va = dot(ga, w - float3(0.0, 0.0, 0.0));
    float vb = dot(gb, w - float3(1.0, 0.0, 0.0));
    float vc = dot(gc, w - float3(0.0, 1.0, 0.0));
    float vd = dot(gd, w - float3(1.0, 1.0, 0.0));
    float ve = dot(ge, w - float3(0.0, 0.0, 1.0));
    float vf = dot(gf, w - float3(1.0, 0.0, 1.0));
    float vg = dot(gg, w - float3(0.0, 1.0, 1.0));
    float vh = dot(gh, w - float3(1.0, 1.0, 1.0));

    return va +
        u.x * (vb - va) +
        u.y * (vc - va) +
        u.z * (ve - va) +
        u.x * u.y * (va - vb - vc + vd) +
        u.y * u.z * (va - vc - ve + vg) +
        u.z * u.x * (va - vb - ve + vf) +
        u.x * u.y * u.z * (-va + vb + vc - vd + ve - vf - vg + vh);
}

float worleyNoise(float3 uv, float freq)
{
    float3 id = floor(uv);
    float3 p = frac(uv);

    float minDist = 10000.0;
    for (float x = -1.0; x <= 1.0; ++x)
    {
        for (float y = -1.0; y <= 1.0; ++y)
        {
            for (float z = -1.0; z <= 1.0; ++z)
            {
                float3 offset = float3(x, y, z);
                float3 h = hash33(fmod(id + offset, float3(freq, freq, freq))) * 0.5 + 0.5;
                h += offset;
                float3 d = p - h;
                minDist = min(minDist, dot(d, d));
            }
        }
    }

    return 1.0 - minDist;
}

float perlinfbm(float3 p, float freq, int octaves)
{
    float G = exp2(-0.85);
    float amp = 1.0;
    float noise = 0.0;
    for (int i = 0; i < octaves; ++i)
    {
        noise += amp * gradientNoise(p * freq, freq);
        freq *= 2.0;
        amp *= G;
    }

    return noise;
}

float worleyFbm(float3 p, float freq)
{
    return worleyNoise(p * freq, freq) * 0.625 +
        worleyNoise(p * freq * 2.0, freq * 2.0) * 0.25 +
        worleyNoise(p * freq * 4.0, freq * 4.0) * 0.125;
}

float calculateCloudDensity(float3 position, float time)
{
    float freq = 4.0;
    float pfbm = lerp(1.0, perlinfbm(position + float3(time * 0.1, 0, 0), 4.0, 7), 0.5);
    pfbm = abs(pfbm * 2.0 - 1.0); // billowy perlin noise

    float worley = worleyFbm(position + float3(time * 0.05, 0, 0), freq);

    float perlinWorley = remap(pfbm, 0.0, 1.0, worley, 1.0);

    float cloud = remap(perlinWorley, worley - 1.0, 1.0, 0.0, 1.0);
    cloud = pow(cloud, 1.5); // 구름 형태를 더 뚜렷하게
    cloud = remap(cloud, 0.7, 1.0, 0.0, 1.0); // 구름 coverage 조정

    return cloud * g_CloudDensity * 2.0; // 밀도 증가
}

float HenyeyGreenstein(float cosTheta, float g)
{
    float g2 = g * g;
    return (1.0 - g2) / (4.0 * 3.14159 * pow(1.0 + g2 - 2.0 * g * cosTheta, 1.5));
}

float3 calculatePointLightEnergy(float3 position, float3 lightPos, float lightRange, float density, float3 viewDir)
{
    float3 lightDir = normalize(lightPos - position);
    float distanceToLight = length(lightPos - position);

    const float LIGHT_ABSORB_COEFF = 0.5;
    const float LIGHT_SCATTER_COEFF = 0.5;
    const float g = 0.2; // forward scattering parameter

    float cosTheta = dot(lightDir, viewDir);
    float phase = HenyeyGreenstein(cosTheta, g);

    float beerLambert = exp(-density * LIGHT_ABSORB_COEFF);
    float inscattering = (1.0 - exp(-density * LIGHT_SCATTER_COEFF)) * phase;

    // 포인트 라이트의 감쇠 계산
    float attenuation = 1.0 - smoothstep(0.0, lightRange, distanceToLight);

    return g_vLightDiffuse.rgb * (beerLambert + inscattering) * attenuation;
}

struct PS_OUT
{
    float4 vColor : SV_Target0;
};

float sphereTrace(float3 start, float3 dir, float maxDist, float stepSize)
{
    float t = 0.0;
    for (int i = 0; i < 128; i++)
    {
        float3 pos = start + t * dir;
        float density = calculateCloudDensity(pos * g_CloudScale, g_fAccTime * g_CloudSpeed);
        if (density > 0.01)
        {
            return t;
        }
        t += stepSize;
        if (t > maxDist) break;
    }
    return maxDist;
}

float coarseSpherTrace(float3 start, float3 dir, float maxDist, float stepSize)
{
    float t = 0.0;
    for (int i = 0; i < 32; i++) // 반복 횟수 감소
    {
        float3 pos = start + t * dir;
        float density = calculateCloudDensity(pos * g_CloudScale, g_fAccTime * g_CloudSpeed);
        if (density > 0.05) // 임계값 증가
        {
            return t;
        }
        t += stepSize * 2.0; // 스텝 크기 증가
        if (t > maxDist) break;
    }
    return maxDist;
}

float fineSpherTrace(float3 start, float3 dir, float maxDist, float stepSize)
{
    float t = 0.0;
    for (int i = 0; i < 16; i++) // 정밀 반복 횟수 제한
    {
        float3 pos = start + t * dir;
        float density = calculateCloudDensity(pos * g_CloudScale, g_fAccTime * g_CloudSpeed);
        if (density > 0.01)
        {
            return t;
        }
        t += stepSize * 0.5; // 더 작은 스텝 크기
        if (t > maxDist) break;
    }
    return maxDist;
}

PS_OUT PS_MAIN(VS_OUT In)
{
    PS_OUT Out = (PS_OUT)0;
    float3 worldPos = In.vWorldPos;
    float time = g_fAccTime;
    const float MAX_DIST = 100.0;
    float3 viewDir = normalize(worldPos - g_vCamPosition.xyz);
    float4 cloudColor = float4(0, 0, 0, 0);

    // 대략적인 레이 마칭
    float coarseT = 0.0;
    for (int i = 0; i < g_iMaxCoarseSteps; i++)
    {
        float3 pos = worldPos + coarseT * viewDir;
        float density = calculateCloudDensity(pos * g_CloudScale, time * g_CloudSpeed);
        if (density > g_fDensityThreshold)
            break;
        coarseT += g_fCoarseStepSize;
        if (coarseT > MAX_DIST)
        {
            discard;
            return Out;
        }
    }

    // 정밀한 레이 마칭
    float t = max(0, coarseT - g_fCoarseStepSize);
    float3 currentPos = worldPos + t * viewDir;
    float totalDensity = 0.0;

    [loop]
        for (int j = 0; j < g_iMaxFineSteps; j++)
        {
            float density = calculateCloudDensity(currentPos * g_CloudScale, time * g_CloudSpeed);
            if (density > 0.01)
            {
                totalDensity += density * g_fFineStepSize;
                float3 lightEnergy = calculatePointLightEnergy(currentPos, g_vLightPosition, g_fLightRange, density, viewDir);
                float3 color = g_CloudColor * lightEnergy * density;
                float transmittance = exp(-density * g_fFineStepSize);
                cloudColor.rgb += color * (1.0 - cloudColor.a) * transmittance;
                cloudColor.a += density * (1.0 - cloudColor.a) * 0.1;

                if (cloudColor.a > g_fAlphaThreshold) break;
            }
            currentPos += viewDir * g_fFineStepSize;
            t += g_fFineStepSize;
            if (t > MAX_DIST) break;
        }

    cloudColor.a = 1.0 - exp(-totalDensity * 0.1);
    if (cloudColor.a < 0.01)
        discard;

    float3 ambient = g_vLightAmbient.rgb * g_CloudColor * 0.1;
    cloudColor.rgb += ambient * (1.0 - cloudColor.a);
    cloudColor.rgb = pow(cloudColor.rgb, 1.0 / 2.2);

    Out.vColor = cloudColor;
    return Out;
}

technique11 DefaultTechnique
{
    pass Cloud
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0); // 깊이 쓰기 비활성화
        SetBlendState(CloudBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}