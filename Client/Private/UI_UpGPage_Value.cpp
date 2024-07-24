#include "UI_UpGPage_Value.h"

#include "GameInstance.h"
#include "Inventory.h"

CUI_UpGPage_Value::CUI_UpGPage_Value(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_UpGPage_Value::CUI_UpGPage_Value(const CUI_UpGPage_Value& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_UpGPage_Value::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_UpGPage_Value::Initialize(void* pArg)
{
	UI_VALUE_DESC* pDesc = static_cast<UI_VALUE_DESC*>(pArg);

	m_eValueType = pDesc->eValueType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (VALUE_SOUL == m_eValueType)
	{
		m_fX = (g_iWinSizeX >> 1) - 70.f; //65
		m_fY = (g_iWinSizeY >> 1) + 165.f;
		m_fSizeX = 128.f; // 128   85.3f
	}
	else if (VALUE_MATERIAL == m_eValueType)
	{
		m_fX = (g_iWinSizeX >> 1) + 30.f; // 35
		m_fY = (g_iWinSizeY >> 1) + 165.f;
		m_fSizeX = 100.f; // 128   85.3f
	}


	m_fSizeY = 42.6f; // 64

	Setting_Position();

	return S_OK;
}

void CUI_UpGPage_Value::Priority_Tick(_float fTimeDelta)
{
}

void CUI_UpGPage_Value::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_UpGPage_Value::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, TENTH);
}

HRESULT CUI_UpGPage_Value::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	Rend_Font();

	return S_OK;
}

HRESULT CUI_UpGPage_Value::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_value"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpGPage_Value::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpGPage_Value::Rend_Font()
{
	// 선택한 현재 weapon의 종류에 따라 / 강화 정도에 따라 필요로 하는 재료 값들이 달라지도록 구현 해야 함


	// m_eValueType에 따라 나누고(Soul / Material) Player로부터 >> Soul을 Inventory에 넣기? Player로부터 가져올 필요 없을 듯
	if (VALUE_SOUL == m_eValueType)
	{
		m_wstrText = to_wstring(CInventory::GetInstance()->Get_Soul()) + TEXT(" / ") + TEXT("100");

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo"), m_wstrText, _float2(m_fX - 35.f, m_fY - 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
			return E_FAIL;
	}
	else if (VALUE_MATERIAL == m_eValueType)
	{
		m_wstrText = TEXT("200");

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo13"), m_wstrText, _float2(m_fX - 20.f, m_fY - 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
			return E_FAIL;
	}


	return S_OK;
}

CUI_UpGPage_Value* CUI_UpGPage_Value::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UpGPage_Value* pInstance = new CUI_UpGPage_Value(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_UpGPage_Value");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_UpGPage_Value::Clone(void* pArg)
{
	CUI_UpGPage_Value* pInstance = new CUI_UpGPage_Value(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_UpGPage_Value");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UpGPage_Value::Free()
{
	__super::Free();
}
