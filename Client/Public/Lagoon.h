#pragma once

#include "Map_Element.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CLagoon final : public CMap_Element
{
private:
	enum TEXS { TEX_NORMAL1, TEX_NORMAL2, TEX_CAUSTIC, TEX_END };
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
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_Mirror() override;

public:
	_float4 m_vLightPosition = { -389.486f, 485.690f, 35.512f, 1.f };
	_float m_fLightRange = 1000.f; // 포인트 라이트의 범위

	_float4 m_vLightDir = { -1.f, -1.f, -1.f, 0.f };
	_float4 m_vLightDiffuse = { 1.f, 1.f, 1.f, 1.f };
	_float4 m_vLightAmbient = { 0.574f, 0.812f, 1.f, 1.f };
	_float4 m_vLightSpecular = { 1.f, 1.f, 1.f, 1.f };
	_float4 m_vMtrlSpecular = { 1.f, 1.f, 1.f, 1.f };

	_float m_fBloomIntensity = 0.65f;    // 물의 기본 알파값
	_float m_fBloomThreshold = 0.524752f;    // 물의 깊이 (미터 단위)

	_float m_fFlowSpeed = 0.3f;
	_float m_fFresnelStrength = 0.399f;
	_float3 m_vSkyColor = { 0.7f, 0.9f, 1.0f };
	_bool m_bDiffuse = true;
	_bool m_bNormal = true;
	_bool m_bSpecular = true;
	_float m_fRoughness = 0.22f;
	_float m_fNormalStrength0 = 0.663f; // 첫 번째 노멀 맵의 강도
	_float m_fNormalStrength1 = 0.756f; // 두 번째 노멀 맵의 강도
	_float m_fNormalStrength2 = 0.330f; // 세 번째 노멀 맵의 강도
	_float m_fCausticStrength = 1.f; // Caustic 노이즈의 강도

	_float m_fWaterAlpha = 0.95f;    // 물의 기본 알파값
	_float m_fWaterDepth = 2.0f;    // 물의 깊이 (미터 단위)



private:
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pNoiseCom = { nullptr };
	CTexture* m_pTexture[TEX_END] = { nullptr };
	//CPhysXComponent_static* m_pPhysXCom = { nullptr };
private:
	_vector m_vTargetPos;
	_float m_fAccTime = 0.f;

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

private:
	float Lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}

	float Clamp(float value, float min, float max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

private:
	_float m_fElevateTimer = 0.f;
	_uint m_iTest = 0;


	_float3 m_vPivotPos = { 0.f, 0.f, 0.f };

public:
	static CLagoon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END