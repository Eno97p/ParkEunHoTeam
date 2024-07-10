
#include "..\Default\framework.h"
#include "../Public/Imgui_Manager.h"
#include "GameInstance.h"
#include "..\Public\imgui_impl_win32.h"
#include "..\Public\imgui_impl_dx11.h"
#include <Windows.h>
#include <string.h>
#include "..\ImGuizmo\ImGuizmo.h"
#include "..\ImGuizmo\ImSequencer.h"

#include "ToolObj_Manager.h"
#include "PipeLine.h"
#include "NaviMgr.h"

#include "Light.h"
#include "Terrain.h"
IMPLEMENT_SINGLETON(CImgui_Manager)

static ImGuizmo::OPERATION mGizmoOperation(ImGuizmo::TRANSLATE);

CImgui_Manager::CImgui_Manager() : m_bShowWindow(false), m_bShowAnotherWindow(false)
{
}


HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{


    m_pDevice = pDevice;
    m_pContext = pContext;
    m_pGameInstance = CGameInstance::GetInstance();

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
    // Win32 초기화 호출
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    m_iStageIdx = 0;
    m_iObjIdx = 0;

    return S_OK;
}

void CImgui_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static XMFLOAT4 objColor;

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered())
    {
        g_bIsHovered = true;

    }
    else
    {
        g_bIsHovered = false;
    }

    //매 업데이트마다 돌아감
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    if (m_bShowWindow) ImGui::ShowDemoWindow(&m_bShowWindow);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float fTerrainX = 0.f;
        static float fTerrainZ = 0.f;
        static int counter = 0;

        ImGui::Begin("~Shattered~ MapTool");

        /*    ImGui::Spacing();
            ImGui::Separator();*/

        ImGui::TextColored({ 0.5f, 1.f, 1.f, 1.f }, "%.1f FPS", 1000.0f / io.Framerate, io.Framerate);
        _float4 pMousePos = { 0.f, 0.f, 0.f, 1.f };
        m_pGameInstance->Get_PickPos(&pMousePos);
        ImGui::Spacing();
        ImGui::TextColored({ 1.0f, 0.84f, 0.32f, 1.f }, "Mouse Pos");
        ImGui::Text("x: %.1f, y: %.1f, z: %.1f, w: %.1f", pMousePos.x, pMousePos.y, pMousePos.z, pMousePos.w);

        ImGui::Spacing();
        ImGui::TextColored({ 1.0f, 0.84f, 0.32f, 1.f }, "Pick Pos (RB to Update)");

        if (m_pGameInstance->Mouse_Down(DIM_RB))
        {
            m_ClickedMousePos = pMousePos;
        }

        ImGui::Text("x: %.1f, y: %.1f, z: %.1f, w: %.1f", m_ClickedMousePos.x, m_ClickedMousePos.y, m_ClickedMousePos.z, m_ClickedMousePos.w);


        ImGui::Separator();

        // Stage 선택(List Box)
        ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "Scene");
        const char* Stages[] = { "Ackbar", "Tutorial", "AndrasArena", "RasSamrahCastle", "Boss Stage" };
        static int Stage_current = 0;
        ImGui::ListBox("###Stage", &Stage_current, Stages, IM_ARRAYSIZE(Stages)); // item_current 변수에 선택 값 저장
        m_iStageIdx = Stage_current;

        if (ImGui::Button("Save Objects"))
            m_IsSave = true;
        ImGui::SameLine();
        if (ImGui::Button("Load Obejects"))
            m_IsLoad = true;
        ImGui::SameLine();
        ImGui::Separator();

        if (ImGui::Button("Save PhysX"))
            m_IsPhysXSave = true;

        if (ImGui::Button("Load PhysX"))
            m_IsPhysXLoad = true;

        ImGui::SameLine();

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "Add Objects");

        ImGui::Checkbox("ADD OBJECT", &m_bAddToolObj); // Navigation 모드 활성화 여부



        if (m_bAddToolObj && !ImGui::IsAnyItemActive() && !g_bIsHovered)
        {
            if (m_pGameInstance->Mouse_Down(DIM_LB))
            {
                //_vector vPickingPos = Check_Picking();
                _bool	isPick = { false };
                _float4 pPickPos = { 0.f, 0.f, 0.f, 1.f };
                if (m_pGameInstance->Get_PickPos(&pPickPos))
                {
                    Add_ToolObj(XMLoadFloat4(&pPickPos), XMLoadFloat4(&objColor)); // Obj 생성
                }


            }
        }

        // ImGui::Checkbox("Navigation Mode On", &m_IsNaviMode); // Navigation 모드 활성화 여부
        ImGui::SameLine();
        /*      if (ImGui::Button("Navigation Clear"))
                  m_IsNaviClear = true;*/

        ImGui::Spacing();


        // Layer 선택(Combo)
        ImGui::Checkbox("ANIM MODEL", &m_bIsAnimModel);
        ImGui::Checkbox("DECO OBJECT", &m_bIsDecoObject);

        const char* Layers[] = { "Monster", "Passive Element", "Active Element", "Trigger"};
        static int Layer_current = 0;
        ImGui::Combo("Layer", &Layer_current, Layers, IM_ARRAYSIZE(Layers));
        m_iLayerIdx = Layer_current;



        Setting_ObjListBox(m_iLayerIdx); // ListBox 구성

        ImGui::Separator();

        ImGui::Spacing();

        ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "Editor");
        ImGui::Checkbox("Object Edit", &m_bShowAnotherWindow);
        ImGui::Spacing();
        ImGui::Checkbox("Light Edit", &m_bLightWindow);
        ImGui::Spacing();
        ImGui::Checkbox("Shadow Edit", &m_bShadowWindow);
        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Checkbox("Camera Edit", &m_bCameraWindow);
        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Checkbox("Terrain Edit", &m_bTerrainWindow);
        ImGui::Spacing();

        // ImGui::Checkbox("Demo Window", &m_bShowWindow);


         // Terrain 정점 개수 조절
   /*      ImGui::Text("Setting Terrain Scale");
         static bool inputs_step = true;
         const short s16_one = 1;
         ImGui::InputScalar("Terrain X", ImGuiDataType_S16, &m_iTerrainX, inputs_step ? &s16_one : NULL, NULL, "%d");
         ImGui::InputScalar("Terrain Z", ImGuiDataType_S16, &m_iTerrainZ, inputs_step ? &s16_one : NULL, NULL, "%d");

         if (ImGui::Button("Terrain ReLoad"))
             m_IsTerrainReLoad = true;*/

        ImGui::End();
    }

    if (m_bShowAnotherWindow)
    {
        m_bLightWindow = false;
        ImGui::Begin("Object Edit", &m_bShowAnotherWindow);
        //ImGui::SetWindowSize("Object State", ImVec2(350, 300));

        ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "Object Transform");

        Setting_CreateObj_ListBox();

        const _float4x4* worldMatrix = CToolObj_Manager::GetInstance()->Get_Obj_WorldMatrix(m_iCreateObjIdx);

        if (nullptr != worldMatrix)
        {
            EditTransform((_float*)CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW), // cameraView
                (_float*)CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ),
                (_float*)worldMatrix,
                true);
        }
        ImGui::ColorEdit4("Obj Diffuse", &objColor.x);

        ImGui::Spacing();
        if (ImGui::Button("Delete Obj")) // 선택 객체 삭제
            Delete_Obj();
        ImGui::SameLine();
        if (ImGui::Button("Close"))
            m_bShowAnotherWindow = false;
        ImGui::End();
    }






    if (m_bLightWindow)
    {
        m_bShowAnotherWindow = false;
        Light_Editor();
    }

    if (m_bShadowWindow)
    {
        Shadow_Editor();
    }

    if (m_bCameraWindow)
    {
        Camera_Editor();
    }

    if (m_bTerrainWindow)
    {
        Terrain_Editor();
    }

    ImGui::EndFrame();

}

