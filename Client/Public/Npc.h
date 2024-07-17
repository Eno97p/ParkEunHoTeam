#pragma once

#include "LandObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CNpc abstract : public CLandObject
{
public:
	typedef struct Npc_Desc : public LANDOBJ_DESC
	{
		LEVEL		eLevel;
	}NPC_DESC;

protected:
	CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpc(const CNpc& rhs);
	virtual ~CNpc() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	class CPhysXComponent_Character* m_pPhysXCom = { nullptr };

	LEVEL			m_eLevel = { LEVEL_END };


public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void			Free() override;
};

END