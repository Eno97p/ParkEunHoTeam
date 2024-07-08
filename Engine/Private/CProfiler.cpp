#include "CProfiler.h"



CProfiler* CProfiler::m_pInstance = nullptr;

CProfiler::CProfiler()
{
   m_globalStartTime = std::chrono::high_resolution_clock::now();
}

CProfiler& CProfiler::GetInstance()
{
    static std::once_flag flag;
    std::call_once(flag, []() {
        m_pInstance = new CProfiler();
        std::atexit([]() { delete m_pInstance; });
        });
    return *m_pInstance;


    
}

void CProfiler::DestroyInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