void CImgui_Manager::Late_Tick(_float fTimeDelta)
{
}

void CImgui_Manager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
{
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    // editTransformDecomposition
    if (true)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_T))
            mGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            mGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
            mGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", mGizmoOperation == ImGuizmo::TRANSLATE))
            mGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mGizmoOperation == ImGuizmo::ROTATE))
            mGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mGizmoOperation == ImGuizmo::SCALE))
            mGizmoOperation = ImGuizmo::SCALE;
        /*if (ImGui::RadioButton("Universal", mGizmoOperation == ImGuizmo::UNIVERSAL))
            mGizmoOperation = ImGuizmo::UNIVERSAL;*/
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

        if (mGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_S))
            useSnap = !useSnap;
        ImGui::Checkbox("##UseSnap", &useSnap);
        ImGui::SameLine();

        switch (mGizmoOperation)
        {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap[0]);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap[0]);
            break;
        }
        ImGui::Checkbox("Bound Sizing", &boundSizing);
        if (boundSizing)
        {
            ImGui::PushID(3);
            ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
            ImGui::SameLine();
            ImGui::InputFloat3("Snap", boundsSnap);
            ImGui::PopID();
        }
    }

    ImGuiIO& io = ImGui::GetIO();
    float viewManipulateRight = io.DisplaySize.x;
    float viewManipulateTop = 0;
    static ImGuiWindowFlags gizmoWindowFlags = 0;
    if (useWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
        ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
        ImGuizmo::SetDrawlist();
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
        viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
        viewManipulateTop = ImGui::GetWindowPos().y;
        //ImGuiWindow* window = ImGui::GetCurrentWindow();
        //gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;
    }
    else
    {
        // 이거만 신경쓰기 얼마만큼 기즈모 화면값
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    }
    //격자
    //ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
    // 큐브 그려주는애
    //ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], gizmoCount);
    // 객체 잡고 동그라미좌표계 그려주는애
    // 객체 잡고 동그라미좌표계 그려주는애
    // matrix : 객체월드행ㄹ려
    ImGuizmo::Manipulate(cameraView, cameraProjection, mGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

    // Gizmo 조작 후 업데이트된 행렬 표시
    ImGui::Text("Updated World Matrix:");
    for (int i = 0; i < 4; i++)
    {
        ImGui::Text("%.3f %.3f %.3f %.3f",
            matrix[i * 4 + 0], matrix[i * 4 + 1],
            matrix[i * 4 + 2], matrix[i * 4 + 3]);
    }

    // 화면 상단 카메라 돌아가는애
    //ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

    if (useWindow)
    {
        ImGui::End();
        // 이거해보기 터지면
        ImGui::PopStyleColor(1);
    }
}

