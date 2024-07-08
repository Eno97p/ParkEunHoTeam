#pragma once
#include"Base.h"

BEGIN(Engine)
class CProfiler final
{
private:
	struct ProfileData {
		std::string name;
		std::chrono::high_resolution_clock::time_point startTime;
		double totalTime = 0.0;
		int callCount = 0;
	};
	


private:
	CProfiler();
	CProfiler(const CProfiler& rhs) = delete;
	CProfiler& operator=(const CProfiler&) = delete;
	~CProfiler() = default;
public:
	void StartProfile(const string& name) {
		lock_guard<mutex> lock(m_Mutex);
		auto& profile = m_profiles[name];
		profile.name = name;
		profile.startTime = chrono::high_resolution_clock::now();
		profile.callCount++;
	}

	void EndProfile(const string& name) {
		lock_guard<std::mutex> lock(m_Mutex);
		auto& profile = m_profiles[name];
		auto endTime = chrono::high_resolution_clock::now();
		profile.totalTime += chrono::duration<double, milli>(endTime - profile.startTime).count();
	}
	void Reset() {
		m_profiles.clear();
		m_globalStartTime = chrono::high_resolution_clock::now();
	}








private:
	static CProfiler* m_pInstance;

private:
	unordered_map<string, ProfileData> m_profiles;
	chrono::high_resolution_clock::time_point m_globalStartTime;
	mutex m_Mutex;
public:
	static CProfiler& GetInstance();
	static void DestroyInstance();
};

END

