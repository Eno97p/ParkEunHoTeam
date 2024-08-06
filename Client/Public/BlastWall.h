#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CShader;
class CModel;
class CPhysXComponent;

END


BEGIN(Client)

class CBlastWall final : public CGameObject
{
private:
	CBlastWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlastWall(const CBlastWall& rhs);
	virtual ~CBlastWall() = default;

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


private:
	CModel* m_pModelCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CPhysXComponent** m_pPhysXCom = { nullptr };
	_uint	m_iNumMeshes = 0;
	vector<_float4x4> m_vecMeshsTransforms;
public:
	static CBlastWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END