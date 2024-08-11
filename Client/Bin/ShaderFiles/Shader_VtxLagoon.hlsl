#include "Engine_Shader_Defines.hlsli"

// ---------------------------------------------------------------------------
// Global structs
// ---------------------------------------------------------------------------
struct VS_IN
{
    float2 vPosition : POSITION;
    uint instanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 worldspacePosition : VSO;
};

struct HS_CONSTANTOUTPUT
{
    float edgeTessFactors[3] : SV_TessFactor;
    float insideTessFactor : SV_InsideTessFactor;
};

struct HS_OUTPUT
{
    float4 worldspacePosition : HSO;
};

struct DS_OUTPUT
{
    float4 clipSpacePosition : SV_Position;
    centroid float4 UVForCascade01 : TEXCOORD0;
    centroid float4 UVForCascade23 : TEXCOORD1;
    centroid float2 UVForLocalWaves : TEXCOORD2;
    centroid float4 cascadeBlendingFactors : TEXCOORD3;
    float3 worldspacePositionDisplaced : TEXCOORD4;
    float3 worldspacePositionUndisplaced : TEXCOORD5;
};

struct SURFACE_PARAMETERS
{
    float3 normal;
    float foamSurfaceFolding;
    float foamEnergy;
    float foamWaveHats;
    float2 firstOrderMoments;
    float2 secondOrderMomentsLowestLOD;
    float3 secondOrderMoments;
};

// ---------------------------------------------------------------------------
// Global variables
// ---------------------------------------------------------------------------
struct PerInstanceElement
{
    float2 patchWorldspaceOrigin;
    float  patchWorldspaceScale;
    float  patchMorphConstantAndSign;
};

cbuffer OCEAN_VS_CBUFFER_PERINSTANCE : register(b0)
{
    PerInstanceElement g_perInstanceData[4096];
};

cbuffer OCEAN_VS_HS_DS_CBUFFER : register(b1)
{
    float4x4 g_WorldMatrix;
    float4x4 g_ViewMatrix;
    float4x4 g_ProjMatrix;
    float4 g_eyePos;
    float g_meanOceanLevel;
    float g_useDiamondPattern;
    float g_dynamicTesselationAmount;
    float g_staticTesselationOffset;
    float g_cascade0UVScale;
    float g_cascade1UVScale;
    float g_cascade2UVScale;
    float g_cascade3UVScale;
    float g_cascade0UVOffset;
    float g_cascade1UVOffset;
    float g_cascade2UVOffset;
    float g_cascade3UVOffset;
    float g_UVWarpingAmplitude;
    float g_UVWarpingFrequency;
    float g_localWavesSimulationDomainWorldspaceSize;
    float2 g_localWavesSimulationDomainWorldspaceCenter;
};

cbuffer OCEAN_PS_CBUFFER : register(b2)
{
    float g_cascadeToCascadeScale;
    float g_windWavesTextureSizeInTexels;
    float g_windWavesFoamWhitecapsThreshold;
    float g_localWavesTextureSizeInTexels;
    float g_localWavesFoamWhitecapsThreshold;
    float g_SimulationDomainSize;
    float2 g_SimulationDomainCenter;
    float g_beckmannRoughness;
    float g_showCascades;
    float3 g_sunDirection;
    float g_sunIntensity;
    float g_useMicrofacetFresnel;
    float g_useMicrofacetSpecular;
    float g_useMicrofacetReflection;
    float pad0;
    float pad1;
};

Texture2DArray g_displacementTextureArrayWindWaves : register(t0);
Texture2DArray g_gradientsTextureArrayWindWaves : register(t1);
Texture2DArray g_momentsTextureArrayWindWaves : register(t2);
Texture2D g_displacementTextureLocalWaves : register(t3);
Texture2D g_gradientsTextureLocalWaves : register(t4);
Texture2D g_textureFoam : register(t5);
Texture2D g_textureBubbles : register(t6);
Texture2D g_textureWindGusts : register(t7);
Texture2D g_textureDynamicSkyDome : register(t8);

