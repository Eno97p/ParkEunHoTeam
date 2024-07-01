#pragma once
#include "Monster.h"

BEGIN(Client)
class CMantari final : public CMonster
{
private:
	CMantari(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMantari(const CMonster& rhs);
	virtual ~CMantari() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT				Add_Components();
	HRESULT				Add_PartObjects();
	virtual HRESULT		Add_Nodes() override;

private:
	vector<CGameObject*>	m_vPartObjects;

public:
	static CMantari* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		Free() override;

};

END