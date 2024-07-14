#pragma once
#include "Base.h"


/*
https://github.com/progschj/ThreadPool
https://koreanfoodie.me/945


*/


BEGIN(Engine)
class ENGINE_DLL CRenderWorker final : public CBase
{
private:
	CRenderWorker(size_t iNumThreadPool, ID3D11Device* pDevice);
	virtual ~CRenderWorker() = default;



public:
	
	template<typename T, typename... Args>
	auto Add_Job(T&& Func) -> future<void>;



	void Shutdown();
	void ExecuteCommandLists(ID3D11DeviceContext* pImmediateContext);
	//void WorkTask();

	void AddCommandList(ID3D11CommandList* pCommandList);
	ID3D11DeviceContext* GetDeferredContext();
	

private:
	HRESULT Initialize();
	void RenderWorkerThread();


private:
	size_t m_ThreadCount = 0;
	ID3D11Device* m_pDevice = nullptr;
	vector<ID3D11DeviceContext*> m_pDeferredContexts;
	vector<ID3D11CommandList*> m_pCommandLists;
	vector<thread> m_RenderWorkers;
	queue<function<void()>> m_RenderJobs;

	mutex m_JobMutex;
	mutex m_ContextMutex;

	condition_variable m_condition;

	_bool m_bIsShutdown = false;
public:
	static CRenderWorker* Create(size_t iNumThreadPool, ID3D11Device* pDevice);
	virtual void Free() override;



};

END
