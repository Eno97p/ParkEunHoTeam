#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"

#include "Shader.h"
#include "ComputeShader_Texture.h"
#include "ComputeShader_Buffer.h"
#include "BlendObject.h"
#include "VIBuffer_Rect.h"
#include "RenderTarget.h"

#include"CRenderWorker.h"
_uint      g_iSizeX = 8192;
_uint      g_iSizeY = 4608;

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
    D3D11_VIEWPORT         ViewportDesc{};
    _uint               iNumViewports = 1;

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    /* Target_Diffuse */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.0f, 0.f, 1.f, 0.f))))
        return E_FAIL;

    /* Target_Normal */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* Target_Depth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 1.f))))
        return E_FAIL;

    /* Target_LightDepth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    /* Target_SpecularMap */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SpecularMap"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Shade */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* Target_Specular */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Emissive */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Emissive"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Roughness */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Roughness"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Metalic */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Metalic"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* MRT_GameObjects */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_SpecularMap"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Roughness"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Metalic"))))
        return E_FAIL;

    /* MRT_ShadowObject */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ShadowObjects"), TEXT("Target_LightDepth"))))
        return E_FAIL;

    /* MRT_LightAcc */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
        return E_FAIL;

#pragma region MRT_Effect

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Decal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Decal"), TEXT("Target_Decal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DecalResult"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DecalResult"), TEXT("Target_DecalResult"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DownSample4x4"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DownSample4x4"), TEXT("Target_DownSample4x4"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DownSample4x4_2"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DownSample4x4_2"), TEXT("Target_DownSample4x4_2"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DownSample5x5"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DownSample5x5"), TEXT("Target_DownSample5x5"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Blur"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurX"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurX"), TEXT("Target_BlurX"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurY"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurY"), TEXT("Target_BlurY"))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom"), TEXT("Target_Bloom"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom2"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom2"), TEXT("Target_Bloom2"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom3"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom3"), TEXT("Target_Bloom3"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurX1"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurX1"), TEXT("Target_BlurX1"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurY1"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurY1"), TEXT("Target_BlurY1"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurX2"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurX2"), TEXT("Target_BlurX2"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurY2"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurY2"), TEXT("Target_BlurY2"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurX3"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurX3"), TEXT("Target_BlurX3"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurY3"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurY3"), TEXT("Target_BlurY3"))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
        return E_FAIL;

    m_pDistortionTex = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Distortion/Distortion%d.png"), 5);
    if (nullptr == m_pDistortionTex)
        return E_FAIL;

    /* Target_LUT */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LUT"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LUT"), TEXT("Target_LUT"))))
        return E_FAIL;

    m_pLUTTex = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Engine/Bin/Textures/LUT/T_LUT_%d.png"), 2);
    if (nullptr == m_pLUTTex)
        return E_FAIL;

#pragma endregion MRT_Effect

#pragma region MRT_Result

    /* Target_Result */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Result"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Result"), TEXT("Target_Result"))))
        return E_FAIL;

#pragma endregion

    m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;


    //const char* bloomPassNames[6] = { "BlurX1", "BlurY1", "BlurX2", "BlurY2", "BlurX3", "BlurY3" };
    //m_pBloomComputeShader = CComputeShader_Texture::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Blur.hlsl"), 6, &bloomPassNames[0]);
    //if (nullptr == m_pBloomComputeShader)
    //	return E_FAIL;


    /* 화면을 꽉 채워주기 위한 월드변환행렬. */
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

    if (nullptr == m_pDevice)
        return E_FAIL;

    ID3D11Texture2D* pDepthStencilTexture = nullptr;



    D3D11_TEXTURE2D_DESC   TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = g_iSizeX;
    TextureDesc.Height = g_iSizeY;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
        /*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
        return E_FAIL;

    /* RenderTarget */
    /* ShaderResource */
    /* DepthStencil */

    if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthStencilView)))
        return E_FAIL;

    Safe_Release(pDepthStencilTexture);

