#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBody_Ghost final : public CPartObject
{
private:
	CBody_Ghost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Ghost(const CBody_Ghost& rhs);
	virtual ~CBody_Ghost() = default;

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
	void Set_Weapon(class CWeapon* pWeapon) { m_pWeapon = pWeapon; }

private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };

	_bool		m_isAnimFinished = { false };
	_uint		m_iPastAnimIndex = 0;
	class CWeapon* m_pWeapon = nullptr;
	_float m_fDamageTiming = 0.f;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	void	Change_Animation(_float fTimeDelta);

public:
	static CBody_Ghost*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END