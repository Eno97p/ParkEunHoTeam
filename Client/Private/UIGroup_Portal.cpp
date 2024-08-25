#include "UIGroup_Portal.h"

#include "GameInstance.h"

#include "UI_PortalPic.h"
#include "UI_PortalText.h"

CUIGroup_Portal::CUIGroup_Portal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Portal::CUIGroup_Portal(const CUIGroup_Portal& rhs)
	: CUIGroup{ rhs }
{
}

_bool CUIGroup_Portal::Get_isPic()
{
	if (m_pPic == nullptr)
		return false;
	else
		return true;
}

void CUIGroup_Portal::Set_FadeOut()
{
	m_pPic->Set_FadeOut();
}

HRESULT CUIGroup_Portal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Portal::Initialize(void* pArg)
{
	UIGROUP_PORTAL_DESC* pDesc = static_cast<UIGROUP_PORTAL_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI(pDesc->isPic, pDesc->iPicNum, pDesc->fAngle, pDesc->vPos, pDesc->fScale)))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_Portal::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Portal::Tick(_float fTimeDelta)
{
	if (nullptr != m_pPic)
	{
		m_pPic->Tick(fTimeDelta);

		if (m_pPic->Get_isFadeEnd())
		{
			// �������� ��
			m_isFadeEnd = true;
		}
	}

	if (nullptr != m_pText)
		m_pText->Tick(fTimeDelta);
}

void CUIGroup_Portal::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pPic)
		m_pPic->Late_Tick(fTimeDelta);

	if (nullptr != m_pText)
		m_pText->Late_Tick(fTimeDelta);
}

HRESULT CUIGroup_Portal::Render()
{
	return S_OK;
}

HRESULT CUIGroup_Portal::Create_UI(_bool isPic, _uint iPicNum, _float fAngle, _vector vPos, _float2 fScale)
{
	CUI_PortalText::UI_PORTALTEXT_DESC pTextDesc{};
	pTextDesc.eLevel = LEVEL_STATIC;
	pTextDesc.vPos = vPos;
	pTextDesc.ePortalLevel = LEVEL_GRASSLAND;

	m_pText = dynamic_cast<CUI_PortalText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_PortalText"), &pTextDesc));
	if (nullptr == m_pText)
		return E_FAIL;

	if (isPic) // Pic�� �ִ� ���
	{
		CUI_PortalPic::UI_PORTALPIC_DESC pPicDesc{};
		pPicDesc.eLevel = LEVEL_STATIC;
		pPicDesc.iPicNum = iPicNum;
		pPicDesc.fAngle = fAngle;
		pPicDesc.vPos = vPos;
		pPicDesc.fScale = fScale;

		m_pPic = dynamic_cast<CUI_PortalPic*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_PortalPic"), &pPicDesc));
		if (nullptr == m_pPic)
			return E_FAIL;
	}

	return S_OK;
}

CUIGroup_Portal* CUIGroup_Portal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_Portal* pInstance = new CUIGroup_Portal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_Portal");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_Portal::Clone(void* pArg)
{
	CUIGroup_Portal* pInstance = new CUIGroup_Portal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_Portal");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_Portal::Free()
{
	__super::Free();

	Safe_Release(m_pText);
	Safe_Release(m_pPic);
}