void CImgui_Manager::Setting_ObjListBox(_int iLayerIdx)
{
    // 배치할 Obj 선택(List Box)
    //ImGui::Text("Select Obj");

    static int item_current = 0;

    switch (iLayerIdx)
    {
    case LAYER_MONSTER:
    {
        const char* items_Monster[] = { "Fiona", "Lantern", "Octopus", "Crab", "EliteMst", "Boss" };
        ImGui::ListBox("###Obj", &item_current, items_Monster, IM_ARRAYSIZE(items_Monster)); // item_current 변수에 선택 값 저장
        break;
    }
    case LAYER_PASSIVE_ELEMENT:
    {
        const char* items_MapElement[] = { "BasicCube", "BasicDonut",  "BasicGround", "TronesT03","AndrasTEST","AndrasArenaRocks","Grasses_TT",
            "RasSamrahCastle",
            "RasSamrahCastle2",
            "RasSamrahCastle3",
            
            //TUTORIAL
            "TutorialMap", "TutorialDecoStructure", "TutorialDecoCube", "TutorialDecoMaze",

            //STAGE 1
            "WellArea", "MergedHouses23", "MergedHouses65", "AckbarCastle",
            "AqueducTower", "BigStairSides", "WoodStair", "_WoodPlatform", "WoodPlank", "RichStairs_Rambarde", "BigRocks", "BigRocks4",
            "Rock1",    "Rock2",    "Rock3",    "Rock4",


            //STAGE 1 DECOS
            "BoxA", "BoxB",
            "YantraStatue",
            "BannerPole1", "BannerPole2", "BannerPole3", "BannerPole4","BannerPole5",

            "AckbarWell", "Remparts_CrenauxA", "Remparts_CrenauxB", "Remparts_HourdA","Remparts_WallMotifA",
            "Remparts_WallRenfortA", "RichStairs_Rambarde",

            "Light_Brasero", "Light_BraseroSmall",
            "Light_Candle", "Light_CandleGroup",
            "Light_Crystal",
            "Light_TorchA", "Light_TorchB",
            "CharetteNew", "CharetteBroke",
            "Facade1", "Facade2",
            "Facade3", "Facade4",
            "Facade5", "Facade6",
            "Facade7", "Facade8",
            "Facade9", "Facade10",

            "RuinsPillar", "AckbarHouseLarge",
            "AckbarHouseSmall",  "AckbarHouseSwamp",
            "AckbarHouseRoofLarge",
            "AckbarHouseRoofMid", "AckbarHouseRoofSmall",
            "AqueducTowerRoof",
            "UnderRoofFilling"


        };
        ImGui::ListBox("###Obj", &item_current, items_MapElement, IM_ARRAYSIZE(items_MapElement)); // item_current 변수에 선택 값 저장
        break;
    }
    case LAYER_ACTIVE_ELEMENT:
    {
        const char* items_MapObj[] = { "Grass", "TutorialMap Bridge", "Well", "FakeWall_Donut", "FakeWall_Box",
                                        "EventTrigger_Box", "EventTrigger_Sphere",
                                        "Boss_Gun", "Cannon" };
        ImGui::ListBox("###Obj", &item_current, items_MapObj, IM_ARRAYSIZE(items_MapObj)); // item_current 변수에 선택 값 저장
        break;
    }
    case LAYER_TRIGGER:
    {
        const char* items_Trigger[] = { "Tutorial_BOSS_ENCOUNTER", "STAGE1_LightsOn" };
        ImGui::ListBox("###Obj", &item_current, items_Trigger, IM_ARRAYSIZE(items_Trigger)); // item_current 변수에 선택 값 저장
        break;
    }
        break;
    default:
        break;
    }

    m_iObjIdx = item_current;
}

void CImgui_Manager::Add_ToolObj(_vector vPosition, _vector vColor)
{
    /*CMonster::MONSTER_DESC pDesc = {};

    pDesc.vPosition = vPosition;*/

    // Calculator의 Picking_OnTerrain( ) 호출 후 반환 값이 -1, -1, -1이 아니면 해당 위치에 모델 생성
    /*if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"), &pDesc)))
        return E_FAIL;*/

    if (FAILED(CToolObj_Manager::GetInstance()->Add_CloneObj(m_iLayerIdx, m_iObjIdx, vPosition, vColor)))
    {
        MSG_BOX("ToolObj 생성 실패");
        return;

    }

    return;
}

