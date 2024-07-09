#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CLoader() = default;

public:
	const _tchar* Get_LoadingText() const {
		return m_szLoadingText;
	}

	_bool is_Finished() const {
		return m_isFinished;
	}

public:	
	HRESULT Initialize(LEVEL eNextLevel);
	HRESULT Loading();
	HRESULT Loading_Map();
	HRESULT Load_LevelData(const _tchar* pFilePath);
	HRESULT Loading_Shader();

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };	


#define MAX_THREAD 4
private:
	void Finish_Thread(const _uint& iIndex)
	{
		m_bIsFinish[iIndex] = true;
		if (++m_iFinishedThreadCount == USED_THREAD_COUNT)
			m_isFinished = true;
	}

private:
	/*For. Thread*/
	static const int USED_THREAD_COUNT = 3;
	atomic<bool> 		m_bIsFinish[MAX_THREAD];
	atomic<int>			m_iFinishedThreadCount;


	/* atomic<T>  */
		//���ڼ� ���� : ������ �߰��� ������ �ʰ� ������ ����˴ϴ�.
		//����ȭ ������� ���� : ��(lock)�� ������� �ʰ� ������ �� ����ȭ�� �����մϴ�.
		//������ ���� ���� : ���� �����尡 ���ÿ� ���� �����Ϳ� �����ص� �����մϴ�.
		//�޸� ���� ���� : �����Ϸ��� CPU�� ����ȭ�� ���� ���� ������ �����մϴ�.



	HANDLE					m_hThread[MAX_THREAD] = { 0 };
	CRITICAL_SECTION		m_Critical_Section[MAX_THREAD] = {};

	
private:
	LEVEL					m_eNextLevel = { LEVEL_END };
	_tchar					m_szLoadingText[MAX_PATH] = { TEXT("") };
	_bool					m_isFinished = { false };
	CGameInstance*			m_pGameInstance = { nullptr };

	

private:
	HRESULT Load_LevelData(LEVEL eLevel, const _tchar* pFilePath);

	HRESULT Loading_For_LogoLevel();
	HRESULT Loading_For_LogoLevel_For_Shader();
	
	HRESULT Loading_For_GamePlayLevel();
	HRESULT Loading_For_GamePlayLevel_For_Shader();



public:
	static CLoader* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevel);
	virtual void Free() override;



};

END