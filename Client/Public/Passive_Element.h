#pragma once

#include "Client_Defines.h"
#include "Map_Element.h"

BEGIN(Client)

class CPassive_Element final : public CMap_Element
{
private:
	CPassive_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPassive_Element(const CPassive_Element& rhs);
	virtual ~CPassive_Element() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Mirror() override;

public:
	HRESULT Add_Components(MAP_ELEMENT_DESC* desc);
	HRESULT Bind_ShaderResources();


private:
	vector<CPhysXComponent*> m_pPhysXCom = { nullptr };
	_bool			 m_bNoCullElement = false;
	_uint			 m_iTest = 0;

	_uint			m_iInstanceCount = 0;
public:
	static CPassive_Element* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END