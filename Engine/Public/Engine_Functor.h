#pragma once
#include "Engine_Defines.h"


namespace Engine
{
    inline float RandomFloat(float min, float max)      //min 과 max 사이의 flaot 값을 배출해준다. 음수는 불가능
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    inline int RandomInt(int min, int max)              //min과 max 사이의 int값을 배출해준다. 음수 불가능
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(min, max);
        return dis(gen);
    }

    inline int RandomSign() //-1 혹은 1의 int형을 반환해줌
    {
        float randomNumber = RandomFloat(-1.0f, 1.0f);
        if (randomNumber < 0)
            return -1;
        else
            return 1;
    }

    // _float3를 위한 더하기 함수 객체
    struct Float3Add
    {
        _float3 operator()(const _float3& lhs, const _float3& rhs) const
        {
            return _float3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
        }
    };

// _float3를 위한 스칼라 곱셈 함수 객체
    struct Float3Multiply
    {
        _float3 operator()(const _float3& v, float scalar) const
        {
            return _float3(v.x * scalar, v.y * scalar, v.z * scalar);
        }
    };


    //matrix를 선형보간 해주는 함수
    inline _matrix XMMatrixSlerp(_matrix matStart, _matrix matEnd, float _t)
    {
        // 행렬에서 회전 부분을 쿼터니언으로 추출
        _vector qStart = XMQuaternionRotationMatrix(matStart);
        _vector qEnd = XMQuaternionRotationMatrix(matEnd);

        // 쿼터니언 구면 선형 보간
        _vector qInterpolated = XMQuaternionSlerp(qStart, qEnd, _t);

        // 행렬에서 Right 벡터, Up 벡터, Look 벡터 추출
        _vector vStartRight = matStart.r[0];
        _vector vEndRight = matEnd.r[0];
        _vector vStartUp = matStart.r[1];
        _vector vEndUp = matEnd.r[1];
        _vector vStartLook = matStart.r[2];
        _vector vEndLook = matEnd.r[2];
        _vector vStartTranslation = matStart.r[3];
        _vector vEndTranslation = matEnd.r[3];

        // Right 벡터, Up 벡터, Look, Position 벡터 선형 보간
        _vector vInterpolatedRight = XMVectorLerp(vStartRight, vEndRight, _t);
        _vector vInterpolatedUp = XMVectorLerp(vStartUp, vEndUp, _t);
        _vector vInterpolatedLook = XMVectorLerp(vStartLook, vEndLook, _t);
        _vector vInterpolatedTranslation = XMVectorLerp(vStartTranslation, vEndTranslation, _t);

        // 보간된 회전, 이동, Right 벡터, Up 벡터, Look 벡터를 조합하여 최종 행렬 생성
        _matrix matResult = XMMatrixRotationQuaternion(qInterpolated);
        matResult.r[0] = XMVector3Normalize(vInterpolatedRight);
        matResult.r[1] = XMVector3Normalize(vInterpolatedUp);
        matResult.r[2] = XMVector3Normalize(vInterpolatedLook);
        matResult.r[3] = vInterpolatedTranslation;

        return matResult;
    }

    inline string wstring_to_string(const std::wstring& wstr) {
        if (wstr.empty()) return std::string();
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
        return strTo;
    }

    // string을 wstring으로 변환하는 함수 (UTF-8)
    inline wstring string_to_wstring(const std::string& str) {
        if (str.empty()) return std::wstring();
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }

    inline void save_wstring_to_stream(const std::wstring& wstr, std::ostream& os) {
        // wstring을 string으로 변환
        std::string utf8_string = wstring_to_string(wstr);

        // string 데이터의 길이 가져오기
        std::size_t size = utf8_string.size();

        // 데이터 크기 먼저 쓰기
        os.write(reinterpret_cast<const char*>(&size), sizeof(size));

        // string 데이터를 바이너리로 스트림에 쓰기
        os.write(utf8_string.c_str(), size);
    }

    inline wstring load_wstring_from_stream(std::istream& is) {
        // 데이터 크기 읽기
        std::size_t size;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));

        // string 데이터 읽기
        std::string utf8_string(size, '\0');
        is.read(&utf8_string[0], size);

        // string을 wstring으로 변환
        return string_to_wstring(utf8_string);
    }


    inline wstring const_char_to_wstring(const char* str) {
		std::string utf8_string = str;
		return string_to_wstring(utf8_string);
	}

    //_float4, _float3, _float2 ==연산이랑 != 연산 가능하도록 오버라이딩
    inline bool operator==(const XMFLOAT4& lhs, const XMFLOAT4& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
    }

    inline bool operator!=(const XMFLOAT4& lhs, const XMFLOAT4& rhs) {
        return !(lhs == rhs);
    }

    inline bool operator==(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }

    inline bool operator!=(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
        return !(lhs == rhs);
    }

    inline bool operator==(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    inline bool operator!=(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
        return !(lhs == rhs);
    }


    //void CreatePhysXProcess(LPCWSTR  applicationName)
    //{
    //    STARTUPINFO si = {};
    //    PROCESS_INFORMATION pi = {};
    //    ZeroMemory(&si, sizeof(si));
    //    si.cb = sizeof(si);
    //    ZeroMemory(&pi, sizeof(pi));

    //    //= L"C:\\Program Files (x86)\\NVIDIA Corporation\\PhysX Visual Debugger 3\\PVD3.exe";
    //    LPWSTR commandLine = _wcsdup(applicationName);

    //    auto createProcessLambda = [&]() -> bool {
    //        return CreateProcess(
    //            applicationName,   // 실행할 프로그램의 이름
    //            commandLine,       // 명령줄 인자
    //            NULL,              // 기본 프로세스 보안 속성
    //            NULL,              // 기본 스레드 보안 속성
    //            FALSE,             // 핸들 상속 비활성화
    //            0,                 // 생성 플래그
    //            NULL,              // 부모 프로세스 환경 변수 사용
    //            NULL,              // 부모 프로세스의 현재 디렉토리 사용
    //            &si,               // STARTUPINFO 구조체
    //            &pi                // PROCESS_INFORMATION 구조체
    //        );
    //        };
    //    if (!createProcessLambda())
    //    {
    //        DWORD error = GetLastError();
    //        wprintf(L"CreateProcess failed (%d).\n", error);
    //    }
    //    else
    //    {
    //        // 프로세스와 스레드 핸들을 닫음
    //        CloseHandle(pi.hProcess);
    //        CloseHandle(pi.hThread);
    //    }

    //    free(commandLine);


    //}


}

extern "C"
{
    void Free_String(wstring* wstrName)
    {
        wstrName->clear();


    }
}