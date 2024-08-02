#include "UI_CharacterBG.h"

#include "GameInstance.h"
#include "Player.h"

CUI_CharacterBG::CUI_CharacterBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_CharacterBG::CUI_CharacterBG(const CUI_CharacterBG& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_CharacterBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_CharacterBG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	Setting_Position();

	return S_OK;
}

void CUI_CharacterBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_CharacterBG::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	Update_Data();
}

void CUI_CharacterBG::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, EIGHT);
}

HRESULT CUI_CharacterBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	Render_Text();

	return S_OK;
}

void CUI_CharacterBG::Update_Data()
{
	// Player의 값들을 받아와서 wstring에 갱신해주기
	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());

	m_wstrVitality = to_wstring(0);
	m_wstrStamina = to_wstring(0);
	m_wstrForce = to_wstring(0);

	m_wstrHealth = to_wstring((_uint)pPlayer->Get_MaxHP());
	m_wstrStamina_State = to_wstring((_uint)pPlayer->Get_MaxStamina());
	m_wstrEther = to_wstring((_uint)pPlayer->Get_MaxMP());

	m_wstrPhysicalDmg = to_wstring(0);
	m_wstrEtheralDmg = to_wstring(0);


}

HRESULT CUI_CharacterBG::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_CharacterBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CharacterBG::Bind_ShaderResources()
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

void CUI_CharacterBG::Render_Text()
{
	_float fFirstColX = 270.f;
	_float fFirstRowX = (g_iWinSizeY >> 1) - 65.f;
	_float fSecColX = (g_iWinSizeX >> 1) + 10.f;
	_float fSecColY = 227.f;

	// WANDERER
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), TEXT("WANDERER"), _float2(220.f, 105.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo13"), TEXT("New level:"), _float2(fFirstColX, 195.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo13"), TEXT("Essence:"), _float2(fFirstColX, 215.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;


	// 좌측
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("VITALITY"), _float2(fFirstColX, fFirstRowX), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), to_wstring(0), _float2(fFirstColX + 280.f, fFirstRowX), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("STAMINA"), _float2(fFirstColX, fFirstRowX + 30.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), to_wstring(0), _float2(fFirstColX + 280.f, fFirstRowX + 30.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("FORCE"), _float2(fFirstColX, fFirstRowX + 60.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), to_wstring(0), _float2(fFirstColX + 280.f, fFirstRowX + 60.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("MYSTIC"), _float2(fFirstColX, fFirstRowX + 90.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), to_wstring(0), _float2(fFirstColX + 280.f, fFirstRowX + 90.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("KNOWLEDGE"), _float2(fFirstColX, fFirstRowX + 120.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), to_wstring(0), _float2(fFirstColX + 280.f, fFirstRowX + 120.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;

	// 우측
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("HELTH"), _float2(fSecColX, fSecColY), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), m_wstrHealth, _float2(fSecColX + 330.f, fSecColY), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("STAMINA"), _float2(fSecColX, fSecColY + 28.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), m_wstrStamina_State, _float2(fSecColX + 330.f, fSecColY + 28.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("ETHER"), _float2(fSecColX, fSecColY + 56.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), m_wstrEther, _float2(fSecColX + 330.f, fSecColY + 56.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("PHYSICAL DAMAGE"), _float2(fSecColX, fSecColY + 108.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), m_wstrPhysicalDmg, _float2(fSecColX + 330.f, fSecColY + 108.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("ETHERAL DAMAGE"), _float2(fSecColX, fSecColY + 134.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), m_wstrEtheralDmg, _float2(fSecColX + 330.f, fSecColY + 134.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("PHYSICAL ARMOR"), _float2(fSecColX, fSecColY + 188.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), to_wstring(0), _float2(fSecColX + 330.f, fSecColY + 188.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), TEXT("ETHERAL ARMOR"), _float2(fSecColX, fSecColY + 213.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), to_wstring(0), _float2(fSecColX + 330.f, fSecColY + 213.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;

}

CUI_CharacterBG* CUI_CharacterBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CharacterBG* pInstance = new CUI_CharacterBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_CharacterBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_CharacterBG::Clone(void* pArg)
{
	CUI_CharacterBG* pInstance = new CUI_CharacterBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_CharacterBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_CharacterBG::Free()
{
	__super::Free();
}
