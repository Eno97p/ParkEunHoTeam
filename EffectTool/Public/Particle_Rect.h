#pragma once

#include "Effect_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Instance_Rect;
END

BEGIN(Effect)

class CParticle_Rect final : public CGameObject
{
public:
	typedef struct PARTICLERECT : public CGameObject::GAMEOBJECT_DESC
	{
		_float4									 vStartPos;
		EFFECTTYPE								 eType;
		_uint									 TextureNumber;
		CVIBuffer_Instance::INSTANCE_DESC		 InstanceDesc;
		wstring									 Texture;
		_bool									 IsBlur = false;
	};
private:
	CParticle_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Rect(const CParticle_Rect& rhs);
	virtual ~CParticle_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_Target(CGameObject* pTarget)
	{
		m_pTarget = pTarget;
	}

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };	
	CVIBuffer_Instance_Rect*		m_pVIBufferCom = { nullptr };	
	EFFECTTYPE		m_eType;
	CVIBuffer_Instance::INSTANCE_DESC m_InstanceDesc;
	_bool m_IsBlur = false;
	_uint m_TextureNum = 0;
	CGameObject* m_pTarget = nullptr;

private:
	HRESULT Add_Components(const wstring& Texcom);
	HRESULT Bind_ShaderResources();

public:
	static CParticle_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END