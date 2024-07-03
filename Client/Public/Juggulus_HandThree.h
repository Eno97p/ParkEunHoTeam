#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CJuggulus_HandThree final : public CPartObject
{
private:
	CJuggulus_HandThree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJuggulus_HandThree(const CJuggulus_HandThree& rhs);
	virtual ~CJuggulus_HandThree() = default;

public:
	virtual bool	Get_AnimFinished() { return m_isAnimFinished; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Distortion();
	virtual HRESULT Render_LightDepth() override;

private:
	_bool		m_isRender = { false };

	CCollider*	m_pColliderCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CTexture*	m_pTextureCom = { nullptr };

	_bool			m_isAnimFinished = { false };
	_uint			m_iPastAnimIndex = 0;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	void					Change_Animation(_float fTimeDelta);

public:
	static CJuggulus_HandThree*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END