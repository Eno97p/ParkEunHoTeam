:: 명령어			옵션		원본 파일이 있는 위치					사본 파일을 저장할 위치

xcopy			/y		.\Engine\Public\*.*					.\EngineSDK\Inc\
xcopy			/y/s	.\Engine\Public\*.*					.\EngineSDK\Inc\
xcopy			/y		.\Engine\Bin\Engine.lib				.\EngineSDK\Lib\

xcopy			/y/s	.\Engine\ThirdPartyLib\*.*			.\EngineSDK\Lib\






::Client------------------------------------------------------------------------------------
::Engine
xcopy			/y		.\Engine\Bin\Engine.dll					.\Client\Bin\
xcopy			/y		.\Engine\Bin\Engine.dll					.\Client\Bin\Release\

::Blast
xcopy			/y/s		.\Engine\DLL\Blast\Debug\*.*		.\Client\Bin\
xcopy			/y/s		.\Engine\DLL\Blast\Release\*.*		.\Client\Bin\Release\

::ASSIMP
xcopy			/y/s		.\Engine\DLL\Assimp\Debug\*.*		.\Client\Bin\
xcopy			/y/s		.\Engine\DLL\Assimp\Release\*.*		.\Client\Bin\Release\

::FOMOD
xcopy			/y/s		.\Engine\DLL\FMOD\*.*				.\Client\Bin\
xcopy			/y/s		.\Engine\DLL\FMOD\*.*				.\Client\Bin\Release\

::PhysX
xcopy			/y/s		.\Engine\DLL\PhysX\Debug\*.*		.\Client\Bin\
xcopy			/y/s		.\Engine\DLL\PhysX\Release\*.*		.\Client\Bin\Release\

::ShaderFiles
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl			.\Client\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli		.\Client\Bin\ShaderFiles\
::Client------------------------------------------------------------------------------------








::EffectTool------------------------------------------------------------------------------------
::Engine
xcopy			/y			.\Engine\Bin\Engine.dll				.\EffectTool\Bin\

::Blast
xcopy			/y/s		.\Engine\DLL\Blast\Release\*.*		.\EffectTool\Bin\

::ASSIMP
xcopy			/y/s		.\Engine\DLL\Assimp\Debug\*.*		.\EffectTool\Bin\
xcopy			/y/s		.\Engine\DLL\Assimp\Release\*.*		.\EffectTool\Bin\

::FOMOD
xcopy			/y/s		.\Engine\DLL\FMOD\*.*				.\EffectTool\Bin\

::PhysX
xcopy			/y/s		.\Engine\DLL\PhysX\Debug\*.*		.\EffectTool\Bin\


::ShaderFiles
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\EffectTool\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\EffectTool\Bin\ShaderFiles\
::EffectTool------------------------------------------------------------------------------------








::AnimTool------------------------------------------------------------------------------------
::Engine
xcopy			/y			.\Engine\Bin\Engine.dll				.\AnimTool\Bin\

::Blast
xcopy			/y/s		.\Engine\DLL\Blast\Release\*.*		.\AnimTool\Bin\

::ASSIMP
xcopy			/y/s		.\Engine\DLL\Assimp\Debug\*.*		.\AnimTool\Bin\
xcopy			/y/s		.\Engine\DLL\Assimp\Release\*.*		.\AnimTool\Bin\

::FOMOD
xcopy			/y/s		.\Engine\DLL\FMOD\*.*				.\AnimTool\Bin\

::PhysX
xcopy			/y/s		.\Engine\DLL\PhysX\Debug\*.*		.\AnimTool\Bin\


::ShaderFiles
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\AnimTool\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\AnimTool\Bin\ShaderFiles\
::AnimTool------------------------------------------------------------------------------------









::MYMapTool------------------------------------------------------------------------------------
::Engine_DLL
xcopy			/y			.\Engine\Bin\Engine.dll				.\MYMapTool\Bin\

::Blast_DLL
xcopy			/y/s		.\Engine\DLL\Blast\Release\*.*		.\MYMapTool\Bin\

::ASSIMP_DLL
xcopy			/y/s		.\Engine\DLL\Assimp\Debug\*.*		.\MYMapTool\Bin\
xcopy			/y/s		.\Engine\DLL\Assimp\Release\*.*		.\MYMapTool\Bin\

::FOMOD_DLL
xcopy			/y/s		.\Engine\DLL\FMOD\*.*				.\MYMapTool\Bin\

::PhysX_DLL
xcopy			/y/s		.\Engine\DLL\PhysX\Debug\*.*		.\MYMapTool\Bin\


::ShaderFiles
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\MYMapTool\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\MYMapTool\Bin\ShaderFiles\
::MYMapTool------------------------------------------------------------------------------------