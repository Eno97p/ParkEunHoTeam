#include "UI_LoadingCircle.h"

#include "GameInstance.h"

CUI_LoadingCircle::CUI_LoadingCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_LoadingCircle::CUI_LoadingCircle(const CUI_LoadingCircle& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_LoadingCircle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_LoadingCircle::Initialize(void* pArg)
{
	UI_CIRCLE_DESC* pDesc = static_cast<UI_CIRCLE_DESC*>(pArg);

	m_eCircleType = pDesc->eCircleType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	m_vFontColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);

	return S_OK;
}

void CUI_LoadingCircle::Priority_Tick(_float fTimeDelta)
{
}

void CUI_LoadingCircle::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	Turn_Animation(fTimeDelta);

	Change_FontColor(fTimeDelta);

}

void CUI_LoadingCircle::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_LoadingCircle::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(5);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	Render_Font();

	return S_OK;
}

HRESULT CUI_LoadingCircle::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LoadingCircle"),  
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LoadingCircle::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_eCircleType)))
		return E_FAIL;

	_float4x4 rotationMatrix;
	XMStoreFloat4x4(&rotationMatrix, m_RotationMatrix);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_RotationMatrix", &rotationMatrix)))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;*/

	return S_OK;
}

void CUI_LoadingCircle::Turn_Animation(_float fTimeDelta)
{
	if(CIRCLE_ONE == m_eCircleType)
		m_fRotationAngle += fTimeDelta * XMConvertToRadians(5.f);
	else
		m_fRotationAngle += fTimeDelta * XMConvertToRadians(-5.f);

	if (m_fRotationAngle > XM_2PI)
		m_fRotationAngle -= XM_2PI;

	m_RotationMatrix = XMMatrixRotationZ(m_fRotationAngle); // XMMatrix
}

void CUI_LoadingCircle::Render_Font()
{
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_HeirofLight15"),
		TEXT("              자칭 GEODESIAN이라는 자에 대해 알려진 것은 거의 없다.\n"
		TEXT("                               그는 과학자라고도, 학자라고도 불린다.\n")
		TEXT("그는 GEODESIC 게이트라는 복잡한 구조물의 창조를 인정할 수 있을 것인가?\n")
		TEXT("                                   스스로를 창조자로 여겼을 것인가.")),
		_float2((g_iWinSizeX>> 1) - 340.f, (g_iWinSizeY >> 1) - 70.f), XMVectorSet(m_fFontRGB, m_fFontRGB, m_fFontRGB, 1.f))))
		return;
}

void CUI_LoadingCircle::Change_FontColor(_float fTimeDelta)
{
	m_fFontTimer += fTimeDelta;


	if (m_isFontOn)
	{
		m_fFontRGB += 0.05f;
		if (m_fFontRGB >= 1.f)
			m_fFontRGB = 1.f;
		if (3.f <= m_fFontTimer)
		{
			m_isFontOn = false;
			m_fFontTimer = 0.f;
		}
	}
	else
	{
		m_fFontRGB -= 0.05f;
		if (m_fFontRGB <= 0.f)
			m_fFontRGB = 0.f;
		if (1.f <= m_fFontTimer)
		{
			m_isFontOn = true;
			m_fFontTimer = 0.f;
		}
	}
}

CUI_LoadingCircle* CUI_LoadingCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_LoadingCircle* pInstance = new CUI_LoadingCircle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_LoadingCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_LoadingCircle::Clone(void* pArg)
{
	CUI_LoadingCircle* pInstance = new CUI_LoadingCircle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_LoadingCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LoadingCircle::Free()
{
	__super::Free();
}
