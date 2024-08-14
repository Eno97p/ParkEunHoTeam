#pragma once
#include "Client_Defines.h"



BEGIN(Client)

template<typename... T>
class CInitLoader 
{

public:
	CInitLoader(CInitLoader** ppSelf)
	{
		*ppSelf = this;
		s_m_vecInitLoader.push_back(this);
		if (s_m_vecInitLoader.size() == 1)		//첫 번째 생성할 때만 등록 
		{
			std::atexit(&CInitLoader::CheckDestroy);
		}
	}

	virtual ~CInitLoader() = default;
	//{
	//	auto iter = find(s_m_vecInitLoader.begin(), s_m_vecInitLoader.end(), this);
	//	if(iter != s_m_vecInitLoader.end())
	//	{
	//		s_m_vecInitLoader.erase(iter);
	//	}
	//}


private:
	static void CheckDestroy()
	{
		for(auto& pLoader : s_m_vecInitLoader)
		{
			if (pLoader != nullptr)
			{
				delete pLoader;
				pLoader = nullptr;
			}
			
		}
		s_m_vecInitLoader.clear();
	}

public:
	virtual void Save_Start(T... Args);
	virtual void Load_Start(T... Args);



	
private:

	static vector<CInitLoader<T ...>*> s_m_vecInitLoader;
private:

	
	vector<_tagMonsterInit_Property> m_pvecMonsterInitProperty;
};

END


template<typename... T>
vector<CInitLoader<T...>*> CInitLoader<T...>::s_m_vecInitLoader;



template<typename ...T>
inline void CInitLoader<T...>::Save_Start(T ...Args)
{
	auto pGameInstance = CGameInstance::GetInstance();
	auto eLevel = get<0>(tuple<T...>(Args ...));
	auto pLayer = get<1>(tuple<T...>(Args ...));
	
	auto future = pGameInstance->AddWork([this, pGameInstance, eLevel, pLayer]() {

		list<CGameObject*> pList = pGameInstance->Get_GameObjects_Ref(eLevel, pLayer);
		_uint iLayerSize = pList.size();
		m_pvecMonsterInitProperty.clear();
		m_pvecMonsterInitProperty.resize(iLayerSize);
		_uint i = 0;
		for(auto& pObject : pList)
		{
			m_pvecMonsterInitProperty[i].vPos = dynamic_cast<CMonster*>(pObject)->Get_InitPos();
			m_pvecMonsterInitProperty[i].strMonsterTag = pObject->Get_ProtoTypeTag();
			++i;
		}
		wstring wstrLevelName = Client::Get_CurLevelName(eLevel);
		wstring wstrFilePath = L"../Bin/DataFiles/LevelInit_" + wstrLevelName + L"_" + pLayer + L".dat";
		Engine::Save_Data(wstrFilePath, false, m_pvecMonsterInitProperty.size(), m_pvecMonsterInitProperty.data());

	});

}

template<typename ...T>
inline void CInitLoader<T...>::Load_Start(T ...Args)
{
	auto pGameInstance = CGameInstance::GetInstance();
	auto eLevel = get<0>(tuple<T...>(Args ...));
	auto pLayer = get<1>(tuple<T...>(Args ...));


	wstring wstrLevelName = Client::Get_CurLevelName(eLevel);
	wstring wstrFilePath = L"../Bin/DataFiles/LevelInit_" + wstrLevelName + L"_" + pLayer + L".dat";

	decltype(auto) pLoadData = Engine::Load_Data<size_t , _tagMonsterInit_Property*>(wstrFilePath);
	if (pLoadData)
	{

		size_t iVecSize = get<0>(*pLoadData);
		_tagMonsterInit_Property* pProperty = get<1>(*pLoadData);
		vector<_tagMonsterInit_Property> vecProperty(pProperty, pProperty + iVecSize);
		for(_uint i = 0 ; i< iVecSize; ++i)
		{
			CLandObject::LANDOBJ_DESC tDesc;
			tDesc.mWorldMatrix._41 = vecProperty[i].vPos.x;
			tDesc.mWorldMatrix._42 = vecProperty[i].vPos.y;
			tDesc.mWorldMatrix._43 = vecProperty[i].vPos.z;
			tDesc.mWorldMatrix._44 = 1.f;

			//pGameInstance->Add_CloneObject(eLevel, pLayer, vecProperty[i].strMonsterTag, &tDesc);
			pGameInstance->CreateObject(eLevel,pLayer.c_str(), vecProperty[i].strMonsterTag, &tDesc);
			
		}



	}

	int test = 0;


}
