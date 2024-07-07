#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXComponent_Vehicle;
END


BEGIN(Client)

class CHoverboard final : public CGameObject
{
private:
	CHoverboard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHoverboard(const CHoverboard& rhs);
	virtual ~CHoverboard() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Add_PxActor();

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CPhysXComponent_Vehicle* m_pPhysXCom = { nullptr };
private:


public:
	static CHoverboard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