// ---------------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------------
float GFSDK_WaveWorks_GetEdgeTessellationFactor(float4 vertex1, float4 vertex2)
{
    float3 edgeCenter = 0.5 * (vertex1.xyz + vertex2.xyz);
    float4 worldEdgeCenter = mul(float4(edgeCenter, 1.0), g_WorldMatrix);
    float  edgeLength = length(vertex1.xyz - vertex2.xyz);
    float  distanceToEdge = length(g_eyePos.xyz - worldEdgeCenter.xyz);
    return g_staticTesselationOffset + g_dynamicTesselationAmount * edgeLength / distanceToEdge;
}

float3 GFSDK_WaveWorks_GetUndisplacedVertexWorldPosition(VS_IN input)
{
    float2 vertexPos2D = input.vPosition;
    PerInstanceElement instanceData = g_perInstanceData[input.instanceID];

    // Use geo-morphing in 2D on the plane to smooth away LOD boundaries.
    float geomorphConstant = abs(instanceData.patchMorphConstantAndSign);
    float geomorphSign = -sign(instanceData.patchMorphConstantAndSign);
    if (geomorphSign == 0.0) geomorphSign = 1.0;
    float2 geomorphOffset = geomorphSign.xx;

    // Calculating origin and target positions for geomorphing iteratively, 
    // as the geomorphing can morph vertices multiple (up to 4) levels down
    float geomorphScale = 1.0;
    float2 vertexPos2DSrc = vertexPos2D;
    float2 vertexPos2DTarget = vertexPos2D;
    float geomorphAmount = 0.0;

    for (int geomorphCurrentLevel = 0; geomorphCurrentLevel < 4; geomorphCurrentLevel++)
    {
        if (g_useDiamondPattern == 0)
        {
            float2 intpart;
            float2 rempart = modf(0.5 * geomorphScale * vertexPos2DSrc, intpart);

            if (rempart.x == 0.5) vertexPos2DTarget.x = vertexPos2DSrc.x + geomorphOffset.x;
            if (rempart.y == 0.5) vertexPos2DTarget.y = vertexPos2DSrc.y + geomorphOffset.y;
        }
        else
        {
            // Calculating target vertex positions is more complicated for diamond pattern
            float2 intpart;
            float2 rempart = modf(0.25 * geomorphScale * vertexPos2DSrc, intpart);
            float2 mirror = float2(1.0, 1.0);

            if (rempart.x > 0.5)
            {
                rempart.x = 1.0 - rempart.x;
                mirror.x = -mirror.x;
            }
            if (rempart.y > 0.5)
            {
                rempart.y = 1.0 - rempart.y;
                mirror.y = -mirror.y;
            }

            if (rempart.x == 0.25 && rempart.y == 0.25) vertexPos2DTarget.xy = vertexPos2DSrc.xy - geomorphOffset * mirror;
            else if (rempart.x == 0.25)                 vertexPos2DTarget.x = vertexPos2DSrc.x + geomorphOffset.x * mirror.x;
            else if (rempart.y == 0.25)                 vertexPos2DTarget.y = vertexPos2DSrc.y + geomorphOffset.y * mirror.y;
        }

        // Calculating target geomorphing LOD
        float3 worldspacePos = float3(vertexPos2DTarget * instanceData.patchWorldspaceScale + instanceData.patchWorldspaceOrigin, g_meanOceanLevel);
        float3 eyeVec = worldspacePos - g_eyePos.xyz;
        float d = length(eyeVec);
        float geomorphTargetLevel = log2(d * geomorphConstant) + 1.0;

        geomorphAmount = saturate(2.0 * (geomorphTargetLevel - float(geomorphCurrentLevel)));

        if (geomorphAmount < 1.0)
        {
            break;
        }
        else
        {
            vertexPos2DSrc = vertexPos2DTarget;
            geomorphScale *= 0.5;

            if (g_useDiamondPattern > 0)
            {
                geomorphOffset *= -2.0;
            }
            else
            {
                geomorphOffset *= 2.0;
            }
        }
    }

    // Lerping 
    vertexPos2D = lerp(vertexPos2DSrc, vertexPos2DTarget, geomorphAmount);

    // Transforming the patch vertices to worldspace
    return float3(vertexPos2D.xy * instanceData.patchWorldspaceScale + instanceData.patchWorldspaceOrigin, g_meanOceanLevel);
}



