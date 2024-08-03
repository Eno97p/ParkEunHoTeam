#include "UI_MonsterHP.h"

#include "GameInstance.h"
#include "Monster.h"

CUI_MonsterHP::CUI_MonsterHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_MonsterHP::CUI_MonsterHP(const CUI_MonsterHP& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_MonsterHP::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_MonsterHP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// 위치는 몬스터 따라 가야 함
	/*m_fX = 390.f;
	m_fY = 45.f;
	m_fSizeX = 768.f;
	m_fSizeY = 24.f;*/

	//Setting_Position();

	return S_OK;
}

void CUI_MonsterHP::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MonsterHP::Tick(_float fTimeDelta)
{
	if (m_fCurrentRatio < m_fPastRatio)
	{
		m_fPastRatio -= fTimeDelta * 0.2f;
		if (m_fCurrentRatio > m_fPastRatio)
		{
			m_fPastRatio = m_fCurrentRatio;
		}
	}

	if (m_isDamageRend)
	{
		m_fDamageTimer += fTimeDelta;
		if (2. <= m_fDamageTimer)
			m_isDamageRend = false;
	}
}

void CUI_MonsterHP::Late_Tick(_float fTimeDelta)
{
	m_pTransformCom->BillBoard();

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_MonsterHP::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue(("g_CurrentRatio"), &m_fCurrentRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue(("g_PastRatio"), &m_fPastRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue(("g_HudRatio"), &m_fHudRatio, sizeof(_float))))
		return E_FAIL;

	m_pShaderCom->Begin(2); // 비율을 위해 2로 하니 출력 크기가 안 맞는 이슈 있음
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (m_isDamageRend)
	{
		// Monster HP Damage는 월드상에 있는 UI이므로? 
		// 월드의 위치를 투영 공간으로 옮겼을 때 위치가 어디인지 받아와서 적용해주면 될 거 같은데
		// Transform의 m_Matrix[MAT_PROJ] 를 활용해야 하는 것인지 > 받아오는 함수가 없는데

		const _float4x4* pProj = m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ); // 얘 사용 X
		// 저 matrix들로 뷰, 투영 행렬을 곱해서 투영 공간까지 올려주도록 하기?

		//if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo25"), m_wstrDamage, _float2(pProj->m[2][0], pProj->m[2][1]), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		//	return E_FAIL;

		//if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo23"), m_wstrDamage, _float2(m_fX + 50.f, m_fY), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		//	return E_FAIL;
	}

	return S_OK;
}

void CUI_MonsterHP::Update_Pos(_vector vMonsterPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMonsterPos);
}

void CUI_MonsterHP::Rend_Damage(_int iValue)
{
	m_iAccumDamage += iValue;
	m_isDamageRend = true;
	m_fDamageTimer = 0.f;
	m_wstrDamage = to_wstring(m_iAccumDamage);
}

HRESULT CUI_MonsterHP::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MonsterHP"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MonsterHP::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CUI_MonsterHP* CUI_MonsterHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MonsterHP* pInstance = new CUI_MonsterHP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_MonsterHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MonsterHP::Clone(void* pArg)
{
	CUI_MonsterHP* pInstance = new CUI_MonsterHP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_MonsterHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MonsterHP::Free()
{
	__super::Free();

	Safe_Release(m_pMonster);
}
