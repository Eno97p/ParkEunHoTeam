#pragma once

#include "ToolObj.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Instance_Point;
class CCollider;
END

BEGIN(MYMapTool)

class CCloud final : public CToolObj
{
public:
private:
	CCloud(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCloud(const CCloud& rhs);
	virtual ~CCloud() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pNoiseCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CPhysXComponent_static* m_pPhysXCom = { nullptr };
	CComputeShader_Texture* m_pCloudCS = { nullptr };
	ID3D11Texture3D* m_pNoiseTexture = nullptr;
	ID3D11ShaderResourceView* m_pNoiseSRV = nullptr;
	ID3D11Texture3D* m_pDensityTexture = nullptr;
	ID3D11ShaderResourceView* m_pDensitySRV = nullptr;
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
	float m_fAccTime = 0.f;
	float m_fWindStrength = 1.f;
	float m_fTargetWindStrength = 1.f;
	float m_fWindChangeTime = 0.f;
	const float m_fWindChangeDuration = 5.f;

	_float3 m_vPivotPos = { 0.f, 0.f, 0.f };

public:
	// 구름 관련 변수들
	float m_fCloudDensity = 0.8f;
	float m_fCloudScale = 0.01f;
	float m_fCloudSpeed = 0.1f;
	float m_fCloudHeight = 0.1f;
	_float3 m_vCloudColor = _float3(0.6f, 0.6f, 0.6f);

	// 조명 관련 변수
	_float4 m_vLightDiffuse = _float4(0.76f, 0.8f, 0.95f, 1.0f);
	_float4 m_vLightAmbient = _float4(0.2f, 0.2f, 0.3f, 1.0f);
	_float4 m_vLightSpecular = _float4(1.0f, 1.0f, 1.0f, 1.0f);
	_float4 m_vLightDir = _float4(-1.0f, -1.0f, -1.0f, 0.0f);
	_float4 m_vLightPosition;
	float m_fLightRange = 100.0f;

	// Sphere Tracing 관련 변수
	float m_fSphereTracingThreshold = 0.01f;
	float m_fMaxRayDistance = 1000.0f;
	int m_iMaxSteps = 128;

	// 레이마칭 관련 변수
	float m_fStepSize = 0.5f;

	// 노이즈 관련 변수
	int m_iPerlinOctaves = 7;
	float m_fPerlinFrequency = 4.0f;
	float m_fWorleyFrequency = 4.0f;

public:
	float m_fCoarseStepSize = 2.0f;
	float m_fFineStepSize = 0.5f;
	int m_iMaxCoarseSteps = 50;
	int m_iMaxFineSteps = 64;
	float m_fDensityThreshold = 0.05f;
	float m_fAlphaThreshold = 0.95f;

public:
	static CCloud* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END