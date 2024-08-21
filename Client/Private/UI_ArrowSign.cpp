#include "UI_ArrowSign.h"

#include "GameInstance.h"

CUI_ArrowSign::CUI_ArrowSign(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_ArrowSign::CUI_ArrowSign(const CUI_ArrowSign& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_ArrowSign::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ArrowSign::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_ArrowSign::Priority_Tick(_float fTimeDelta)
{
}

void CUI_ArrowSign::Tick(_float fTimeDelta)
{
}

void CUI_ArrowSign::Late_Tick(_float fTimeDelta)
{
	m_pTransformCom->BillBoard();

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	CGameInstance::GetInstance()->Add_UI(this, FIRST);
}

HRESULT CUI_ArrowSign::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_ArrowSign::Add_Components()
{
	return E_NOTIMPL;
}

HRESULT CUI_ArrowSign::Bind_ShaderResources()
{
	return E_NOTIMPL;
}

CUI_ArrowSign* CUI_ArrowSign::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CUI_ArrowSign::Clone(void* pArg)
{
	return nullptr;
}

void CUI_ArrowSign::Free()
{
}
