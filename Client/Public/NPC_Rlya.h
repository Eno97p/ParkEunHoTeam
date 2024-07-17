#pragma once
#include "Npc.h"
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
	// PartObject가 Body 하나뿐이니 하나만 생성해도 될듯

private:
	HRESULT				Add_Components();
	HRESULT				Add_PartObjects();

public:
	static CNPC_Rlya*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