DS_OUTPUT GFSDK_WaveWorks_GetDisplacedVertexAfterTessellation(float4 In0, float4 In1, float4 In2, float3 BarycentricCoords)
{
    // Get starting position
    precise float3 tessellatedWorldspacePosition = In0.xyz * BarycentricCoords.x + In1.xyz * BarycentricCoords.y + In2.xyz * BarycentricCoords.z;
    float3 worldspacePositionUndisplaced = tessellatedWorldspacePosition;

    // Blending factors for cascades    
    float distance = length(g_eyePos.xyz - worldspacePositionUndisplaced);
    float4 cascadeWordldspaceSizes = float4(1.0 / g_cascade0UVScale, 1.0 / g_cascade1UVScale, 1.0 / g_cascade2UVScale, 1.0 / g_cascade3UVScale);
    float4 cascadeBlendingFactors = float4(1.0, cascadeBlendingFactors.yzw = saturate(0.033 * (cascadeWordldspaceSizes.yzw * 30.0 - distance) / cascadeWordldspaceSizes.yzw));

    // UVs
    float2 UVForCascade0 = worldspacePositionUndisplaced.xy * g_cascade0UVScale + g_cascade0UVOffset.xx;
    float2 UVForCascade1 = worldspacePositionUndisplaced.xy * g_cascade1UVScale + g_cascade1UVOffset.xx;
    float2 UVForCascade2 = worldspacePositionUndisplaced.xy * g_cascade2UVScale + g_cascade2UVOffset.xx;
    float2 UVForCascade3 = worldspacePositionUndisplaced.xy * g_cascade3UVScale + g_cascade3UVOffset.xx;
    float2 UVForLocalWaves = float2(0.5, 0.5) + (worldspacePositionUndisplaced.xy - g_localWavesSimulationDomainWorldspaceCenter) / g_localWavesSimulationDomainWorldspaceSize;

    UVForCascade0 += float2(g_UVWarpingAmplitude * cos(UVForCascade0.y * g_UVWarpingFrequency), g_UVWarpingAmplitude * sin(UVForCascade0.x * g_UVWarpingFrequency));
    UVForCascade1 += float2(g_UVWarpingAmplitude * cos(UVForCascade1.y * g_UVWarpingFrequency), g_UVWarpingAmplitude * sin(UVForCascade1.x * g_UVWarpingFrequency));
    UVForCascade2 += float2(g_UVWarpingAmplitude * cos(UVForCascade2.y * g_UVWarpingFrequency), g_UVWarpingAmplitude * sin(UVForCascade2.x * g_UVWarpingFrequency));
    UVForCascade3 += float2(g_UVWarpingAmplitude * cos(UVForCascade3.y * g_UVWarpingFrequency), g_UVWarpingAmplitude * sin(UVForCascade3.x * g_UVWarpingFrequency));

    // displacements
    float3 displacement;
    displacement = cascadeBlendingFactors.x * g_displacementTextureArrayWindWaves.SampleLevel(LinearSampler, float3(UVForCascade0, 0.0), 0.0).xyz;
    displacement += cascadeBlendingFactors.y == 0 ? float3(0, 0, 0) : cascadeBlendingFactors.y * g_displacementTextureArrayWindWaves.SampleLevel(LinearSampler, float3(UVForCascade1, 1.0), 0.0).xyz;
    displacement += cascadeBlendingFactors.z == 0 ? float3(0, 0, 0) : cascadeBlendingFactors.z * g_displacementTextureArrayWindWaves.SampleLevel(LinearSampler, float3(UVForCascade2, 2.0), 0.0).xyz;
    displacement += cascadeBlendingFactors.w == 0 ? float3(0, 0, 0) : cascadeBlendingFactors.w * g_displacementTextureArrayWindWaves.SampleLevel(LinearSampler, float3(UVForCascade3, 3.0), 0.0).xyz;
    displacement += g_displacementTextureLocalWaves.SampleLevel(LinearSampler, UVForLocalWaves, 0.0).xyz;
    float3 worldspacePositionDisplaced = worldspacePositionUndisplaced + displacement;

    // Output
    DS_OUTPUT Output;
    Output.UVForCascade01.xy = UVForCascade0;
    Output.UVForCascade01.zw = UVForCascade1;
    Output.UVForCascade23.xy = UVForCascade2;
    Output.UVForCascade23.zw = UVForCascade3;
    Output.UVForLocalWaves = UVForLocalWaves;
    Output.cascadeBlendingFactors = cascadeBlendingFactors;
    Output.worldspacePositionDisplaced = worldspacePositionDisplaced;
    Output.worldspacePositionUndisplaced = worldspacePositionUndisplaced;

    return Output;
}

