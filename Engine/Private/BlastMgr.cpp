#include "BlastMgr.h"

CBlastMgr::CBlastMgr()
{
}

HRESULT CBlastMgr::Initialize()
{
	m_pFramework = NvBlastTkFrameworkCreate();
	if (nullptr == m_pFramework)
		return E_FAIL;

	
	return S_OK;
}

CBlastMgr* CBlastMgr::Create()
{
	CBlastMgr* pInstance = new CBlastMgr();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CBlastMgr Created Failed");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CBlastMgr::Free()
{
	Safe_physX_Release(m_pFramework);
}