#pragma region 계층적 Z 버퍼 구현
    //PrevDepth
    // 이전 프레임 깊이 버퍼를 위한 렌더 타겟 생성
    //if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PrevDepth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
    //    return E_FAIL;

    //// MRT에 이전 프레임 깊이 타겟 추가
    //if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PrevDepth"), TEXT("Target_PrevDepth"))))
    //    return E_FAIL;

 
    const char* hzbPassNames[] = { "CS_BuildHZB" };
    m_pHZBComputeShader = CComputeShader_Texture::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_HZB.hlsl"), 1, hzbPassNames);
    if (nullptr == m_pHZBComputeShader)
        return E_FAIL;

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
    texDesc.Width = ViewportDesc.Width;
    texDesc.Height = ViewportDesc.Height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  //기존 깊이버퍼랑 맞춰야하나
    texDesc.SampleDesc.Quality = 0;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;  // GPU 읽기/쓰기 가능
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;  // SRV 생성 가능
    texDesc.CPUAccessFlags = 0;  // CPU 액세스 없음
    texDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pPrevDepthTexture)))
        return E_FAIL;

    // m_pPrevDepthTexture에 대한 SRV 생성
    //D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    //ZeroMemory(&srvDesc, sizeof(srvDesc));
    //srvDesc.Format = texDesc.Format;
    //srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    //srvDesc.Texture2D.MipLevels = 1;
    //srvDesc.Texture2D.MostDetailedMip = 0;
    if (FAILED(m_pDevice->CreateShaderResourceView(m_pPrevDepthTexture, nullptr, &m_pPrevDepthSRV)))
        return E_FAIL;


    ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Width = ViewportDesc.Width;
    texDesc.Height = ViewportDesc.Height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R32_FLOAT;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    for (UINT i = 0; i < MAX_MIP_LEVELS; ++i)
    {
        if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pHZBTexture[i])))
            return E_FAIL;

        // SRV 생성
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;

        if (FAILED(m_pDevice->CreateShaderResourceView(m_pHZBTexture[i], /*&srvDesc*/nullptr, &m_pHZBSRV[i])))
            return E_FAIL;

        // UAV 생성
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        ZeroMemory(&uavDesc, sizeof(uavDesc));
        uavDesc.Format = texDesc.Format;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Texture2D.MipSlice = 0;

        if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pHZBTexture[i], &uavDesc, &m_pHZBUAV[i])))
            return E_FAIL;

        // 다음 밉맵 레벨을 위해 크기 조정
        texDesc.Width = max(texDesc.Width / 2, 1);
        texDesc.Height = max(texDesc.Height / 2, 1);
    }

#pragma endregion 계층적 Z 버퍼 끝

#ifdef _DEBUG
        const float startX = 100.0f;
        const float startY = 50.0f;
        const float targetWidth = 100.0f;
        const float targetHeight = 100.0f;
        const float gap = 0.0f;

        float currentX = startX;
        float currentY = startY;


    if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_DecalResult"), currentX, currentY, targetWidth, targetHeight)))
       return E_FAIL;
    currentX += targetWidth + gap;


        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_DecalResult"), currentX, currentY, targetWidth, targetHeight)))
        //   return E_FAIL;
        //currentX += targetWidth + gap;

        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_DownSample4x4"), currentX, currentY, targetWidth, targetHeight)))
        //   return E_FAIL;
        //currentX += targetWidth + gap;

        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_DownSample4x4_2"), currentX, currentY, targetWidth, targetHeight)))
        //   return E_FAIL;
        //currentX += targetWidth + gap;

        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_DownSample5x5"), currentX, currentY, targetWidth, targetHeight)))
        //   return E_FAIL;
        //currentX += targetWidth + gap;

        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BlurY1"), currentX, currentY, targetWidth, targetHeight)))
        //   return E_FAIL;
        //currentX += targetWidth + gap;

        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Bloom"), currentX, currentY, targetWidth, targetHeight)))
        //   return E_FAIL;
        //currentX += targetWidth + gap;

        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Distortion"), currentX, currentY, targetWidth, targetHeight)))
        //   return E_FAIL;
        if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Diffuse"), currentX, currentY, targetWidth, targetHeight)))
            return E_FAIL;
        currentX += targetWidth + gap;

        if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Normal"), currentX, currentY, targetWidth, targetHeight)))
            return E_FAIL;
        currentX += targetWidth + gap;

    //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Depth"), currentX, currentY, targetWidth, targetHeight)))
    //    return E_FAIL;
    //currentX += targetWidth + gap;

        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_SpecularMap"), currentX, currentY, targetWidth, targetHeight)))
        //    return E_FAIL;
        //currentX += targetWidth + gap;

        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Emissive"), currentX, currentY, targetWidth, targetHeight)))
        //    return E_FAIL;
        //currentX += targetWidth + gap;

        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Roughness"), currentX, currentY, targetWidth, targetHeight)))
        //    return E_FAIL;
        //currentX += targetWidth + gap;

        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Metalic"), currentX, currentY, targetWidth, targetHeight)))
        //    return E_FAIL;
        //currentX += targetWidth + gap;


        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Shade"), currentX, currentY, targetWidth, targetHeight)))
        //    return E_FAIL;
        //currentX += targetWidth + gap;

        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Specular"), currentX, currentY, targetWidth, targetHeight)))
        //    return E_FAIL;
        //currentX += targetWidth + gap;

        //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_DeferredResult"), currentX, currentY, targetWidth, targetHeight)))
        //   return E_FAIL;
        //currentX += targetWidth + gap;

        /*if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Blur"), currentX, currentY, targetWidth, targetHeight)))
           return E_FAIL;
        currentX += targetWidth + gap;

        if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Brightness"), currentX, currentY, targetWidth, targetHeight)))
           return E_FAIL;
        currentX += targetWidth + gap;

        if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Glow"), currentX, currentY, targetWidth, targetHeight)))
           return E_FAIL;
        currentX += targetWidth + gap;

        if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Final"), currentX, currentY, targetWidth, targetHeight)))
           return E_FAIL;*/

           //if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_LightDepth"), currentX, currentY, targetWidth, targetHeight)))
           //   return E_FAIL;