float3 CombineMoments(float2 FirstOrderMomentsA, float2 FirstOrderMomentsB, float3 SecondOrderMomentsA, float3 SecondOrderMomentsB)
{
    return float3(
        SecondOrderMomentsA.x + SecondOrderMomentsB.x + 2.0 * FirstOrderMomentsA.x * FirstOrderMomentsB.x,
        SecondOrderMomentsA.y + SecondOrderMomentsB.y + 2.0 * FirstOrderMomentsA.y * FirstOrderMomentsB.y,
        SecondOrderMomentsA.z + SecondOrderMomentsB.z + FirstOrderMomentsA.x * FirstOrderMomentsB.y + FirstOrderMomentsA.y * FirstOrderMomentsB.x);
}

SURFACE_PARAMETERS GFSDK_WaveWorks_GetSurfaceParameters(DS_OUTPUT In)
{
    // Reading the gradient/surface folding data from texture array for wind simulation and from texture for local waves
    float4 gradFold0 = g_gradientsTextureArrayWindWaves.Sample(LinearSampler, float3(In.UVForCascade01.xy, 0.0));
    float4 gradFold1 = g_gradientsTextureArrayWindWaves.Sample(LinearSampler, float3(In.UVForCascade01.zw, 1.0));
    float4 gradFold2 = g_gradientsTextureArrayWindWaves.Sample(LinearSampler, float3(In.UVForCascade23.xy, 2.0));
    float4 gradFold3 = g_gradientsTextureArrayWindWaves.Sample(LinearSampler, float3(In.UVForCascade23.zw, 3.0));
    float4 localWavesGradFold = g_gradientsTextureLocalWaves.Sample(LinearSampler, In.UVForLocalWaves);

    // Reading the second order moments from texture array
    float3 secondOrderMoments0 = g_momentsTextureArrayWindWaves.Sample(LinearSampler, float3(In.UVForCascade01.xy, 0.0)).xyz;
    float3 secondOrderMoments1 = g_momentsTextureArrayWindWaves.Sample(LinearSampler, float3(In.UVForCascade01.zw, 1.0)).xyz;
    float3 secondOrderMoments2 = g_momentsTextureArrayWindWaves.Sample(LinearSampler, float3(In.UVForCascade23.xy, 2.0)).xyz;
    float3 secondOrderMoments3 = g_momentsTextureArrayWindWaves.Sample(LinearSampler, float3(In.UVForCascade23.zw, 3.0)).xyz;

    // Read the lowest LOD second order moments from texture array
    float3 secondOrderMomentsLowestLOD0 = g_momentsTextureArrayWindWaves.SampleLevel(LinearSampler, float3(0.0, 0.0, 0.0), 20.0).xyz;
    float3 secondOrderMomentsLowestLOD1 = g_momentsTextureArrayWindWaves.SampleLevel(LinearSampler, float3(0.0, 0.0, 1.0), 20.0).xyz;
    float3 secondOrderMomentsLowestLOD2 = g_momentsTextureArrayWindWaves.SampleLevel(LinearSampler, float3(0.0, 0.0, 2.0), 20.0).xyz;
    float3 secondOrderMomentsLowestLOD3 = g_momentsTextureArrayWindWaves.SampleLevel(LinearSampler, float3(0.0, 0.0, 3.0), 20.0).xyz;

    // Splitting data from textures to meaningfully named variables
    float2 firstOrderMoments0 = gradFold0.xy;
    float2 firstOrderMoments1 = gradFold1.xy;
    float2 firstOrderMoments2 = gradFold2.xy;
    float2 firstOrderMoments3 = gradFold3.xy;
    float2 localWavesGradients = localWavesGradFold.xy;

    float surfaceFolding0 = gradFold0.z;
    float surfaceFolding1 = gradFold1.z;
    float surfaceFolding2 = gradFold2.z;
    float surfaceFolding3 = gradFold3.z;
    float localWavesSurfaceFolding = localWavesGradFold.z;

    float foamEnergy0 = gradFold0.w;
    float foamEnergy1 = gradFold1.w;
    float foamEnergy2 = gradFold2.w;
    float foamEnergy3 = gradFold3.w;
    float localWavesFoamEnergy = localWavesGradFold.w;

    // Combining gradients (first order moments) from all the cascades and the local waves to calculate normal
    float2 gradients =
        firstOrderMoments0 * In.cascadeBlendingFactors.x +
        firstOrderMoments1 * In.cascadeBlendingFactors.y +
        firstOrderMoments2 * In.cascadeBlendingFactors.z +
        firstOrderMoments3 * In.cascadeBlendingFactors.w +
        localWavesGradients;
    float3 normal = normalize(float3(gradients, 1.0));

    // Calculating foam energy
    float energyC2CScale = 0.7 / sqrt(g_cascadeToCascadeScale);
    float foamEnergy =
        100.0 * foamEnergy0 *
        lerp(energyC2CScale, foamEnergy1, In.cascadeBlendingFactors.y) *
        lerp(energyC2CScale, foamEnergy2, In.cascadeBlendingFactors.z) *
        lerp(energyC2CScale, foamEnergy3, In.cascadeBlendingFactors.w) +
        localWavesFoamEnergy;

    // Calculating surface folding and wave hats
    float surfaceFolding =
        max(-100.0,
            (1.0 - surfaceFolding0) +
            (1.0 - surfaceFolding1) +
            (1.0 - surfaceFolding2) +
            (1.0 - surfaceFolding3) +
            (1.0 - localWavesSurfaceFolding));

    float waveHatsC2CScale = 0.5;
    float foamWaveHats =
        10.0 * (-g_windWavesFoamWhitecapsThreshold +
            (1.0 - surfaceFolding0) +
            (1.0 - surfaceFolding1) * waveHatsC2CScale +
            (1.0 - surfaceFolding2) * waveHatsC2CScale * waveHatsC2CScale +
            (1.0 - surfaceFolding3) * waveHatsC2CScale * waveHatsC2CScale * waveHatsC2CScale +
            (1.0 - localWavesSurfaceFolding - g_localWavesFoamWhitecapsThreshold));

    float4 cascadeBlendingFactorsSquared = In.cascadeBlendingFactors * In.cascadeBlendingFactors;

    // Fading second order moments to lowest LOD (single values) where cascades fade out
    secondOrderMoments0 = cascadeBlendingFactorsSquared.x * secondOrderMoments0 + (1.0 - cascadeBlendingFactorsSquared.x) * secondOrderMomentsLowestLOD0;
    secondOrderMoments1 = cascadeBlendingFactorsSquared.y * secondOrderMoments1 + (1.0 - cascadeBlendingFactorsSquared.y) * secondOrderMomentsLowestLOD1;
    secondOrderMoments2 = cascadeBlendingFactorsSquared.z * secondOrderMoments2 + (1.0 - cascadeBlendingFactorsSquared.z) * secondOrderMomentsLowestLOD2;
    secondOrderMoments3 = cascadeBlendingFactorsSquared.w * secondOrderMoments3 + (1.0 - cascadeBlendingFactorsSquared.w) * secondOrderMomentsLowestLOD3;

    // Fading first order moments to zero where cascades fade out
    firstOrderMoments0 *= In.cascadeBlendingFactors.x;
    firstOrderMoments1 *= In.cascadeBlendingFactors.y;
    firstOrderMoments2 *= In.cascadeBlendingFactors.z;
    firstOrderMoments3 *= In.cascadeBlendingFactors.w;

    // Combining first and second order moments from all 4 cascades, step by step
    float2 firstOrderMoments01 = firstOrderMoments0 + firstOrderMoments1;
    float3 secondOrderMoments01 = CombineMoments(firstOrderMoments0, firstOrderMoments1, secondOrderMoments0, secondOrderMoments1);

    float2 firstOrderMoments012 = firstOrderMoments01 + firstOrderMoments2;
    float3 secondOrderMoments012 = CombineMoments(firstOrderMoments01, firstOrderMoments2, secondOrderMoments01, secondOrderMoments2);

    float2 firstOrderMoments0123 = firstOrderMoments012 + firstOrderMoments3;
    float3 secondOrderMoments0123 = CombineMoments(firstOrderMoments012, firstOrderMoments3, secondOrderMoments012, secondOrderMoments3);

    // Combining with first order moments of local waves
    float2 firstOrderMoments0123L = firstOrderMoments0123 + localWavesGradients;
    float3 secondOrderMoments0123L = CombineMoments(firstOrderMoments0123, localWavesGradients, secondOrderMoments0123, float3(localWavesGradients.xy * localWavesGradients.xy, localWavesGradients.x * localWavesGradients.y));

    // Combining lowest LOD second order moments, fading in where simulation cascades fade out
    float2 secondOrderMomentsLowestLOD =
        secondOrderMomentsLowestLOD0.xy * (1.0 - cascadeBlendingFactorsSquared.x) +
        secondOrderMomentsLowestLOD1.xy * (1.0 - cascadeBlendingFactorsSquared.y) +
        secondOrderMomentsLowestLOD2.xy * (1.0 - cascadeBlendingFactorsSquared.z) +
        secondOrderMomentsLowestLOD3.xy * (1.0 - cascadeBlendingFactorsSquared.w) +
        localWavesGradients * localWavesGradients;

    // Output
    SURFACE_PARAMETERS Output;
    Output.normal = normal;
    Output.foamSurfaceFolding = surfaceFolding;
    Output.foamEnergy = foamEnergy;
    Output.foamWaveHats = foamWaveHats;
    Output.firstOrderMoments = firstOrderMoments0123L;
    Output.secondOrderMomentsLowestLOD = secondOrderMomentsLowestLOD;
    Output.secondOrderMoments = secondOrderMoments0123L;
    return Output;
}

