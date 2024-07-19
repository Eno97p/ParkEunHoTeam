#include "UI_StateEther.h"

#include "GameInstance.h"
#include "Player.h"

CUI_StateEther::CUI_StateEther(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{pDevice, pContext}
{
}

CUI_StateEther::CUI_StateEther(const CUI_StateEther& rhs)
	: CUI{rhs}
{
}

HRESULT CUI_StateEther::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_StateEther::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = 390.f;
	m_fY = 75.f;
	m_fSizeX = 768.f;
	m_fSizeY = 24.f;

	Setting_Position();

	return S_OK;
}

void CUI_StateEther::Priority_Tick(_float fTimeDelta)
{
}

void CUI_StateEther::Tick(_float fTimeDelta)
{
}

void CUI_StateEther::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, THIRD);
}

HRESULT CUI_StateEther::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_StateEther::Resset_Player()
{
	//Safe_Release(m_pPlayer);
	//m_pPlayer = nullptr;
}

HRESULT CUI_StateEther::Add_Components()
{
	/* For. Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateEther"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StateEther::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CUI_StateEther* CUI_StateEther::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_StateEther* pInstance = new CUI_StateEther(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_StateEther");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_StateEther::Clone(void* pArg)
{
	CUI_StateEther* pInstance = new CUI_StateEther(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_StateEther");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_StateEther::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