void CImgui_Manager::Light_Editor()
{
    ImGui::Begin("Light Editing", &m_bLightWindow);
    ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "Input LightDesc : ");
    static LIGHT_DESC desc{};
    static int lightIdx = 0;
    static float diffuseCol[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static float ambientCol[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static float specularCol[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    static XMFLOAT4 direction{ 0.f, 0.f, 0.f, 1.f };
    static XMFLOAT4 position{ 0.f, 0.f, 0.f, 1.f };
    static float range = 15.0f;
    static float innerAngle = 30.0f;
    static float outerAngle = 80.0f;

    // 빛의 종류 선택
    const char* lightTypes[] = { "Directional", "Point", "Spotlight" };
    static int selectedLightType = 0;

    // LightIdx 선택
    if (ImGui::InputInt("Light Index", &lightIdx))
    {
        LIGHT_DESC newDesc{};
        newDesc = *m_pGameInstance->Get_LightDesc(lightIdx);

        // newDesc의 변수들로 static 변수 초기화
        selectedLightType = static_cast<int>(newDesc.eType);
        direction = newDesc.vDirection;
        position = newDesc.vPosition;
        range = newDesc.fRange;
        innerAngle = newDesc.innerAngle;
        outerAngle = newDesc.outerAngle;
        memcpy(diffuseCol, &newDesc.vDiffuse, sizeof(float) * 4);
        memcpy(ambientCol, &newDesc.vAmbient, sizeof(float) * 4);
        memcpy(specularCol, &newDesc.vSpecular, sizeof(float) * 4);
    }


    ImGui::Combo("Light Type", &selectedLightType, lightTypes, IM_ARRAYSIZE(lightTypes));

    ImGui::Spacing();

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Light On / Off");

    if (ImGui::Button("Light On"))
        m_pGameInstance->LightOn(lightIdx);
    ImGui::SameLine();

    // ImGui::Spacing();
    if (ImGui::Button("Light Off"))
        m_pGameInstance->LightOff(lightIdx);

    // Gizmo 세팅
    float cameraView[16];
    float cameraProjection[16];
    memcpy(cameraView, CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW), sizeof(float) * 16);
    memcpy(cameraProjection, CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ), sizeof(float) * 16);

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImGuizmo::SetRect(0, 0, displaySize.x, displaySize.y);

    // 빛의 위치와 방향을 나타내는 행렬 생성
    XMFLOAT4X4 lightMatrix;
    XMStoreFloat4x4(&lightMatrix, XMMatrixIdentity());

    switch (selectedLightType)
    {
    case 0: // Directional
    {
        ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Direction");
        ImGui::SliderFloat("X##DirectionX", &direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat("Y##DirectionY", &direction.y, -1.0f, 1.0f);
        ImGui::SliderFloat("Z##DirectionZ", &direction.z, -1.0f, 1.0f);

        // 방향 벡터 정규화
        XMVECTOR directionVector = XMVector3Normalize(XMLoadFloat4(&direction));
        XMStoreFloat4(&direction, directionVector);
    }
    break;
    case 1: // Point
    {
        XMStoreFloat4x4(&lightMatrix, XMMatrixTranslation(position.x, position.y, position.z));

        if (ImGuizmo::Manipulate(cameraView, cameraProjection, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&lightMatrix, NULL, NULL))
        {
            // 조작된 행렬에서 위치 추출
            XMStoreFloat4(&position, XMLoadFloat4((XMFLOAT4*)&lightMatrix._41));
        }

        ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Position");
        ImGui::InputFloat3("##PositionInput", (float*)&position, "%.2f");
        ImGui::InputFloat("Range", &range);
    }
    break;
    case 2: // Spotlight
    {
        XMMATRIX translationMatrix = XMMatrixTranslationFromVector(XMLoadFloat4(&position));
        XMStoreFloat4x4(&lightMatrix, translationMatrix);

        if (ImGuizmo::Manipulate(cameraView, cameraProjection, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&lightMatrix, NULL, NULL))
        {
            // 조작된 행렬에서 위치 추출
            XMStoreFloat4(&position, XMLoadFloat4((XMFLOAT4*)&lightMatrix._41));
        }

        ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Position");
        ImGui::InputFloat3("##PositionInput", (float*)&position, "%.2f");
        ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Direction");
        ImGui::SliderFloat("X##DirectionX", &direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat("Y##DirectionY", &direction.y, -1.0f, 1.0f);
        ImGui::SliderFloat("Z##DirectionZ", &direction.z, -1.0f, 1.0f);

        // 방향 벡터 정규화
        XMVECTOR directionVector = XMVector3Normalize(XMLoadFloat4(&direction));
        XMStoreFloat4(&direction, directionVector);

        ImGui::InputFloat("Range", &range);
        ImGui::InputFloat("Inner Angle", &innerAngle);
        ImGui::InputFloat("Outer Angle", &outerAngle);
    }
    break;
    }

    // 빛 색 조정
    ImGui::ColorEdit4("Light Diffuse", diffuseCol);
    ImGui::ColorEdit4("Light Ambient", ambientCol);
    ImGui::ColorEdit4("Light Specular", specularCol);

    ImGui::Separator();
    ImGui::Spacing();

    static bool autoUpdateLight = false;
    ImGui::Checkbox("Auto Update Light", &autoUpdateLight);

    ImGui::Spacing();

    if (ImGui::Button("Set LightDesc") || autoUpdateLight)
    {
        desc.eType = static_cast<LIGHT_DESC::TYPE>(selectedLightType);
        desc.vDiffuse = _float4{ diffuseCol[0], diffuseCol[1], diffuseCol[2], diffuseCol[3] };
        desc.vAmbient = _float4{ ambientCol[0], ambientCol[1], ambientCol[2], ambientCol[3] };
        desc.vSpecular = _float4{ specularCol[0], specularCol[1], specularCol[2], specularCol[3] };

        switch (selectedLightType)
        {
        case 0: // Directional
            desc.vDirection = direction;
            break;
        case 1: // Point
            desc.vPosition = position;
            desc.fRange = range;
            break;
        case 2: // Spotlight
            desc.vDirection = direction;
            desc.vPosition = position;
            desc.fRange = range;
            desc.innerAngle = XMConvertToRadians(innerAngle);
            desc.outerAngle = XMConvertToRadians(outerAngle);
            break;
        }

        m_pGameInstance->Edit_Light(lightIdx, &desc);
    }

    ImGui::Spacing();

    if (ImGui::Button("Add Light"))
    {
        desc.eType = static_cast<LIGHT_DESC::TYPE>(selectedLightType);
        desc.vDiffuse = _float4{ diffuseCol[0], diffuseCol[1], diffuseCol[2], diffuseCol[3] };
        desc.vAmbient = _float4{ ambientCol[0], ambientCol[1], ambientCol[2], ambientCol[3] };
        desc.vSpecular = _float4{ specularCol[0], specularCol[1], specularCol[2], specularCol[3] };

        switch (selectedLightType)
        {
        case 0: // Directional
            desc.vDirection = direction;
            break;
        case 1: // Point
            desc.vPosition = position;
            desc.fRange = range;
            break;
        case 2: // Spotlight
            desc.vDirection = direction;
            desc.vPosition = position;
            desc.fRange = range;
            desc.innerAngle = XMConvertToRadians(innerAngle);
            desc.outerAngle = XMConvertToRadians(outerAngle);
            break;
        }

        m_pGameInstance->Add_Light(desc);
    }

    ImGui::Spacing();


    ImGui::Separator();
    ImGui::Spacing();
    if (ImGui::Button("Save Lights"))
        Save_Lights();

    ImGui::SameLine();
    if (ImGui::Button("Load Lights"))
        Load_Lights();

    ImGui::SameLine();
    if (ImGui::Button("Clear Lights"))
    {
        m_pGameInstance->LightOff_All();
        lightIdx = 0;
    }




    ImGui::Spacing();

    if (ImGui::Button("Close"))
        m_bLightWindow = false;

    ImGui::End();
}

void CImgui_Manager::Save_Lights()
{
    int iStageIdx = Get_StageIdx();
    switch (iStageIdx)
    {
    case STAGE_HOME:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage_Lights.dat";
        break;
    case STAGE_ONE:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage1_Lights.dat";
        break;
    case STAGE_TWO:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage2_Lights.dat";
        break;
    case STAGE_THREE:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage3_Lights.dat";
        break;
    case STAGE_BOSS:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage4_Lights.dat";
        break;
    default:
        MSG_BOX("Setting File Name is Failed");
        return;
    }

    HANDLE hFile = CreateFile(m_LightsDataPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (nullptr == hFile)
    {
        MSG_BOX("hFile is nullptr");
        return;
    }

    DWORD dwByte(0);

    list<CLight*> lights = m_pGameInstance->Get_Lights();

    _uint iLightCount = lights.size();
    WriteFile(hFile, &iLightCount, sizeof(_uint), &dwByte, nullptr);

    for (auto& iter : lights)
    {
        if (!iter->Get_LightOn()) continue;

        LIGHT_DESC desc{};
        desc = *iter->Get_LightDesc();

        WriteFile(hFile, &desc.eType, sizeof(LIGHT_DESC::TYPE), &dwByte, nullptr);
        WriteFile(hFile, &desc.vDiffuse, sizeof(XMFLOAT4), &dwByte, nullptr);
        WriteFile(hFile, &desc.vAmbient, sizeof(XMFLOAT4), &dwByte, nullptr);
        WriteFile(hFile, &desc.vSpecular, sizeof(XMFLOAT4), &dwByte, nullptr);

        switch (desc.eType)
        {
        case LIGHT_DESC::TYPE_DIRECTIONAL:
            WriteFile(hFile, &desc.vDirection, sizeof(XMFLOAT4), &dwByte, nullptr);
            break;
        case LIGHT_DESC::TYPE_POINT:
            WriteFile(hFile, &desc.vPosition, sizeof(XMFLOAT4), &dwByte, nullptr);
            WriteFile(hFile, &desc.fRange, sizeof(float), &dwByte, nullptr);
            break;
        case LIGHT_DESC::TYPE_SPOTLIGHT:
            WriteFile(hFile, &desc.vDirection, sizeof(XMFLOAT4), &dwByte, nullptr);
            WriteFile(hFile, &desc.vPosition, sizeof(XMFLOAT4), &dwByte, nullptr);
            WriteFile(hFile, &desc.fRange, sizeof(float), &dwByte, nullptr);
            WriteFile(hFile, &desc.innerAngle, sizeof(float), &dwByte, nullptr);
            WriteFile(hFile, &desc.outerAngle, sizeof(float), &dwByte, nullptr);
            break;
        }
    }

    CloseHandle(hFile);
    MSG_BOX("Lights Data Save");

    return;
}

void CImgui_Manager::Load_Lights()
{
    list<CLight*> lights = m_pGameInstance->Get_Lights();
    for (auto& iter : lights)
    {
        iter->LightOff();
    }

    int iStageIdx = CImgui_Manager::GetInstance()->Get_StageIdx();
    switch (iStageIdx)
    {
    case STAGE_HOME:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage_Lights.dat";
        break;
    case STAGE_ONE:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage1_Lights.dat";
        break;
    case STAGE_TWO:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage2_Lights.dat";
        break;
    case STAGE_THREE:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage3_Lights.dat";
        break;
    case STAGE_BOSS:
        m_LightsDataPath = L"../Bin/MapData/LightsData/Stage4_Lights.dat";
        break;
    default:
        MSG_BOX("Setting File Name is Failed");
        return;
    }

    HANDLE hFile = CreateFile(m_LightsDataPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (nullptr == hFile)
    {
        MSG_BOX("hFile is nullptr");
        return;
    }

    DWORD dwByte(0);
    _uint iLightCount = 0;
    ReadFile(hFile, &iLightCount, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < iLightCount; ++i)
    {
        LIGHT_DESC desc{};
        ReadFile(hFile, &desc.eType, sizeof(LIGHT_DESC::TYPE), &dwByte, nullptr);
        ReadFile(hFile, &desc.vDiffuse, sizeof(XMFLOAT4), &dwByte, nullptr);
        ReadFile(hFile, &desc.vAmbient, sizeof(XMFLOAT4), &dwByte, nullptr);
        ReadFile(hFile, &desc.vSpecular, sizeof(XMFLOAT4), &dwByte, nullptr);

        switch (desc.eType)
        {
        case LIGHT_DESC::TYPE_DIRECTIONAL:
            ReadFile(hFile, &desc.vDirection, sizeof(XMFLOAT4), &dwByte, nullptr);
            break;
        case LIGHT_DESC::TYPE_POINT:
            ReadFile(hFile, &desc.vPosition, sizeof(XMFLOAT4), &dwByte, nullptr);
            ReadFile(hFile, &desc.fRange, sizeof(float), &dwByte, nullptr);
            break;
        case LIGHT_DESC::TYPE_SPOTLIGHT:
            ReadFile(hFile, &desc.vDirection, sizeof(XMFLOAT4), &dwByte, nullptr);
            ReadFile(hFile, &desc.vPosition, sizeof(XMFLOAT4), &dwByte, nullptr);
            ReadFile(hFile, &desc.fRange, sizeof(float), &dwByte, nullptr);
            ReadFile(hFile, &desc.innerAngle, sizeof(float), &dwByte, nullptr);
            ReadFile(hFile, &desc.outerAngle, sizeof(float), &dwByte, nullptr);
            break;
        }

        if (0 == dwByte)
            break;

        if (FAILED(m_pGameInstance->Add_Light(desc)))
        {
            MSG_BOX("Failed to Add Light");
            return;
        }
    }

    CloseHandle(hFile);
    MSG_BOX("Lights Data Load");

    return;
}

void CImgui_Manager::Shadow_Editor()
{
    ImGui::Begin("Shadow Settings", &m_bShadowWindow);
    m_bShowAnotherWindow = false;
    m_bLightWindow = false;

    static float ShadowEye[4] = { 0.f, 10.f, -10.f, 1.f };
    static float ShadowFocus[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    static int selectedPosition = 0; // 0 for Eye, 1 for Focus
    static bool autoUpdateShadow = false;
    static float shadowThreshold = 0.5f; // 새로운 float 변수 추가

    const char* positionTypes[] = { "Eye", "Focus" };
    ImGui::Combo("Select Position", &selectedPosition, positionTypes, IM_ARRAYSIZE(positionTypes));

    // Gizmo 세팅
    float cameraView[16];
    float cameraProjection[16];
    memcpy(cameraView, CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW), sizeof(float) * 16);
    memcpy(cameraProjection, CGameInstance::GetInstance()->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ), sizeof(float) * 16);

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImGuizmo::SetRect(0, 0, displaySize.x, displaySize.y);

    // Shadow Eye/Focus 위치를 나타내는 행렬 생성
    XMFLOAT4X4 shadowMatrix;
    XMStoreFloat4x4(&shadowMatrix, XMMatrixTranslation(
        selectedPosition == 0 ? ShadowEye[0] : ShadowFocus[0],
        selectedPosition == 0 ? ShadowEye[1] : ShadowFocus[1],
        selectedPosition == 0 ? ShadowEye[2] : ShadowFocus[2]
    ));

    bool positionChanged = false;

    if (ImGuizmo::Manipulate(cameraView, cameraProjection, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&shadowMatrix, NULL, NULL))
    {
        // 조작된 행렬에서 위치 추출
        XMFLOAT4 newPosition;
        XMStoreFloat4(&newPosition, XMLoadFloat4((XMFLOAT4*)&shadowMatrix._41));

        if (selectedPosition == 0)
        {
            ShadowEye[0] = newPosition.x;
            ShadowEye[1] = newPosition.y;
            ShadowEye[2] = newPosition.z;
        }
        else
        {
            ShadowFocus[0] = newPosition.x;
            ShadowFocus[1] = newPosition.y;
            ShadowFocus[2] = newPosition.z;
        }
        positionChanged = true;
    }

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Shadow Threshold");

    ImGui::SliderFloat("##ShadowParam", &shadowThreshold, 0.0f, 1.1f, "%.2f");
   
    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Shadow Eye");
    if (ImGui::InputFloat3("##EyeInput", ShadowEye, "%.2f"))
        positionChanged = true;

    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Shadow Focus");
    if (ImGui::InputFloat3("##FocusInput", ShadowFocus, "%.2f"))
        positionChanged = true;

    ImGui::Checkbox("Auto Update Shadow", &autoUpdateShadow);

    if (ImGui::Button("Set Shadow") || (autoUpdateShadow && positionChanged))
    {
        _vector vEye = XMLoadFloat4((XMFLOAT4*)ShadowEye);
        _vector vFocus = XMLoadFloat4((XMFLOAT4*)ShadowFocus);
        m_pGameInstance->Set_ShadowEyeFocus(vEye, vFocus, shadowThreshold);
    }

    ImGui::End();
}

void CImgui_Manager::Camera_Editor()
{
    // Set color scheme
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.06f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

    ImGui::Begin("Camera Settings", &m_bCameraWindow);

    // Camera Editing Toggle
    static bool bCameraEditing = false;
    ImGui::Checkbox("Camera Editing", &bCameraEditing);

    if (bCameraEditing)
    {
        // Section: Camera KeyFrame Controls
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.9f, 1.0f), "Camera KeyFrame Controls");

        if (ImGui::Button("Save Camera KeyFrame"))
        {
            CCutSceneCamera::CameraKeyFrame keyFrame;
            keyFrame.fTime = m_fKeyFrameTime;
            m_fKeyFrameTime += 0.5f;
            keyFrame.matWorld = *m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);
            keyFrame.fFovy = XMConvertToRadians(60.f);
            keyFrame.fNear = 0.1f;
            keyFrame.fFar = 3000.f;

            m_vCameraKeyFrames.emplace_back(keyFrame);
        }

        ImGui::SameLine();
        if (ImGui::Button("Pop Camera KeyFrame"))
        {
            if (!m_vCameraKeyFrames.empty())
                m_vCameraKeyFrames.pop_back();
        }

        ImGui::SameLine();
        if (ImGui::Button("Clear Camera KeyFrames"))
        {
            m_vCameraKeyFrames.clear();
        }

        // Section: Current Time Display
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.9f, 1.0f), "Current Time");
        ImGui::Text("Time Now: %.2f" /* m_pGameInstance->Get_Cameras().back() */);

        // Section: KeyFrames List
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "KeyFrames:");

        // 스크롤 가능한 영역 시작
        float itemHeight = ImGui::GetTextLineHeightWithSpacing();
        ImGui::BeginChild("KeyframesScroll", ImVec2(0, 5 * itemHeight + ImGui::GetStyle().ScrollbarSize), true);

        for (size_t i = 0; i < m_vCameraKeyFrames.size(); ++i)
        {
            ImGui::PushID(static_cast<int>(i));

            // 키프레임 선택을 위한 셀렉터블 추가
            char label[32];
            snprintf(label, sizeof(label), "KeyFrame %d", static_cast<int>(i));
            if (ImGui::Selectable(label, m_iSelectedKeyFrame == i))
            {
                m_iSelectedKeyFrame = i;
            }

            ImGui::PopID();
        }

        ImGui::EndChild();

        // 선택된 키프레임 편집
        if (m_iSelectedKeyFrame >= 0 && m_iSelectedKeyFrame < m_vCameraKeyFrames.size())
        {
            CCamera::CameraKeyFrame& keyFrame = m_vCameraKeyFrames[m_iSelectedKeyFrame];

            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.3f, 1.0f), "Editing KeyFrame %d", m_iSelectedKeyFrame);

            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            ImGui::SliderFloat("Total Time", &keyFrame.fTime, 0.0f, 10.0f);

            float fovDegrees = XMConvertToDegrees(keyFrame.fFovy);
            if (ImGui::SliderFloat("FOV", &fovDegrees, 1.0f, 179.0f))
            {
                keyFrame.fFovy = XMConvertToRadians(fovDegrees);
            }
            ImGui::PopStyleColor();

            // Speed Changes SubSection
            ImGui::TextColored(ImVec4(0.3f, 0.7f, 0.9f, 1.f), "Speed Changes:");
            for (size_t j = 0; j < keyFrame.speedChanges.size(); ++j)
            {
                ImGui::PushID(static_cast<int>(j));
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Speed Change %d", static_cast<int>(j));
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                ImGui::SliderFloat("Timing", &get<0>(keyFrame.speedChanges[j]), 0.0f, 1.0f);
                ImGui::SliderFloat("Speed", &get<1>(keyFrame.speedChanges[j]), 0.1f, 10.0f);
                ImGui::SliderFloat("Smooth Offset", &get<2>(keyFrame.speedChanges[j]), 0.0f, 1.0f);
                ImGui::PopStyleColor();
                ImGui::PopID();
            }

            if (ImGui::Button("Add Speed Change"))
            {
                keyFrame.speedChanges.emplace_back(0.0f, 1.0f, 0.3f);
            }

            // Speed Change Graph 부분 수정
            if (!keyFrame.speedChanges.empty())
            {
                ImGui::Spacing();
                ImGui::Text("Speed Change Graph:");
                ImVec2 graphSize(250, 120); // 그래프 크기
                ImVec2 graphPos = ImGui::GetCursorScreenPos();
                graphPos.x += 30; // 그래프를 오른쪽으로 30픽셀 이동

                // 그래프 배경 및 격자 그리기
                ImGui::GetWindowDrawList()->AddRectFilled(
                    graphPos,
                    ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y),
                    IM_COL32(30, 30, 30, 255));

                // 수평 격자선 (y축 레이블 수정)
                for (int i = 0; i <= 5; i++) {
                    float y = graphPos.y + (i * graphSize.y / 5);
                    ImGui::GetWindowDrawList()->AddLine(
                        ImVec2(graphPos.x, y),
                        ImVec2(graphPos.x + graphSize.x, y),
                        IM_COL32(100, 100, 100, 100));
                    char label[16];
                    snprintf(label, sizeof(label), "%.f", (5 - i) * 2.0f);
                    ImGui::GetWindowDrawList()->AddText(
                        ImVec2(graphPos.x + graphSize.x + 10, y - 7),
                        IM_COL32(200, 200, 200, 150),
                        label);
                }

                // y축 레이블 추가
                ImGui::GetWindowDrawList()->AddText(
                    ImVec2(graphPos.x + graphSize.x +  25, graphPos.y - 15),
                    IM_COL32(126, 66, 245, 200),
                    "Speed");

                // 수직 격자선 (x축 레이블 수정)
                for (int i = 0; i <= 10; i++) {
                    float x = graphPos.x + (i * graphSize.x / 10);
                    ImGui::GetWindowDrawList()->AddLine(
                        ImVec2(x, graphPos.y),
                        ImVec2(x, graphPos.y + graphSize.y),
                        IM_COL32(100, 100, 100, 100));

                    // 0에서 10까지의 정수 값으로 레이블 수정
                    char label[16];
                    snprintf(label, sizeof(label), "%d", i);
                    ImGui::GetWindowDrawList()->AddText(
                        ImVec2(x - 10, graphPos.y + graphSize.y + 5),
                        IM_COL32(200, 200, 200, 150),
                        label);
                }

                // x축 레이블 추가
                ImGui::GetWindowDrawList()->AddText(
                    ImVec2(graphPos.x + graphSize.x + 25, graphPos.y + graphSize.y - 5),
                    IM_COL32(126, 66, 245, 200),
                    "Time");

                // 속도 그래프 그리기
                std::vector<ImVec2> points;
                for (int i = 0; i <= 100; i++) {
                    float t = static_cast<float>(i) / 100.0f;
                    float speedMultiplier = 1.0f;
                    float prevSpeedMultiplier = 1.0f;

                    for (const auto& [changeTime, changeSpeed, smoothOffset] : keyFrame.speedChanges)
                    {
                        if (t < changeTime - smoothOffset)
                        {
                            speedMultiplier = prevSpeedMultiplier;
                            break;
                        }
                        else if (t >= changeTime - smoothOffset && t < changeTime + smoothOffset)
                        {
                            float lerpT = (t - (changeTime - smoothOffset)) / (2.0f * smoothOffset);
                            speedMultiplier = prevSpeedMultiplier + (changeSpeed - prevSpeedMultiplier) * lerpT * lerpT * (3.0f - 2.0f * lerpT);
                            break;
                        }
                        else
                        {
                            prevSpeedMultiplier = changeSpeed;
                        }
                    }

                    points.push_back(ImVec2(
                        graphPos.x + t * graphSize.x,
                        graphPos.y + graphSize.y - (speedMultiplier / 10.0f) * graphSize.y
                    ));
                }

                // 선 그리기
                for (size_t i = 1; i < points.size(); i++) {
                    ImGui::GetWindowDrawList()->AddLine(points[i - 1], points[i], IM_COL32(0, 255, 0, 255), 2.0f);
                }

                // 속도 변화 지점 표시
                for (const auto& [changeTime, changeSpeed, smoothOffset] : keyFrame.speedChanges)
                {
                    float x = graphPos.x + changeTime * graphSize.x;
                    float y = graphPos.y + graphSize.y - (changeSpeed / 10.0f) * graphSize.y;
                    ImGui::GetWindowDrawList()->AddCircleFilled(
                        ImVec2(x, y), 4.0f, IM_COL32(255, 0, 0, 255));
                }

                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Dummy(graphSize);  // 그래프 영역만큼 공간 확보
            }
        }

        // Section: Camera Controls
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.9f, 1.0f), "Camera Controls");

        if (ImGui::Button("Add Camera"))
        {
            CCutSceneCamera::CUTSCENECAMERA_DESC CSCdesc{};

            CSCdesc.vEye = _float4(50.f, 100.f, -10.f, 1.f);
            CSCdesc.vAt = _float4(0.f, 30.f, 0.f, 1.f);
            CSCdesc.fFovy = XMConvertToRadians(60.f);
            CSCdesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
            CSCdesc.fNear = 0.1f;
            CSCdesc.fFar = 3000.f;
            CSCdesc.fSpeedPerSec = 40.f;
            CSCdesc.fRotationPerSec = XMConvertToRadians(90.f);

            CSCdesc.KeyFrames = m_vCameraKeyFrames;
            m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_CutSceneCamera"), &CSCdesc);
        }

        ImGui::SameLine();
        if (ImGui::Button("Return to FreeCamera"))
        {
            m_pGameInstance->Set_MainCamera(0);
            m_fKeyFrameTime = 0.f;
        }
    }

    ImGui::End();

    // Pop color styles
    ImGui::PopStyleColor(4);
}

