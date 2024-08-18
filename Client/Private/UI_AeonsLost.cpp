#include "UI_AeonsLost.h"

#include "GameInstance.h"

CUI_AeonsLost::CUI_AeonsLost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_AeonsLost::CUI_AeonsLost(const CUI_AeonsLost& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_AeonsLost::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_AeonsLost::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = (g_iWinSizeX >> 1);
	m_fY = (g_iWinSizeY >> 1);
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	Setting_Position();

	return S_OK;
}

void CUI_AeonsLost::Priority_Tick(_float fTimeDelta)
{
}

void CUI_AeonsLost::Tick(_float fTimeDelta)
{
	m_fDeadTimer += fTimeDelta;
	if (1.f <= m_fDeadTimer) // 1.f
	{
		// 떠 있다가 노이즈 좀 들어가주면서 팍 사라져야 함 >> 노이즈 구현 하고 나서 손댈 지?
		
		m_isEnd = true;
	}
}

void CUI_AeonsLost::Late_Tick(_float fTimeDelta)
{
	if(!m_isEnd)
		CGameInstance::GetInstance()->Add_UI(this, SEVENTEENTH); // 더 추가될 수 있음
}

HRESULT CUI_AeonsLost::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(8); // 8
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo35"), TEXT("AEONS LOST"), _float2(m_fX - 110.f, m_fY - 40.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_AeonsLost::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_AeonsLost"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_AeonsLost::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool)))) // m_isRenderOffAnim
	//	return E_FAIL;

	return S_OK;
}

CUI_AeonsLost* CUI_AeonsLost::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_AeonsLost* pInstance = new CUI_AeonsLost(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_AeonsLost");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_AeonsLost::Clone(void* pArg)
{
	CUI_AeonsLost* pInstance = new CUI_AeonsLost(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_AeonsLost");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_AeonsLost::Free()
{
	__super::Free();
}
