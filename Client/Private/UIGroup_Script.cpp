#include "UIGroup_Script.h"

#include "GameInstance.h"

#include "UI_ScriptBG_Aura.h"
#include "UI_ScriptBG_Npc.h"
#include "UI_Script_DialogBox.h"
#include "UI_Script_NameBox.h"

CUIGroup_Script::CUIGroup_Script(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Script::CUIGroup_Script(const CUIGroup_Script& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_Script::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Script::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_Script::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Script::Tick(_float fTimeDelta)
{
	_bool isRender_End = false;
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
		{
			if (!m_isRenderOnAnim && !(pUI->Get_RenderOnAnim()))
			{
				pUI->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pUI->Get_RenderOnAnim())
			{
				pUI->Resset_Animation(false);
			}

			pUI->Tick(fTimeDelta);

			isRender_End = pUI->isRender_End();
		}
		if (isRender_End)
			m_isRend = false;

		m_pDialogBox->Tick(fTimeDelta);
	}
}

void CUIGroup_Script::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);

		m_pDialogBox->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_Script::Render()
{
	return S_OK;
}

void CUIGroup_Script::Set_DialogText(wstring wstrDialogText)
{
	m_pDialogBox->Setting_Text(wstrDialogText);
}

HRESULT CUIGroup_Script::Create_UI()
{
	CUI::UI_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;

	// BG Aura
	m_vecUI.emplace_back(dynamic_cast<CUI_ScriptBG_Aura*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ScriptBG_Aura"), &pDesc)));
	
	// BG Npc
	m_vecUI.emplace_back(dynamic_cast<CUI_ScriptBG_Npc*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ScriptBG_Npc"), &pDesc)));

	// DialogBox
	m_pDialogBox = dynamic_cast<CUI_Script_DialogBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Script_DialogBox"), &pDesc));

	// NameBox
	CUI_Script_NameBox::UI_SCRIPT_NAMEBOX_DESC pNameDesc{};
	pNameDesc.eLevel = LEVEL_STATIC;
	pNameDesc.eNpcType = CUI_Script_NameBox::NPC_RLYA; // 나중에 수정할 수 있도록 변경해야 함
	m_vecUI.emplace_back(dynamic_cast<CUI_Script_NameBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ScriptBG_NameBox"), &pNameDesc)));

	return S_OK;
}

CUIGroup_Script* CUIGroup_Script::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_Script* pInstance = new CUIGroup_Script(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_Script");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_Script::Clone(void* pArg)
{
	CUIGroup_Script* pInstance = new CUIGroup_Script(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_Script");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_Script::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);

	Safe_Release(m_pDialogBox);
}
