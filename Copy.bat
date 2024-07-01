// 명령어			옵션		원본 파일이 있는 위치					사본 파일을 저장할 위치

xcopy			/y		.\Engine\Public\*.*					.\EngineSDK\Inc\
xcopy			/y/s	.\Engine\Public\*.*					.\EngineSDK\Inc\

xcopy			/y		.\Engine\Bin\Engine.dll				.\Client\Bin\
xcopy			/y		.\Engine\Bin\Engine.dll				.\EffectTool\Bin\
xcopy			/y		.\Engine\Bin\Engine.lib				.\EngineSDK\Lib\

xcopy			/y		.\Engine\ThirdPartyLib\*.*			.\EngineSDK\Lib\




// 피직스 
xcopy			/y		.\Engine\Bin\freeglutd.dll				.\Client\Bin\
xcopy			/y		.\Engine\Bin\PhysX_64.dll				.\Client\Bin\
xcopy			/y		.\Engine\Bin\PhysXCommon_64.dll				.\Client\Bin\
xcopy			/y		.\Engine\Bin\PhysXCooking_64.dll				.\Client\Bin\
xcopy			/y		.\Engine\Bin\PhysXDevice64.dll				.\Client\Bin\
xcopy			/y		.\Engine\Bin\PhysXFoundation_64.dll				.\Client\Bin\
xcopy			/y		.\Engine\Bin\PhysXGpu_64.dll				.\Client\Bin\
xcopy			/y		.\Engine\Bin\PVDRuntime_64.dll				.\Client\Bin\

// 박은호
xcopy			/y		.\Engine\Bin\freeglutd.dll				.\EffectTool\Bin\
xcopy			/y		.\Engine\Bin\PhysX_64.dll				.\EffectTool\Bin\
xcopy			/y		.\Engine\Bin\PhysXCommon_64.dll				.\EffectTool\Bin\
xcopy			/y		.\Engine\Bin\PhysXCooking_64.dll				.\EffectTool\Bin\
xcopy			/y		.\Engine\Bin\PhysXDevice64.dll				.\EffectTool\Bin\
xcopy			/y		.\Engine\Bin\PhysXFoundation_64.dll				.\EffectTool\Bin\
xcopy			/y		.\Engine\Bin\PhysXGpu_64.dll				.\EffectTool\Bin\
xcopy			/y		.\Engine\Bin\PVDRuntime_64.dll				.\EffectTool\Bin\

// 김보민
xcopy			/y		.\Engine\Bin\Engine.dll					.\AnimTool\Bin\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl			.\AnimTool\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli		.\AnimTool\Bin\ShaderFiles\


xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\Client\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\Client\Bin\ShaderFiles\

// 박은호
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\EffectTool\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\EffectTool\Bin\ShaderFiles\


// 이민영
xcopy			/y		.\Engine\Bin\freeglutd.dll				.\MYMapTool\Bin\
xcopy			/y		.\Engine\Bin\PhysX_64.dll				.\MYMapTool\Bin\
xcopy			/y		.\Engine\Bin\PhysXCommon_64.dll				.\MYMapTool\Bin\
xcopy			/y		.\Engine\Bin\PhysXCooking_64.dll				.\MYMapTool\Bin\
xcopy			/y		.\Engine\Bin\PhysXDevice64.dll				.\MYMapTool\Bin\
xcopy			/y		.\Engine\Bin\PhysXFoundation_64.dll				.\MYMapTool\Bin\
xcopy			/y		.\Engine\Bin\PhysXGpu_64.dll				.\MYMapTool\Bin\
xcopy			/y		.\Engine\Bin\PVDRuntime_64.dll				.\MYMapTool\Bin\

xcopy			/y		.\Engine\Bin\Engine.dll				.\MYMapTool\Bin\

xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\MYMapTool\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\MYMapTool\Bin\ShaderFiles\