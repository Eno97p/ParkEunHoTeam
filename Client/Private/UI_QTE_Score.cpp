#include "UI_QTE_Score.h"

#include "GameInstance.h"

#include "UI_QTE_Particle.h"

CUI_QTE_Score::CUI_QTE_Score(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_QTE_Score::CUI_QTE_Score(const CUI_QTE_Score& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_QTE_Score::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_QTE_Score::Initialize(void* pArg)
{
    UI_SCORE_DESC* pDesc = static_cast<UI_SCORE_DESC*>(pArg);

    m_eScoreType = pDesc->eScoreType;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    Setting_Position();

    if (SCORE_PERFECT == m_eScoreType)
        Create_Particle();

    return S_OK;
}

void CUI_QTE_Score::Priority_Tick(_float fTimeDelta)
{
}

void CUI_QTE_Score::Tick(_float fTimeDelta)
{
    // ¾ê »ý¼ºµÉ ¶§ ¹º°¡ ÆÅ Æ¢´Â ¸®¼Ò½º Ãß°¡ÇØÁÖ±â

    if (nullptr != m_pParticle)
    {
        if (m_pParticle->Get_isDead())
        {
            Safe_Release(m_pParticle);
            m_pParticle = nullptr;
        }
        else
            m_pParticle->Tick(fTimeDelta);
    }
}

void CUI_QTE_Score::Late_Tick(_float fTimeDelta)
{
    CGameInstance::GetInstance()->Add_UI(this, FIRST);

    if (nullptr != m_pParticle)
        m_pParticle->Late_Tick(fTimeDelta);
}

HRESULT CUI_QTE_Score::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(8);
    m_pVIBufferCom->Bind_Buffers();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CUI_QTE_Score::Add_Components()
{
    /* For. Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    wstring wstrTextureName = TEXT("");
    switch (m_eScoreType)
    {
    case Client::CUI_QTE_Score::SCORE_PERFECT:
        wstrTextureName = TEXT("Prototype_Component_Texture_UI_QTE_Score_Perfect");
        break;
    case Client::CUI_QTE_Score::SCORE_GOOD:
        wstrTextureName = TEXT("Prototype_Component_Texture_UI_QTE_Score_Good");
        break;
    case Client::CUI_QTE_Score::SCORE_BAD:
        wstrTextureName = TEXT("Prototype_Component_Texture_UI_QTE_Score_Bad");
        break;
    default:
        break;
    }

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, wstrTextureName,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_QTE_Score::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_QTE_Score::Create_Particle()
{
    CUI::UI_DESC pDesc{};
    pDesc.eLevel = LEVEL_STATIC;
    pDesc.fX = m_fX;
    pDesc.fY = m_fY;
    pDesc.fSizeX = 512.f * 0.4f; // 512
    pDesc.fSizeY = 512.f * 0.4f;

    m_pParticle = dynamic_cast<CUI_QTE_Particle*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QTE_Particle"), &pDesc));

    return S_OK;
}

CUI_QTE_Score* CUI_QTE_Score::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_QTE_Score* pInstance = new CUI_QTE_Score(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUI_QTE_Score");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_QTE_Score::Clone(void* pArg)
{
    CUI_QTE_Score* pInstance = new CUI_QTE_Score(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUI_QTE_Score");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_QTE_Score::Free()
{
    __super::Free();

    Safe_Release(m_pParticle);
}
