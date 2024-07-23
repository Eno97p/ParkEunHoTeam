#pragma once
#include "Active_Element.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CTreasureChest final : public CActive_Element
{
private:
	CTreasureChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTreasureChest(const CTreasureChest& rhs);
	virtual ~CTreasureChest() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pNoiseCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CPhysXComponent_static* m_pPhysXCom = { nullptr };
private:
	_vector m_vTargetPos;
	_float m_fReboundTimer = 0.f;

	_bool m_bChestOpened = false;

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();


private:
	_uint m_iTest = 0;

public:
	static CTreasureChest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END