#endif	
        size_t iNumThreadPool = PxThread::getNbPhysicalCores();
        m_pRenderWorker = CRenderWorker::Create(iNumThreadPool, m_pDevice);
        if (nullptr == m_pRenderWorker)
            return E_FAIL;


        

        return S_OK;
    
}

HRESULT CRenderer::Add_RenderObject(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
    m_RenderGroup[eRenderGroup].push_back(pRenderObject);

    Safe_AddRef(pRenderObject);

    return S_OK;
}

void CRenderer::Clear()
{
    for (size_t i = 0; i < RENDER_END; i++)
    {
        for (auto& pGameObject : m_RenderGroup[i])
            Safe_Release(pGameObject);

        m_RenderGroup[i].clear();
    }

#ifdef _DEBUG
    for (auto& DebugObject : m_DebugComponents)
        Safe_Release(DebugObject);
    m_DebugComponents.clear();
#endif

}

void CRenderer::Draw()
{
    vector<future<void>> futures;
    
 
    //futures.push_back(m_pRenderWorker->Add_Job([this]() {
    //    PROFILE_CALL("Render Priority", Render_Priority());
    //    }));
	PROFILE_CALL("Render Priority", Render_Priority());
    
    //futures.push_back(m_pRenderWorker->Add_Job([this]() {
    //    PROFILE_CALL("Render ShadowObjects", Render_ShadowObjects());
    //    }));

	PROFILE_CALL("Render ShadowObjects", Render_ShadowObjects());



    for (auto& future : futures)
        future.get();

    m_pRenderWorker->ExecuteCommandLists(m_pContext);

	PROFILE_CALL("Render NonBlend", Render_NonBlend());
   


	PROFILE_CALL("Render Decal", Render_Decal());

	PROFILE_CALL("Render LightAcc", Render_LightAcc());

   

	PROFILE_CALL("Render DeferredResult", Render_DeferredResult());

	PROFILE_CALL("Render Render_NonLight", Render_NonLight());
	PROFILE_CALL("Render Blend", Render_Blend());
	PROFILE_CALL("Render Blur", Render_Blur());
	PROFILE_CALL("Render Bloom", Render_Bloom());
	PROFILE_CALL("Render Distortion", Render_Distortion());

	PROFILE_CALL("Render Final", Render_Final());
	PROFILE_CALL("Render UI", Render_UI());

#ifdef _DEBUG
	PROFILE_CALL("Render Debug", Render_Debug());
#endif

    //////FOR OCCULUSION CULLING
    //if (FAILED(m_pGameInstance->Copy_Resource(TEXT("Target_Depth"), m_pPrevDepthTexture)))
    //    return;

    //PROFILE_CALL("Render HZB", Update_HZB());

   

    int tmep = 0;
    

}
#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pComponent)
{
    m_DebugComponents.push_back(pComponent);

    Safe_AddRef(pComponent);

    return S_OK;
}
#endif
void CRenderer::Render_Priority()
{
    m_pGameInstance->Begin_MRT(TEXT("MRT_Result"));

    for (auto& pGameObject : m_RenderGroup[RENDER_PRIORITY])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        Safe_Release(pGameObject);
    }
    m_RenderGroup[RENDER_PRIORITY].clear();

    m_pGameInstance->End_MRT();
}

