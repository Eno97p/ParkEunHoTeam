#include "UI_Ch_UpgradeBtn.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"
#include "CMouse.h"
#include "Player.h"

#include "UI_Ch_UpgradeBtn_Select.h"

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

    Create_Select();

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

    if (m_isSelect && m_pGameInstance->Mouse_Down(DIM_LB))
    {
        // 클릭 시 + or - 여부에 따라 다르게 로직 구현
        // 소울이 모자라지 않다면
        Apply_BtnEvent();
    }

    if (m_isSelect)
        m_pSelect->Tick(fTimeDelta);
}

void CUI_Ch_UpgradeBtn::Late_Tick(_float fTimeDelta)
{
    CGameInstance::GetInstance()->Add_UI(this, NINETH);

    if (m_isSelect)
        m_pSelect->Late_Tick(fTimeDelta);
}

HRESULT CUI_Ch_UpgradeBtn::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(3);
    m_pVIBufferCom->Bind_Buffers();
    m_pVIBufferCom->Render();

    Rend_Font();

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

HRESULT CUI_Ch_UpgradeBtn::Create_Select()
{
    CUI_Ch_UpgradeBtn_Select::UI_CHUP_BTNSELECT_DESC pDesc{};
    pDesc.eLevel = LEVEL_STATIC;
    pDesc.fX = m_fX;
    pDesc.fY = m_fY;
    pDesc.fSizeX = m_fSizeX;
    pDesc.fSizeY = m_fSizeY;
    
    if (m_isPlus)
        pDesc.wstrSign = TEXT("+");
    else
        pDesc.wstrSign = TEXT("-");

    m_pSelect = dynamic_cast<CUI_Ch_UpgradeBtn_Select*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Ch_Upgrade_BtnSelect"), &pDesc));

    return S_OK;
}

void CUI_Ch_UpgradeBtn::Rend_Font()
{
    wstring wstrSign = TEXT("");

    if (m_isPlus)
    {
        wstrSign = TEXT("+");
    }
    else
    {
        wstrSign = TEXT("-");
    }

    if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo13"), wstrSign, _float2(m_fX - 7.f, m_fY - 12.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
        return;
}

void CUI_Ch_UpgradeBtn::Apply_BtnEvent()
{
    // + 누르면 추가되는 값 초록색으로 변하며 추가된 값으로 출력
    // OK 누르면 효과 적용되고 창 나가는 걸로

    list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());

    _uint iSoul = CInventory::GetInstance()->Get_Soul();
    _int iLevelCost = (pPlayer->Get_Level() * 10) + 290;
    _int iValue = 1;

    if (m_isPlus && (iSoul < iLevelCost)) // Soul 부족하면
        return;

    if (m_isPlus)
        iValue = 1;
    else
    {
        iValue = -1;

        if (0 == pPlayer->Get_Level())
            return;
    }

    switch (m_eAbilityType)
    {
    case Client::CUI_Ch_UpgradeBtn::ABILITY_VITALITY:
    {
        if (!m_isPlus && (0 == pPlayer->Get_VitalityLv())) // Level이나 State이 0이면
            return;
        pPlayer->Set_VitalityLv(iValue);

        // Player가 가지고 있는 해당 State 값에 + or - 를 해야 함
        // 일단 ESC를 누르면 화면을 빠져나가고 여태 계산했던 것들을 리셋해주어야 함
        // 값을 바로 적용시키는 게 아니라 OK를 누른 경우에 적용되도록? OK 누르면 바로 창 꺼지면서 해당 값들 저장되도록.
        // 값에 변화가 있으면 초록색으로 렌더해야 하는데... > 기존의 값들을 저장하고 있어야 하나? 
        // 일단 특정 State의 + 버튼이 눌리면 해당 Font의 Color를 녹색으로 변경해서 출력
        // 원래 값을 저장하고 있다가 비교 하는 식을 ㅗ행 ㅑ할듯!




        break;
    }
    case Client::CUI_Ch_UpgradeBtn::ABILITY_STAMINA:
    {
        if (!m_isPlus && (0 == pPlayer->Get_StaminaLv())) // Level이나 State이 0이면
            return;
        pPlayer->Set_StaminaLv(iValue);
        break;
    }
    case Client::CUI_Ch_UpgradeBtn::ABILITY_STRENGHT:
    {
        if (!m_isPlus && (0 == pPlayer->Get_StrenghtLv())) // Level이나 State이 0이면
            return;
        pPlayer->Set_StrengthLv(iValue);
        break;
    }
    case Client::CUI_Ch_UpgradeBtn::ABILITY_MYSTICISM:
    {
        if (!m_isPlus && (0 == pPlayer->Get_MysticismLv())) // Level이나 State이 0이면
            return;
        pPlayer->Set_MysticismLv(iValue);
        break;
    }
    case Client::CUI_Ch_UpgradeBtn::ABILITY_KNOWLEDGE:
    {
        if (!m_isPlus && (0 == pPlayer->Get_KnowledgeLv())) // Level이나 State이 0이면
            return;
        pPlayer->Set_KnowledgeLv(iValue);
        break;
    }
    default:
        break;
    }

    if (!m_isPlus)
        CInventory::GetInstance()->Calcul_Soul(iLevelCost - 10);
    else
        CInventory::GetInstance()->Calcul_Soul(-iLevelCost);

    pPlayer->Set_Level(iValue);
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

    Safe_Release(m_pSelect);
}
