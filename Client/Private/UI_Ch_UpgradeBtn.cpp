#include "UI_Ch_UpgradeBtn.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "CMouse.h"

CUI_Ch_UpgradeBtn::CUI_Ch_UpgradeBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Interaction{ pDevice, pContext }
{
}

CUI_Ch_UpgradeBtn::CUI_Ch_UpgradeBtn(const CUI_Ch_UpgradeBtn& rhs)
    : CUI_Interaction{ rhs }
{
}

HRESULT CUI_Ch_UpgradeBtn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Ch_UpgradeBtn::Initialize(void* pArg)
{
    UI_CHUP_BTN_DESC* pDesc = static_cast<UI_CHUP_BTN_DESC*>(pArg);

    m_isPlus = pDesc->isPlus;
    m_eAbilityType = pDesc->eAbilityType;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    Setting_Position();

    return S_OK;
}

void CUI_Ch_UpgradeBtn::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Ch_UpgradeBtn::Tick(_float fTimeDelta)
{
    if (!m_isRenderAnimFinished)
        Render_Animation(fTimeDelta);

    __super::Tick(fTimeDelta);

    m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());
}

void CUI_Ch_UpgradeBtn::Late_Tick(_float fTimeDelta)
{
    CGameInstance::GetInstance()->Add_UI(this, NINETH);
}

HRESULT CUI_Ch_UpgradeBtn::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(3);
    m_pVIBufferCom->Bind_Buffers();
    m_pVIBufferCom->Render();

    //Rend_Font();

    return S_OK;
}

HRESULT CUI_Ch_UpgradeBtn::Add_Components()
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
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ch_Upgrade_Btn"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Ch_UpgradeBtn::Bind_ShaderResources()
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

void CUI_Ch_UpgradeBtn::Rend_Font()
{
    if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), TEXT("ON"), _float2(m_fX - 22.f, m_fY - 15.f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
        return;
}

CUI_Ch_UpgradeBtn* CUI_Ch_UpgradeBtn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Ch_UpgradeBtn* pInstance = new CUI_Ch_UpgradeBtn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUI_Ch_UpgradeBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Ch_UpgradeBtn::Clone(void* pArg)
{
    CUI_Ch_UpgradeBtn* pInstance = new CUI_Ch_UpgradeBtn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUI_Ch_UpgradeBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Ch_UpgradeBtn::Free()
{
    __super::Free();
}
