#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CCorruptedSword final : public CWeapon
{
private:
	CCorruptedSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCorruptedSword(const CCorruptedSword& rhs);
	virtual ~CCorruptedSword() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CCorruptedSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END