void CRenderer::Render_ShadowObjects()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ShadowObjects"), true, m_pLightDepthStencilView)))
        return;

    D3D11_VIEWPORT         ViewPortDesc;
    ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
    ViewPortDesc.TopLeftX = 0;
    ViewPortDesc.TopLeftY = 0;
    ViewPortDesc.Width = (_float)g_iSizeX;
    ViewPortDesc.Height = (_float)g_iSizeY;
    ViewPortDesc.MinDepth = 0.f;
    ViewPortDesc.MaxDepth = 1.f;

    m_pContext->RSSetViewports(1, &ViewPortDesc);


    for (auto& pGameObject : m_RenderGroup[RENDER_SHADOWOBJ])
    {
        if (nullptr != pGameObject)
            pGameObject->Render_LightDepth();

        Safe_Release(pGameObject);
    }
    m_RenderGroup[RENDER_SHADOWOBJ].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return;

    ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
    ViewPortDesc.TopLeftX = 0;
    ViewPortDesc.TopLeftY = 0;
    ViewPortDesc.Width = (_float)1280.0f;
    ViewPortDesc.Height = (_float)720.0f;
    ViewPortDesc.MinDepth = 0.f;
    ViewPortDesc.MaxDepth = 1.f;

    m_pContext->RSSetViewports(1, &ViewPortDesc);
}

void  CRenderer::Render_NonBlend()
{
    /* Diffuse + Normal */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
        return;

    for (auto& pGameObject : m_RenderGroup[RENDER_NONBLEND])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        Safe_Release(pGameObject);
    }
    m_RenderGroup[RENDER_NONBLEND].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return;
}

void CRenderer::Render_Decal()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Decal"))))
        return;

    for (auto& pGameObject : m_RenderGroup[RENDER_DECAL])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        Safe_Release(pGameObject);
    }
    m_RenderGroup[RENDER_DECAL].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return;

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DecalResult"))))
        return;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return;

    _vector v;
    _matrix m = XMMatrixInverse(&v, XMLoadFloat4x4(&m_WorldMatrix));
    _float4x4 mat;
    XMStoreFloat4x4(&mat, m);
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrixInv", &mat)))
        return;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
        return;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
        return;

    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
        return;


    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
        return;
    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return;
    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Decal"), m_pShader, "g_EffectTexture")))
        return;

    m_pShader->Begin(17);

    m_pVIBuffer->Bind_Buffers();

    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return;
}

void CRenderer::Render_LightAcc()
{
    /* Shade */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
        return;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return;



    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
        return;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
        return;

    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
        return;




    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
        return;
    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return;
    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_SpecularMap"), m_pShader, "g_SpecularMapTexture")))
        return;
    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Roughness"), m_pShader, "g_RoughnessTexture")))
        return;
    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Metalic"), m_pShader, "g_MetalicTexture")))
        return;


    m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);

    if (FAILED(m_pGameInstance->End_MRT()))
        return;
}

void CRenderer::Render_DeferredResult()
{
    m_pGameInstance->Begin_MRT(TEXT("MRT_Result"), false);

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
        return;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
        return;
    _float4x4      ViewMatrix, ProjMatrix;

    /* 광원 기준의 뷰 변환행렬. */
    XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(m_vShadowEye, m_vShadowFocus, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
    XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iSizeX / g_iSizeY, 0.1f, 3000.f));

    //매직넘버 던져줌
    if (FAILED(m_pShader->Bind_RawValue("g_fShadowThreshold", &m_fShadowThreshold, sizeof(_float))))
        return;

    if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &ViewMatrix)))
        return;
    if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &ProjMatrix)))
        return;
    
    //if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_DecalResult"), m_pShader, "g_DiffuseTexture")))
    //    return;
    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
        return;
    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
        return;
    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
        return;
    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_LightDepth"), m_pShader, "g_LightDepthTexture")))
        return;
    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return;
    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Emissive"), m_pShader, "g_EmissiveTexture")))
        return;
    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Metalic"), m_pShader, "g_MetalicTexture")))
        return;

    m_pShader->Begin(3);

    m_pVIBuffer->Bind_Buffers();

    m_pVIBuffer->Render();
}

