#include "SoundMgr.h"

CSoundMgr::CSoundMgr()
{
}

HRESULT CSoundMgr::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	if (FMOD_System_Create(&m_pSystem, FMOD_VERSION) != FMOD_OK)
	{
		MSG_BOX("Failed Sound Initialize");
		return E_FAIL;
	}
	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	if (FMOD_System_Init(m_pSystem, 128, FMOD_INIT_NORMAL, NULL) != FMOD_OK)
	{
		MSG_BOX("Failed Sound Initialize");
		return E_FAIL;
	}
	if (Load_SoundFile() != FMOD_OK)
	{
		MSG_BOX("Failed Sound Initialize");
		return E_FAIL;
	}

	//LoadSoundFile();
	return S_OK;
}

void CSoundMgr::PlaySound_Z(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	//if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	//{
	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
	//}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);

	FMOD_Channel_SetPriority(m_pChannelArr[SOUND_BGM], 255);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlaySubBGM(const TCHAR* pSoundKey, float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[SOUND_SUBBGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_SUBBGM], FMOD_LOOP_NORMAL);

	FMOD_Channel_SetPriority(m_pChannelArr[SOUND_SUBBGM], 254);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_SUBBGM], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::LoadSoundFile()
{
	_wfinddata64_t fd;
	__int64 handle = _wfindfirst64(L"../../Client/Bin/Resources/Sound/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

	int iResult = 0;

	char szCurPath[128] = "../../Client/Bin/Resources/Sound/";
	char szFullPath[128] = "";
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, szFilename);
		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_3D | 0x00000020, 0, &pSound);
		if (eRes == FMOD_OK)
		{
			int iLength = strlen(szFilename) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
			MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		iResult = _wfindnext64(handle, &fd);
	}
	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}

FMOD_RESULT CSoundMgr::Load_SoundFile()
{
	FMOD_RESULT result = FMOD_OK;
	stack<wstring> directories;
	directories.push(L"../../Client/Bin/Resources/Sound/");
	while (!directories.empty() && result == FMOD_OK)
	{
		wstring currentPath = directories.top();
		directories.pop();

		wstring searchPath = currentPath + L"*.*";
		_wfinddata64_t fd;
		__int64 handle = _wfindfirst64(searchPath.c_str(), &fd);

		if (handle == -1 || handle == 0)
			continue;

		do
		{
			if (wcscmp(fd.name, L".") != 0 && wcscmp(fd.name, L"..") != 0)
			{
				if (fd.attrib & _A_SUBDIR)
				{
					// 하위 디렉토리를 스택에 추가
					directories.push(currentPath + fd.name + L"\\");
				}
				else
				{
					// 파일 처리
					wstring fullPath = currentPath + fd.name;
					char szFullPath[MAX_PATH];
					WideCharToMultiByte(CP_UTF8, 0, fullPath.c_str(), -1, szFullPath, sizeof(szFullPath), NULL, NULL);

					FMOD_SOUND* pSound = nullptr;
					result = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_3D | 0x00000020, 0, &pSound);

					if (result == FMOD_OK)
					{
						int iLength = wcslen(fd.name) + 1;
						TCHAR* pSoundKey = new TCHAR[iLength];
						wcscpy_s(pSoundKey, iLength, fd.name);

						m_mapSound.emplace(pSoundKey, pSound);
					}
					else
					{
						MSG_BOX("Failed_Load_Sound");
						break;
					}
				}
			}
		} while (_wfindnext64(handle, &fd) == 0);

		_findclose(handle);
	}

	if (result == FMOD_OK)
	{
		result = FMOD_System_Update(m_pSystem);
	}

	return result;
}

CSoundMgr* CSoundMgr::Create()
{
	CSoundMgr* pInstance = new CSoundMgr();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : SoundManager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSoundMgr::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}