// ... [여기에 GetFoam 및 기타 필요한 헬퍼 함수들을 추가]

float GetFoam(float3 worldspacePosition, SURFACE_PARAMETERS surfParams)
{
    float foamDensityMapLowFrequency = g_textureFoam.Sample(LinearSampler, worldspacePosition.xy * 0.04).x - 1.0;
    float foamDensityMapHighFrequency = g_textureFoam.Sample(LinearSampler, worldspacePosition.xy * 0.15).x - 1.0;

    float foamDensity = saturate(foamDensityMapHighFrequency + min(3.5, 1.0 * surfParams.foamEnergy - 0.2));
    foamDensity += (foamDensityMapLowFrequency + min(1.5, 1.0 * surfParams.foamEnergy));

    foamDensity -= 0.1 * saturate(-surfParams.foamSurfaceFolding);
    foamDensity = max(0, foamDensity);
    foamDensity *= 1.0 + 0.8 * saturate(surfParams.foamSurfaceFolding);

    return saturate(foamDensity);
}
// ---------------------------------------------------------------------------
// Vertex Shader
// ---------------------------------------------------------------------------
VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    float3 worldPosition = GFSDK_WaveWorks_GetUndisplacedVertexWorldPosition(input);
    float4 worldPos = mul(float4(worldPosition, 1.0), g_WorldMatrix);
    float4 viewPos = mul(worldPos, g_ViewMatrix);

    output.worldspacePosition = worldPos;

    return output;
}