void  CRenderer::Render_NonLight()
{
    for (auto& pGameObject : m_RenderGroup[RENDER_NONLIGHT])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        Safe_Release(pGameObject);
    }
    m_RenderGroup[RENDER_NONLIGHT].clear();
}

//_bool Compare(CGameObject* pSour, CGameObject* pDest)
//{
//   return dynamic_cast<CBlendObject*>(pSour)->Get_CamDistance() > dynamic_cast<CBlendObject*>(pDest)->Get_CamDistance();
//}

void  CRenderer::Render_Blend()
{
    m_RenderGroup[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
        {
            return dynamic_cast<CBlendObject*>(pSour)->Get_CamDistance() > dynamic_cast<CBlendObject*>(pDest)->Get_CamDistance();
        });

    for (auto& pGameObject : m_RenderGroup[RENDER_BLEND])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        Safe_Release(pGameObject);
    }
    m_RenderGroup[RENDER_BLEND].clear();

    m_pGameInstance->End_MRT();
}

void CRenderer::Render_Blur()
{
    m_pGameInstance->Begin_MRT(TEXT("MRT_Blur"));

    for (auto& pGameObject : m_RenderGroup[RENDER_BLUR])
    {
        if (nullptr != pGameObject)
            pGameObject->Render_Blur();

        Safe_Release(pGameObject);
    }
    m_RenderGroup[RENDER_BLUR].clear();

    m_pGameInstance->End_MRT();

    m_pGameInstance->Begin_MRT(TEXT("MRT_BlurX"));

    _uint iBlurNum = 0;
    m_pShader->Bind_RawValue("g_BlurNum", &iBlurNum, sizeof(_uint));
    m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Blur"), m_pShader, "g_EffectTexture");

    m_pShader->Begin(12);

    m_pVIBuffer->Bind_Buffers();

    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();

    m_pGameInstance->Begin_MRT(TEXT("MRT_BlurY"));

    m_pShader->Bind_RawValue("g_BlurNum", &iBlurNum, sizeof(_uint));
    m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BlurX"), m_pShader, "g_EffectTexture");

    m_pShader->Begin(13);

    m_pVIBuffer->Bind_Buffers();

    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();
}

