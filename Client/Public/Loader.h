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
	HRESULT Loading_Shader();

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };	


#define MAX_THREAD 4

	HANDLE					m_hThread[MAX_THREAD] = {0};
	CRITICAL_SECTION		m_Critical_Section[MAX_THREAD] = {};
	
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