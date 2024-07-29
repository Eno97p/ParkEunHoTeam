#include "UI_ShopSelect.h"

#include "GameInstance.h"

#include "UI_ItemIcon.h"

CUI_ShopSelect::CUI_ShopSelect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_ShopSelect::CUI_ShopSelect(const CUI_ShopSelect& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_ShopSelect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_ShopSelect::Initialize(void* pArg)
{
    UI_SHOPSELECT_DESC* pDesc = static_cast<UI_SHOPSELECT_DESC*>(pArg);

    m_iSlotIdx = pDesc->iSlotIdx;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    Setting_Position();

    if (FAILED(Create_UI()))
        return E_FAIL;

    return S_OK;
}

void CUI_ShopSelect::Priority_Tick(_float fTimeDelta)
{
}

void CUI_ShopSelect::Tick(_float fTimeDelta)
{
    if (!m_isRenderAnimFinished)
        Render_Animation(fTimeDelta);

    m_pItemIcon->Tick(fTimeDelta);
}

void CUI_ShopSelect::Late_Tick(_float fTimeDelta)
{
    CGameInstance::GetInstance()->Add_UI(this, SEVENTH);

    m_pItemIcon->Late_Tick(fTimeDelta);
}

HRESULT CUI_ShopSelect::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(3);
    m_pVIBufferCom->Bind_Buffers();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CUI_ShopSelect::Add_Components()
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
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ShopSelect"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_ShopSelect::Bind_ShaderResources()
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

HRESULT CUI_ShopSelect::Create_UI()
{
    // ItemIcon / RemainIcon / Soul 생성 필요
    CUI_ItemIcon::UI_ITEMICON_DESC pItemIconDesc{};
    pItemIconDesc.eLevel = LEVEL_STATIC;
    pItemIconDesc.eUISort = EIGHT;
    pItemIconDesc.fX = m_fX - 190.f;
    pItemIconDesc.fY = m_fY;
    pItemIconDesc.fSizeX = 60.f;
    pItemIconDesc.fSizeY = 60.f;

    switch (m_iSlotIdx)
    {
    case 0:
    {
        pItemIconDesc.wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Ether"); // 마나 채워주는 템
        break;
    }
    case 1:
    {
        pItemIconDesc.wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff3"); // 버프 템
        break;
    }
    case 2:
    {
        pItemIconDesc.wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Upgrade1"); // 강화
        break;
    }
    default:
        break;
    }


    // ItemIcon
    m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pItemIconDesc));

    return S_OK;
}

CUI_ShopSelect* CUI_ShopSelect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_ShopSelect* pInstance = new CUI_ShopSelect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUI_ShopSelect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_ShopSelect::Clone(void* pArg)
{
    CUI_ShopSelect* pInstance = new CUI_ShopSelect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUI_ShopSelect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_ShopSelect::Free()
{
    __super::Free();

    Safe_Release(m_pItemIcon);
}
