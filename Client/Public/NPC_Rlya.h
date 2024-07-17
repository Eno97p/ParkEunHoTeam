#pragma once

#include "Npc.h"

BEGIN(Client)
class CBody_Rlya;

class CNPC_Rlya final : public CNpc
{
private:
	CNPC_Rlya(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Rlya(const CNPC_Rlya& rhs);
	virtual ~CNPC_Rlya() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CBody_Rlya*			m_pBody = { nullptr };

private:
	HRESULT				Add_Components();
	HRESULT				Add_PartObjects();

	void				Check_Distance();

public:
	static CNPC_Rlya*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END