// ---------------------------------------------------------------------------
// Hull Shader
// ---------------------------------------------------------------------------
HS_CONSTANTOUTPUT HS_Constant(InputPatch<VS_OUT, 3> input)
{
    HS_CONSTANTOUTPUT output;
    output.edgeTessFactors[0] = GFSDK_WaveWorks_GetEdgeTessellationFactor(input[1].worldspacePosition, input[2].worldspacePosition);
    output.edgeTessFactors[1] = GFSDK_WaveWorks_GetEdgeTessellationFactor(input[2].worldspacePosition, input[0].worldspacePosition);
    output.edgeTessFactors[2] = GFSDK_WaveWorks_GetEdgeTessellationFactor(input[0].worldspacePosition, input[1].worldspacePosition);
    output.insideTessFactor = (output.edgeTessFactors[0] + output.edgeTessFactors[1] + output.edgeTessFactors[2]) / 3.0;
    return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[patchconstantfunc("HS_Constant")]
[outputcontrolpoints(3)]
HS_OUTPUT HS_MAIN(InputPatch<VS_OUT, 3> input, uint uCPID : SV_OutputControlPointID)
{
    HS_OUTPUT output;
    output.worldspacePosition = input[uCPID].worldspacePosition;
    return output;
}

// ---------------------------------------------------------------------------
// Domain Shader
// ---------------------------------------------------------------------------
[domain("tri")]
DS_OUTPUT DS_MAIN(HS_CONSTANTOUTPUT HSConstantData, const OutputPatch<HS_OUTPUT, 3> input, float3 barycentricCoords : SV_DomainLocation)
{
    DS_OUTPUT output = GFSDK_WaveWorks_GetDisplacedVertexAfterTessellation(input[0].worldspacePosition, input[1].worldspacePosition, input[2].worldspacePosition, barycentricCoords);

    float4 worldPos = float4(output.worldspacePositionDisplaced, 1.0);
    float4 viewPos = mul(worldPos, g_ViewMatrix);
    output.clipSpacePosition = mul(viewPos, g_ProjMatrix);

    return output;
}

// ---------------------------------------------------------------------------
// Pixel Shader
// ---------------------------------------------------------------------------

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    //vector vNormal : SV_TARGET1;
    //vector vDepth : SV_TARGET2;
    //vector vSpecular : SV_TARGET3;
    //vector vEmissive : SV_TARGET4;
    //vector vRoughness : SV_TARGET5;
    //vector vMetalic : SV_TARGET6;
    //float2 vVelocity : SV_TARGET7;
};


