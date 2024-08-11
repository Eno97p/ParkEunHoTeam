#pragma once
#include "Map_Element.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CLagoon final : public CMap_Element
{
private:
	enum TEXTURE { TEX_WINDGUST, TEX_FOAM_INTENSITY, TEX_FOAM_BUBBLES, TEX_END }; // 텍스쳐 목록

public:
	struct OCEAN_VS_HS_DS_CBUFFER
	{
		XMMATRIX g_WorldMatrix;
		XMMATRIX g_ViewMatrix;
		XMMATRIX g_ProjMatrix;
		XMFLOAT4 g_eyePos;
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
		XMFLOAT2 g_localWavesSimulationDomainWorldspaceCenter;
	};

	struct OCEAN_PS_CBUFFER
	{
		float g_cascadeToCascadeScale;
		float g_windWavesFoamWhitecapsThreshold;
		float g_localWavesFoamWhitecapsThreshold;
		float g_padding;
		XMFLOAT3 g_sunDirection;
		float g_sunIntensity;
	};

	struct OCEAN_VS_CBUFFER_PERINSTANCE_ENTRY
	{
		XMFLOAT2 g_patchWorldspaceOrigin;
		float g_patchWorldspaceScale;
		float g_patchMorphConstantAndSign;
	};
public:
	typedef struct LAGOOON_DESC : public CMap_Element::MAP_ELEMENT_DESC
	{
		_uint iTexNum = 0;
	};

private:
	CLagoon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLagoon(const CLagoon& rhs);
	virtual ~CLagoon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
//virtual HRESULT Render_Bloom() override;

private:
	_uint m_iTexNum = 0;
	CVIBuffer_Rect* m_pVIBufferCom = {nullptr};

	// WaveWorks 관련 멤버 변수
	ID3D11Buffer* m_pOceanVB = nullptr;
	ID3D11Buffer* m_pOceanIB = nullptr;
	ID3D11Buffer* m_pOceanVSHSDSCB = nullptr;
	ID3D11Buffer* m_pOceanPSCB = nullptr;
	ID3D11Buffer* m_pOceanPerInstanceCB = nullptr;


	CTexture* m_pTextureCom[TEX_END] = { nullptr };
	_float m_fTimeDelta = 0.f;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLagoon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END