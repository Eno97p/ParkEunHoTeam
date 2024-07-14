#include "CRenderWorker.h"


CRenderWorker::CRenderWorker(size_t iNumThreadPool, ID3D11Device* pDevice)
	: m_ThreadCount(iNumThreadPool)
	, m_bIsShutdown(false)
	, m_pDevice(pDevice)
{

	Safe_AddRef(m_pDevice);
}



template<typename T, typename... Args>
auto CRenderWorker::Add_Job(T&& Func)->future<void>
{
	auto job = make_shared<packaged_task<void()>>(forward<T>(Func));
	future<void> result = job->get_future();

	{
		lock_guard<mutex> lock(m_JobMutex);
		m_RenderJobs.push([this, job]() {
			ID3D11DeviceContext* pDeferredContext = this->GetDeferredContext();
			(*job)();
			ID3D11CommandList* pCommandList = nullptr;
			pDeferredContext->FinishCommandList(FALSE, &pCommandList);
			this->AddCommandList(pCommandList);
			});

	}
	m_condition.notify_one();
	return result;
}


void CRenderWorker::Shutdown()
{
	{
		std::lock_guard<std::mutex> lock(m_JobMutex);
		m_bIsShutdown = true;
	}

	m_condition.notify_all();

	for (auto& worker : m_RenderWorkers)
	{
		if (worker.joinable())
			worker.join();
	}

	for (auto& context : m_pDeferredContexts)
	{
		if (context)
			Safe_Release(context);
	}

}

void CRenderWorker::ExecuteCommandLists(ID3D11DeviceContext* pImmediateContext)
{
	lock_guard<mutex> lock(m_ContextMutex);
	
	for(auto& pCommandList : m_pCommandLists)
	{
		pImmediateContext->ExecuteCommandList(pCommandList, FALSE);
		Safe_Release(pCommandList);
	}
	m_pCommandLists.clear();


}

void CRenderWorker::AddCommandList(ID3D11CommandList* pCommandList)
{
	lock_guard<mutex> lock(m_ContextMutex);
	m_pCommandLists.push_back(pCommandList);
}

ID3D11DeviceContext* CRenderWorker::GetDeferredContext()
{
	size_t index = hash<thread::id>{}(this_thread::get_id()) % m_ThreadCount;
	return m_pDeferredContexts[index];
}

HRESULT CRenderWorker::Initialize()
{
	m_pDeferredContexts.resize(m_ThreadCount);

	for (size_t i = 0; i < m_ThreadCount; ++i)
	{
		if (FAILED(m_pDevice->CreateDeferredContext(0, &m_pDeferredContexts[i])))
			return E_FAIL;

		m_RenderWorkers.emplace_back(&CRenderWorker::RenderWorkerThread, this);
	}

	return S_OK;
}

void CRenderWorker::RenderWorkerThread()
{
	while (true)
	{
		function<void()> job;
		{
			unique_lock<mutex> lock(m_JobMutex);
			m_condition.wait(lock, [this] { return m_bIsShutdown || !m_RenderJobs.empty(); });
			if (m_bIsShutdown && m_RenderJobs.empty())
				return;
			if (!m_RenderJobs.empty())
			{
				job = move(m_RenderJobs.front());
				m_RenderJobs.pop();
			}
		}
		if (job)
		{
			job();
		}
	}
}




CRenderWorker* CRenderWorker::Create(size_t iNumThreadPool, ID3D11Device* pDevice)
{
	CRenderWorker* pInstance = new CRenderWorker(iNumThreadPool, pDevice);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CRenderWorker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderWorker::Free()
{
	Shutdown();
	Safe_Release(m_pDevice);


}
