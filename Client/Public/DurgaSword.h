#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CDurgaSword final : public CWeapon
{
private:
	CDurgaSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDurgaSword(const CDurgaSword& rhs);
	virtual ~CDurgaSword() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Reflection() override;
	virtual HRESULT Render_LightDepth() override;
	virtual HRESULT Render_Bloom() override;
	virtual void Set_Active(_bool isActive) override;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDurgaSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END