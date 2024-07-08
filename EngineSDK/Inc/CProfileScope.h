#pragma once
#include"CProfiler.h"

#define PROFILE_FUNCTION() ProfileScope profiler##__LINE__(__FUNCTION__)
#define PROFILE_SCOPE(name) ProfileScope profiler##__LINE__(name)

BEGIN(Engine)
class ProfileScope
{
public:
    ProfileScope(const char* name) : m_name(name) {
        CProfiler::GetInstance().StartProfile(m_name);
    }
    ~ProfileScope() {
        CProfiler::GetInstance().EndProfile(m_name);
    }
private:
    const char* m_name;
};

END