void CRenderer::Render_Bloom()
{
    m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom"));

    for (auto& pGameObject : m_RenderGroup[RENDER_BLOOM])
    {
        if (nullptr != pGameObject)
            pGameObject->Render_Bloom();

        Safe_Release(pGameObject);
    }
    m_RenderGroup[RENDER_BLOOM].clear();

    m_pGameInstance->End_MRT();

    m_pGameInstance->Begin_MRT(TEXT("MRT_DownSample4x4"));

    m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Bloom"), m_pShader, "g_DiffuseTexture");

    m_pShader->Begin(9);

    m_pVIBuffer->Bind_Buffers();

    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();

    // Target_DownSample4x4에 두번째 downsamping된 텍스쳐 넣기
    m_pGameInstance->Begin_MRT(TEXT("MRT_DownSample4x4_2"));

    m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_DownSample4x4"), m_pShader, "g_DiffuseTexture");

    m_pShader->Begin(9);

    m_pVIBuffer->Bind_Buffers();

    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();

    // Target_DownSample4x4에 세번째 downsamping된 텍스쳐 넣기
    m_pGameInstance->Begin_MRT(TEXT("MRT_DownSample5x5"));

    m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_DownSample4x4_2"), m_pShader, "g_DiffuseTexture");

    m_pShader->Begin(10);

    m_pVIBuffer->Bind_Buffers();

    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();



	m_pGameInstance->Begin_MRT(TEXT("MRT_BlurX1"));

	_uint iBlurNum = 3;
	m_pShader->Bind_RawValue("g_BlurNum", &iBlurNum, sizeof(_uint));
	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_DownSample5x5"), m_pShader, "g_EffectTexture");

	m_pShader->Begin(12);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	m_pGameInstance->End_MRT();

	m_pGameInstance->Begin_MRT(TEXT("MRT_BlurY1"));

	iBlurNum = 3;
	m_pShader->Bind_RawValue("g_BlurNum", &iBlurNum, sizeof(_uint));
	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BlurX1"), m_pShader, "g_EffectTexture");

	m_pShader->Begin(13);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	m_pGameInstance->End_MRT();

	// 업샘플링
	m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom"), false);

	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_DownSample4x4_2"), m_pShader, "g_DiffuseTexture");
	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BlurY1"), m_pShader, "g_EffectTexture");



    m_pShader->Begin(11);

    m_pVIBuffer->Bind_Buffers();

    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();




	m_pGameInstance->Begin_MRT(TEXT("MRT_BlurX2"));

	iBlurNum = 2;
	m_pShader->Bind_RawValue("g_BlurNum", &iBlurNum, sizeof(_uint));
	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Bloom"), m_pShader, "g_EffectTexture");

	m_pShader->Begin(12);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	m_pGameInstance->End_MRT();

	m_pGameInstance->Begin_MRT(TEXT("MRT_BlurY2"));

	iBlurNum = 2;
	m_pShader->Bind_RawValue("g_BlurNum", &iBlurNum, sizeof(_uint));
	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BlurX2"), m_pShader, "g_EffectTexture");

	m_pShader->Begin(13);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	m_pGameInstance->End_MRT();

	// 업샘플링
	m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom2"));

	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_DownSample4x4"), m_pShader, "g_DiffuseTexture");
	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BlurY2"), m_pShader, "g_EffectTexture");


    m_pShader->Begin(11);

    m_pVIBuffer->Bind_Buffers();

    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();



	m_pGameInstance->Begin_MRT(TEXT("MRT_BlurX3"));

	iBlurNum = 1;
	m_pShader->Bind_RawValue("g_BlurNum", &iBlurNum, sizeof(_uint));
	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Bloom2"), m_pShader, "g_EffectTexture");

	m_pShader->Begin(12);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	m_pGameInstance->End_MRT();

	m_pGameInstance->Begin_MRT(TEXT("MRT_BlurY3"));

	iBlurNum = 1;
	m_pShader->Bind_RawValue("g_BlurNum", &iBlurNum, sizeof(_uint));
	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BlurX3"), m_pShader, "g_EffectTexture");

	m_pShader->Begin(13);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	m_pGameInstance->End_MRT();

	// 업샘플링
	m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom3"));

	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Result"), m_pShader, "g_DiffuseTexture");
	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BlurY3"), m_pShader, "g_EffectTexture");

    m_pShader->Begin(11);

    m_pVIBuffer->Bind_Buffers();

    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();
}

void CRenderer::Render_Distortion()
{
    m_pGameInstance->Begin_MRT(TEXT("MRT_Effect"));

    for (auto& pGameObject : m_RenderGroup[RENDER_DISTORTION])
    {
        if (nullptr != pGameObject)
            pGameObject->Render_Distortion();

        Safe_Release(pGameObject);
    }
    m_RenderGroup[RENDER_DISTORTION].clear();

    m_pGameInstance->End_MRT();


	m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion"));
	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Effect"), m_pShader, "g_EffectTexture");
	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Bloom3"), m_pShader, "g_DiffuseTexture");
	m_pDistortionTex->Bind_ShaderResource(m_pShader, "g_DistortionTexture", 3);
	m_pShader->Bind_RawValue("g_Time", &m_fTime, sizeof(_float));
	//if (m_pGameInstance->Get_DIKeyState(DIK_Y))
	//{
	m_fTime += 0.001f;
	//}


    m_pVIBuffer->Bind_Buffers();

    m_pShader->Begin(4);

    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();
}