void CImgui_Manager::Terrain_Editor()
{
    static char heightMapPath[256] = "../Bin/Resources/Textures/Terrain/heightmap.r16";
    static float maxHeight = 100.0f;
    static float brushSize = 10.0f;
    static float brushStrength = 0.1f;
    static int brushMode = 0;
    static float flattenHeight = 50.0f;
    static bool bShowBrush = false;

    ImGui::Begin("Terrain Editor");

    // 높이맵 섹션
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Height Map");
    ImGui::InputText("Height Map Path", heightMapPath, IM_ARRAYSIZE(heightMapPath));

    // Load 버튼
    if (ImGui::Button("Load Height Map"))
    {
        CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), 0));
        if (pTerrain)
        {
            std::wstring wHeightMapPath;
            wHeightMapPath.assign(heightMapPath, heightMapPath + strlen(heightMapPath));
            pTerrain->LoadHeightMap(wHeightMapPath);
        }
    }

    // Save 버튼
    ImGui::SameLine();
    if (ImGui::Button("Save Height Map"))
    {
        CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), 0));
        if (pTerrain)
        {
            std::wstring wHeightMapPath;
            wHeightMapPath.assign(heightMapPath, heightMapPath + strlen(heightMapPath));
            if (SUCCEEDED(pTerrain->SaveHeightMapToR16(wHeightMapPath)))
            {
                ImGui::OpenPopup("Save Successful");
            }
            else
            {
                ImGui::OpenPopup("Save Failed");
            }
        }
    }

    // 성공/실패 팝업
    if (ImGui::BeginPopupModal("Save Successful", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Height map saved successfully.");
        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("Save Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Failed to save height map.");
        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    ImGui::SliderFloat("Max Height", &maxHeight, 10.0f, 500.0f);
    ImGui::SliderFloat("Brush Size", &brushSize, 1.0f, 100.0f);
    ImGui::SliderFloat("Brush Strength", &brushStrength, 0.01f, 1.0f);

    ImGui::Separator();

    // 브러쉬 모드 선택
    const char* brushModes[] = { "Raise", "Lower", "Flatten" };
    ImGui::Combo("Brush Mode", &brushMode, brushModes, IM_ARRAYSIZE(brushModes));

    // 평탄화 높이 설정 (Flatten 모드에서 사용)
    if (brushMode == 2) // Flatten 모드
    {
        ImGui::SliderFloat("Flatten Height", &flattenHeight, 0.0f, maxHeight);
    }

    ImGui::Separator();

    // 브러쉬 섹션
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Brush");
    ImGui::Checkbox("Show Brush", &bShowBrush);

    if (bShowBrush && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered())
    {
        _bool isPick = false;
        _float4 pPickPos = { 0.f, 0.f, 0.f, 1.f };
        if (m_pGameInstance->Get_PickPos(&pPickPos))
        {
            CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), 0));
            if (pTerrain)
            {
                switch (brushMode)
                {
                case 0: // Raise
                    pTerrain->Brush_Picking(XMLoadFloat4(&pPickPos), brushSize, brushStrength, maxHeight);
                    break;
                case 1: // Lower
                    pTerrain->Brush_Picking(XMLoadFloat4(&pPickPos), brushSize, -brushStrength, maxHeight);
                    break;
                case 2: // Flatten
                    pTerrain->Brush_Flatten(XMLoadFloat4(&pPickPos), brushSize, brushStrength, flattenHeight);
                    break;
                }
            }
        }
    }

    ImGui::End();
}

