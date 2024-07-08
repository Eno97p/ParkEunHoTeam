#include "UI_LogoSelector.h"

#include "GameInstance.h"
#include "CMouse.h"

CUI_LogoSelector::CUI_LogoSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Interaction{ pDevice, pContext }
{
}

CUI_LogoSelector::CUI_LogoSelector(const CUI_LogoSelector& rhs)
    : CUI_Interaction{ rhs }
{
}

HRESULT CUI_LogoSelector::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_LogoSelector::Initialize(void* pArg)
{
    UI_SELECTOR_DESC* pDesc = static_cast<UI_SELECTOR_DESC*>(pArg);

    m_eSelectorType = pDesc->eSelectorType;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    Setting_Position();

    return S_OK;
}

void CUI_LogoSelector::Priority_Tick(_float fTimeDelta)
{
}

void CUI_LogoSelector::Tick(_float fTimeDelta)
{
    if (!m_isRenderAnimFinished)
        Render_Animation(fTimeDelta);

    //__super::Tick(fTimeDelta);

    m_CollisionRect = { LONG(m_fX - m_fSizeX * 0.3f),
                    LONG(m_fY - m_fSizeY * 0.1f),
                    LONG(m_fX + m_fSizeX * 0.3f),
                    LONG(m_fY + m_fSizeY * 0.1f) };

    m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());
}

void CUI_LogoSelector::Late_Tick(_float fTimeDelta)
{
    CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_LogoSelector::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(3);
    m_pVIBufferCom->Bind_Buffers();

    if(m_isSelect)
        m_pVIBufferCom->Render();


    if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo"), Setting_SelectorText(), _float2(m_fX, m_fY), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_LogoSelector::Add_Components()
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
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Logo_Selector"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_LogoSelector::Bind_ShaderResources()
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

_tchar* CUI_LogoSelector::Setting_SelectorText()
{
    switch (m_eSelectorType)
    {
    case Client::CUI_LogoSelector::SELECTOR_CONTINUE:
        return TEXT("CONTINUE");
    case Client::CUI_LogoSelector::SELECTOR_NEWGAME:
        return TEXT("NEW GAME");
    case Client::CUI_LogoSelector::SELECTOR_SETTINGS:
        return TEXT("SETTINGS");
    case Client::CUI_LogoSelector::SELECTOR_CREDITS:
        return TEXT("CREDITS");
    case Client::CUI_LogoSelector::SELECTOR_LEAVE:
        return TEXT("LEAVE");
    default:
        return TEXT("");
    }
}

CUI_LogoSelector* CUI_LogoSelector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_LogoSelector* pInstance = new CUI_LogoSelector(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUI_LogoSelector");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_LogoSelector::Clone(void* pArg)
{
    CUI_LogoSelector* pInstance = new CUI_LogoSelector(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUI_LogoSelector");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_LogoSelector::Free()
{
    __super::Free();
}