void CRenderer::Render_Final()
{

	m_pGameInstance->Begin_MRT(TEXT("MRT_LUT"));
	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Distortion"), m_pShader, "g_DistortionTexture");
	m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Bloom3"), m_pShader, "g_ResultTexture");
    m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BlurY"), m_pShader, "g_BlurTexture");
	m_pLUTTex->Bind_ShaderResource(m_pShader, "g_LUTTexture", 1);


    m_pVIBuffer->Bind_Buffers();

    // no lut
    if (m_pGameInstance->Get_DIKeyState(DIK_5))
    {
        m_pShader->Begin(15);
    }
    // lut
    else
    {
        m_pShader->Begin(16);
    }

    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_LUT"), m_pShader, "g_ResultTexture");

    m_pVIBuffer->Bind_Buffers();

    if (m_pGameInstance->Get_DIKeyState(DIK_1))
    {
        // reinhard
        m_pShader->Begin(7);
    }
    else if (m_pGameInstance->Get_DIKeyState(DIK_2))
    {
        //aces filmic
        m_pShader->Begin(8);
    }
    else
    {
        m_pShader->Begin(5);
    }
    if (m_pGameInstance->Get_DIKeyState(DIK_3))
    {
        m_fValue -= 0.01f;
    }
    else if (m_pGameInstance->Get_DIKeyState(DIK_4))
    {
        m_fValue += 0.01f;
    }
    if (FAILED(m_pShader->Bind_RawValue("g_Value", &m_fValue, sizeof(_float))))
        return;

    m_pVIBuffer->Render();

    _tchar               m_szFPS[MAX_PATH] = TEXT("");
    _int i = (_int)(m_fValue * 100);
    wsprintf(m_szFPS, TEXT("1. reinhard   2. ACES   3. HDR Decrease   4. HDR Increase\n5. LUT\nValue : %d"), i);
    m_pGameInstance->Render_Font(TEXT("Font_Default"), m_szFPS, _float2(0.f, 200.f), XMVectorSet(1.f, 1.f, 0.f, 1.f));
}

void CRenderer::Compute_HDR()
{

}

void CRenderer::Update_HZB()
{
    D3D11_VIEWPORT ViewportDesc;
    UINT iNumViewports = 1;
    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
    UINT width = static_cast<UINT>(ViewportDesc.Width);
    UINT height = static_cast<UINT>(ViewportDesc.Height);

    // 현재 프레임의 깊이 버퍼를 m_pPrevDepthTexture에 복사
    ID3D11ShaderResourceView* pCurrentDepthSRV = m_pPrevDepthSRV;
    if (pCurrentDepthSRV)
    {
        ID3D11Resource* pCurrentDepthResource = nullptr;
        pCurrentDepthSRV->GetResource(&pCurrentDepthResource);
        m_pContext->CopyResource(m_pPrevDepthTexture, pCurrentDepthResource);
        Safe_Release(pCurrentDepthResource);
    }

    // 첫 번째 레벨 생성
    m_pHZBComputeShader->Begin(0);
    m_pHZBComputeShader->Bind_SRV("gInput", m_pPrevDepthSRV);
    m_pHZBComputeShader->Bind_RawValue("gInputWidth", &width, sizeof(UINT));
    m_pHZBComputeShader->Bind_RawValue("gInputHeight", &height, sizeof(UINT));
    UINT mipLevel = 0;
    m_pHZBComputeShader->Bind_RawValue("gMipLevel", &mipLevel, sizeof(UINT));
    m_pHZBComputeShader->Compute((width + 15) / 16, (height + 15) / 16, 1);

    // 첫 번째 레벨의 결과를 m_pHZBSRV[0]에 저장
    ID3D11ShaderResourceView* pFirstLevelSRV = m_pHZBComputeShader->Get_SRV(0);
    m_pHZBSRV[0] = pFirstLevelSRV;

    // 나머지 밉맵 레벨 생성
    for (UINT i = 1; i < MAX_MIP_LEVELS; ++i)
    {
        width = max(width / 2, 1);
        height = max(height / 2, 1);

        m_pHZBComputeShader->Begin(0);  // 매 반복마다 Begin 호출
        m_pHZBComputeShader->Bind_SRV("gInput", m_pHZBSRV[i - 1]);
        m_pHZBComputeShader->Bind_RawValue("gInputWidth", &width, sizeof(UINT));
        m_pHZBComputeShader->Bind_RawValue("gInputHeight", &height, sizeof(UINT));
        m_pHZBComputeShader->Bind_RawValue("gMipLevel", &i, sizeof(UINT));
        m_pHZBComputeShader->Compute((width + 15) / 16, (height + 15) / 16, 1);

        // 결과를 m_pHZBSRV[i]에 저장
        ID3D11ShaderResourceView* pCurrentLevelSRV = m_pHZBComputeShader->Get_SRV(0);
        m_pHZBSRV[i] = pCurrentLevelSRV;
    }
}

