#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Effect)
class CElectricCylinder final : public CBlendObject
{
public:
	typedef struct ANDRAS_ELECTRIC_DESC
	{
		_float3 vMaxSize = { 1.f,1.f,3.f };
		_float3 vOffset = { 0.f,1.f,1.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float fRotationSpeed = 1000.f;
		_float fMaxLifeTime = 0.f;
		_float frameSpeed = 10.f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CElectricCylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElectricCylinder(const CElectricCylinder& rhs);
	virtual ~CElectricCylinder() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_Distortion() override;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom[2] = {nullptr,nullptr};

private:
	shared_ptr<ANDRAS_ELECTRIC_DESC> m_OwnDesc;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;
	_float3						m_CurrentSize{};

	_float						m_CurFrame = 0.f;
	_float						m_MaxFrame = 16.f;
	_float						m_FrameRatio = 0.f;

public:
	static CElectricCylinder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END