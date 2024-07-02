#pragma once

#include "GameObject.h"
#include "AnimTool_Defines.h"

BEGIN(Engine)
class CTransform;
class CShader;
class CModel;
END

BEGIN(AnimTool)

class CToolPartObj final : public CGameObject
{
public:
	typedef struct PartObj_Desc : public GAMEOBJECT_DESC
	{
		const _float4x4*	pCombinedTransformationMatrix;
		const _float4x4*	pParentMatrix;
	}PARTOBJ_DESC;

private:
	CToolPartObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolPartObj(const CToolPartObj& rhs);
	virtual ~CToolPartObj() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float4x4				m_WorldMatrix;
	const _float4x4*		m_pParentMatrix = { nullptr };

	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	const _float4x4*		m_pSocketMatrix = { nullptr };

private:
	HRESULT			Add_Components();
	HRESULT			Bind_ShaderResources();

public:
	static CToolPartObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END