PS_OUT PS_MAIN(DS_OUTPUT input)
{
    PS_OUT Out = (PS_OUT)0;

    SURFACE_PARAMETERS surfaceParameters = GFSDK_WaveWorks_GetSurfaceParameters(input);
    float foamIntensity = GetFoam(input.worldspacePositionUndisplaced, surfaceParameters);

    float3 worldSpaceEyePos = mul(g_eyePos, g_WorldMatrix).xyz;
    float3 viewDir = normalize(worldSpaceEyePos - input.worldspacePositionDisplaced);
    float3 lightDir = normalize(g_sunDirection);
    float3 halfVec = normalize(viewDir + lightDir);

    float3 normal = surfaceParameters.normal;

    // 기본 물 색상
    float3 waterColor = float3(0.0, 0.3, 0.5);

    // 프레넬 효과
    float fresnel = pow(1.0 - saturate(dot(viewDir, normal)), 5.0);

    // 반사
    float3 reflection = g_textureDynamicSkyDome.Sample(LinearSampler, reflect(-viewDir, normal).xy * 0.5 + 0.5).rgb;

    // 스페큘러
    float specular = pow(max(dot(normal, halfVec), 0.0), 100.0);

    // 최종 색상 조합
    float3 finalColor = lerp(waterColor, reflection, fresnel);
    finalColor += g_sunIntensity * specular * float3(1.0, 0.9, 0.7);
    finalColor = lerp(finalColor, float3(1.0, 1.0, 1.0), foamIntensity);

    Out.vColor = float4(1.f, 0.f, 1.f, 1.0);
    //Out.vNormal = vector(0.f, 1.f, 0.f, 1.f);
    //Out.vDepth = vector(1, 1, 0.0f, 1.f);

    return Out;
}
// ---------------------------------------------------------------------------
// Techniques
// ---------------------------------------------------------------------------
technique11 DefaultTechnique
{
      pass WaterPass
    {
       SetRasterizerState(RS_Wireframe);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = compile hs_5_0 HS_MAIN();
       DomainShader = compile ds_5_0 DS_MAIN();
       PixelShader = compile ps_5_0 PS_MAIN();
    }


}