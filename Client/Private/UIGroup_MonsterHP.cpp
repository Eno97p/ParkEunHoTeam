#include "UIGroup_MonsterHP.h"

#include "GameInstance.h"
#include "UI_MonsterHP.h"

CUIGroup_MonsterHP::CUIGroup_MonsterHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_MonsterHP::CUIGroup_MonsterHP(const CUIGroup_MonsterHP& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_MonsterHP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_MonsterHP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_MonsterHP::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_MonsterHP::Tick(_float fTimeDelta)
{
	/*_bool isRender_End = false;
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
	}*/

	m_isRend = true;
}

void CUIGroup_MonsterHP::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_MonsterHP::Render()
{
	return S_OK;
}

void CUIGroup_MonsterHP::Update_Pos(_vector vMonsterPos)
{
	// Monster Pos 받아와서 적용해주기
	_vector vPos = vMonsterPos;
	vPos.m128_f32[1] += 50.f;

	vector<CUI*>::iterator hp = m_vecUI.begin();
	dynamic_cast<CUI_MonsterHP*>(*hp)->Update_Pos(vPos);
}

HRESULT CUIGroup_MonsterHP::Create_UI()
{
	CUI::UI_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;

	// HP
	m_vecUI.emplace_back(dynamic_cast<CUI_MonsterHP*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MonsterHP"), &pDesc)));

	// Bar


	return S_OK;
}

CUIGroup_MonsterHP* CUIGroup_MonsterHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_MonsterHP* pInstance = new CUIGroup_MonsterHP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_MonsterHP");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_MonsterHP::Clone(void* pArg)
{
	CUIGroup_MonsterHP* pInstance = new CUIGroup_MonsterHP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_MonsterHP");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_MonsterHP::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