//void CImgui_Manager::Table_Channel()
//{s
//    ImGui::Text("Current Key Frame");
//    if (ImGui::BeginTable("Channel Keyframe", 4))
//    {
//        for (int row = 0; row < m_vecChannel.size() * 0.5; row++)
//        {
//            ImGui::TableNextRow();
//            for (int column = 0; column < 4; column++)
//            {
//                ImGui::TableSetColumnIndex(column);
//
//                int i = 0;
//                if (0 == column % 2)
//                {
//                    if (0 == column)
//                        i = 0;
//                    else
//                        i = 1;
//
//                    ImGui::Text(m_vecChannel[row + (i * m_vecChannel.size() * 0.5)]);
//                }
//                else
//                {
//                    if (1 == column)
//                        i = 0;
//                    else
//                        i = 1;
//
//                    _uint iCurKeyframe = CToolObj_Manager::GetInstance()->Get_CurKeyFrame(m_iAnimIdx, row + (i * m_vecChannel.size() * 0.5));
//
//                    ImGui::Text("\t %d", iCurKeyframe);
//                }
//            }
//        }
//        ImGui::EndTable();
//    }
//}


void CImgui_Manager::Setting_CreateObj_ListBox()
{
    if (m_IsNaviMode) // Navi 상태 활성화
    {
        static int Cell_current_idx = 0;
        if (ImGui::BeginListBox("##Obj", ImVec2(300, 200)))
        {
            for (int n = 0; n < m_vecCreateCell.size(); n++)
            {
                const bool is_selected = (Cell_current_idx == n);
                if (ImGui::Selectable(m_vecCreateCell[n], is_selected)) // Cell 추가 시 요쪽에 오류 발생
                {
                    Cell_current_idx = n;
                    m_iCreateCellIdx = Cell_current_idx;
                    //CNaviMgr::GetInstance()->Check_Cell(m_iCreateCellIdx); // 이렇게 전달해줄 게 아니고 그냥 Terrain에서 Imgui의 Get 함수를 받아오는게?
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
    }
    else
    {
        static int item_current_idx = 0;
        if (ImGui::BeginListBox("##Obj", ImVec2(300, 200)))
        {
            for (int n = 0; n < m_vecCreateObj.size(); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(m_vecCreateObj[n], is_selected))
                {
                    item_current_idx = n;
                    m_iCreateObjIdx = item_current_idx;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
    }
}

void CImgui_Manager::Delete_Obj()
{
    //if (m_IsNaviMode) // Navigation 모드 On 일 때
    //{
    //    m_isNaviDelete = true;
    //}
    //else
    {
        CToolObj_Manager::GetInstance()->Delete_Obj(m_iCreateObjIdx); // Obj 제거

        // 생성 목록에서 이름 제거
        vector<_char*>::iterator iter = m_vecCreateObj.begin();

        for (int i = 0; i < m_iCreateObjIdx; ++i)
            ++iter;

        //이벤트 매니저로 삭제
        //Safe_Delete(*iter);
        //m_pGameInstance->Erase(iter);
        m_vecCreateObj.erase(iter);
    }
}

void CImgui_Manager::Add_vecCreateCellSize()
{
    _char szName[MAX_PATH] = "";
    sprintf(szName, "%d", m_vecCreateCell.size());
    m_vecCreateCell.emplace_back(szName);
}

void CImgui_Manager::Clear_vecCell()
{
    for (auto& iter : m_vecCreateCell)
    {
        Safe_Delete(iter);
    }
    m_vecCreateCell.clear();
}

void CImgui_Manager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);

    //for (_int i = 0; i < m_vecCreateObj.size(); ++i)
    //{
    //    Safe_Release(m_vecCreateObj[i]);
    //}

    //for (auto& iter : m_vecCreateObj)
    //{
    //    Safe_Delete(iter);
    //}
    //Safe_Delete_Array(m_vecCreateObj);
    //m_vecCreateObj.clear();

    //for (auto& iter : m_vecCreateCell)
    //{
    //    Safe_Delete(iter);
    //}
    //Safe_Delete_Array(m_vecCreateCell);
    //m_vecCreateCell.clear();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}