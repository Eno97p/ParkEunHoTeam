#pragma once

#include "Base.h"

class CCascade final : public CBase
{
private:
    CCascade();
    virtual ~CCascade() = default;

private:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
    void Tick(const XMMATRIX& viewMatrix, const XMMATRIX& projMatrix);
    void RenderShadowMaps();
    void BindShadowMaps();

private:
    void CreateShadowMapTextures(UINT shadowMapSize);
    void CalculateFrustumCorners(float nearZ, float farZ, float fov, float aspectRatio, XMVECTOR* corners);
    XMMATRIX CalculateLightViewProjection(const XMVECTOR* frustumCorners);

    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    UINT m_uCascadeCount = 4;
    vector<XMMATRIX> m_LightViewProj;
    vector<float> m_CascadeEnds;
    vector<ID3D11Texture2D*> m_pShadowMapTextures;
    vector<ID3D11DepthStencilView*> m_pShadowMapDSVs;
    vector<ID3D11ShaderResourceView*> m_pShadowMapSRVs;
    ID3D11SamplerState* m_pShadowMapSampler = nullptr;
    ID3D11Buffer* m_pCascadeConstantBuffer = nullptr;

public:
    static CCascade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};