float CRenderer::Sample_HZB(_float2 uv, UINT mipLevel)
{
    if (mipLevel >= MAX_MIP_LEVELS)
        return 1.0f;  // 최대 깊이 반환 

    ID3D11Texture2D* pStagingTexture = nullptr;
    D3D11_TEXTURE2D_DESC stagingDesc;
    m_pHZBTexture[mipLevel]->GetDesc(&stagingDesc);
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.BindFlags = 0;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    stagingDesc.MiscFlags = 0;

    HRESULT hr = m_pDevice->CreateTexture2D(&stagingDesc, NULL, &pStagingTexture);
    if (FAILED(hr))
    {
        return 1.0f;
    }

    // HZB 텍스처에서 스테이징 텍스처로 복사
    m_pContext->CopyResource(pStagingTexture, m_pHZBTexture[mipLevel]);

    // 스테이징 텍스처 매핑
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    hr = m_pContext->Map(pStagingTexture, 0, D3D11_MAP_READ, 0, &mappedResource);
    if (FAILED(hr))
    {
        pStagingTexture->Release();
        return 1.0f;
    }

    UINT width = stagingDesc.Width;
    UINT height = stagingDesc.Height;

    // 범위 검사 추가
    UINT x = min(static_cast<UINT>(uv.x * width), width - 1);
    UINT y = min(static_cast<UINT>(uv.y * height), height - 1);

    float depth = 1.0f;  // 기본값 설정
    if (width > 0 && height > 0 && mappedResource.pData != nullptr)
    {
        float* pData = reinterpret_cast<float*>(mappedResource.pData);
        depth = pData[y * width + x];
    }

    // 언매핑 및 리소스 해제
    m_pContext->Unmap(pStagingTexture, 0);
    pStagingTexture->Release();

    return depth;
}

void  CRenderer::Render_UI()
{
    for (auto& pGameObject : m_RenderGroup[RENDER_UI])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        Safe_Release(pGameObject);
    }
    m_RenderGroup[RENDER_UI].clear();
}

#ifdef _DEBUG

void CRenderer::Render_Debug()
{
    for (auto& pDebugCom : m_DebugComponents)
    {
        pDebugCom->Render();
        Safe_Release(pDebugCom);
    }
    m_DebugComponents.clear();

    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	m_pGameInstance->Render_RTDebug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_RTDebug(TEXT("MRT_ShadowObjects"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_RTDebug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_RTDebug(TEXT("MRT_Distortion"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_RTDebug(TEXT("MRT_DownSample4x4"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_RTDebug(TEXT("MRT_DownSample4x4_2"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_RTDebug(TEXT("MRT_DownSample5x5"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_RTDebug(TEXT("MRT_Bloom"), m_pShader, m_pVIBuffer);

	m_pGameInstance->Render_RTDebug(TEXT("MRT_DecalResult"), m_pShader, m_pVIBuffer);
    //m_pGameInstance->Render_RTDebug(TEXT("MRT_BlurY"), m_pShader, m_pVIBuffer);

}

#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRenderer* pInstance = new CRenderer(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Created : CRenderer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderer::Free()
{
    Safe_Release(m_pGameInstance);

    for (size_t i = 0; i < RENDER_END; i++)
    {
        for (auto& pGameObject : m_RenderGroup[i])
            Safe_Release(pGameObject);

        m_RenderGroup[i].clear();
    }

    Safe_Release(m_pLightDepthStencilView);

    Safe_Release(m_pPrevDepthTexture);
    Safe_Release(m_pPrevDepthSRV);
    Safe_Release(m_pHZBComputeShader);

    for (UINT i = 0; i < MAX_MIP_LEVELS; ++i)
    {
        Safe_Release(m_pHZBSRV[i]);
        Safe_Release(m_pHZBUAV[i]);
        Safe_Release(m_pHZBTexture[i]);

    }

	Safe_Release(m_pShader);
	//Safe_Release(m_pBloomComputeShader);
	Safe_Release(m_pVIBuffer);

    Safe_Release(m_pRenderWorker);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pLUTTex);
    Safe_Release(m_pDistortionTex);
}
