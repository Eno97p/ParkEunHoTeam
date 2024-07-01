#include "UI_StateSoul.h"

#include "GameInstance.h"

CUI_StateSoul::CUI_StateSoul(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{pDevice, pContext}
{
}

CUI_StateSoul::CUI_StateSoul(const CUI_StateSoul& rhs)
    : CUI{rhs}
{
}

HRESULT CUI_StateSoul::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_StateSoul::Initialize(void* pArg)
{
	UI_SOUL_DESC* pDesc = static_cast<UI_SOUL_DESC*>(pArg);

	m_isInv = pDesc->isInv;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Data();
	Setting_Position();

	return S_OK;
}

void CUI_StateSoul::Priority_Tick(_float fTimeDelta)
{
}

void CUI_StateSoul::Tick(_float fTimeDelta)
{
}

void CUI_StateSoul::Late_Tick(_float fTimeDelta)
{
	if(m_isInv)
		CGameInstance::GetInstance()->Add_UI(this, NINETH);
	else
		CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_StateSoul::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	_tchar wszSoul[MAX_PATH] = TEXT("");
	wsprintf(wszSoul, TEXT("%d"), 52); // 크기 키우고 올려야함(나중)

	//m_pGameInstance->Render_Font(TEXT("Font_Cardo"), wszSoul, _float2(m_fX - 50.f, m_fY), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CUI_StateSoul::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateSoul"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StateSoul::Bind_ShaderResources()
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

void CUI_StateSoul::Setting_Data()
{
	if (m_isInv)
	{
		m_fX = 250.f;
		m_fY = 120.f;
		m_fSizeX = 32.f;
		m_fSizeY = 32.f;
	}
	else
	{
		m_fX = 200.f;
		m_fY = 100.f;
		m_fSizeX = 32.f;
		m_fSizeY = 32.f;
	}
}

CUI_StateSoul* CUI_StateSoul::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_StateSoul* pInstance = new CUI_StateSoul(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_StateSoul");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_StateSoul::Clone(void* pArg)
{
	CUI_StateSoul* pInstance = new CUI_StateSoul(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_StateSoul");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_StateSoul::Free()
{
	__super::Free();
}
