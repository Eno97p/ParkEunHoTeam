#pragma once
#include "Base.h"

BEGIN(Engine)

class CSoundMgr final : public CBase
{
private:
    CSoundMgr();
    virtual ~CSoundMgr() = default;
public:
    HRESULT Initialize();
public:
    void PlaySound_Z(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
    void PlayBGM(const TCHAR* pSoundKey, float fVolume);
    void StopAll();
    void StopSound(CHANNELID eID);
    void SetChannelVolume(CHANNELID eID, float fVolume);
private:
    void LoadSoundFile();
private:
    // 사운드 리소스 정보를 갖는 객체 
    map<TCHAR*, FMOD_SOUND*> m_mapSound;
    FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
    FMOD_SYSTEM* m_pSystem;

public:
    static CSoundMgr* Create();
    virtual void Free() override;
};

END
