:: 명령어			옵션		원본 파일이 있는 위치					사본 파일을 저장할 위치

xcopy			/y		.\Engine\Public\*.*					.\EngineSDK\Inc\
xcopy			/y/s	.\Engine\Public\*.*					.\EngineSDK\Inc\
xcopy			/y		.\Engine\Bin\Engine.lib				.\EngineSDK\Lib\

xcopy			/y/s	.\Engine\ThirdPartyLib\*.*			.\EngineSDK\Lib\









::ShaderFiles
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl			.\Client\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli		.\Client\Bin\ShaderFiles\



::ShaderFiles
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\EffectTool\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\EffectTool\Bin\ShaderFiles\


::ShaderFiles
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\MYMapTool\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\MYMapTool\Bin\ShaderFiles\