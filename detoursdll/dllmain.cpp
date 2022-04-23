// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <tchar.h>
#include "detours.h"
#pragma comment(lib, "detours.lib")

//sleep的函数指针  加入static 防止外部文件调用
static VOID(WINAPI* TureSleep)(DWORD dwMilliseconds) = Sleep;

VOID WINAPI hkSleep(DWORD dwMilliseconds)
{
    ULONGLONG dwBeg = GetTickCount64();
    TureSleep(dwMilliseconds);
    ULONGLONG dwEnd = GetTickCount64();

    TCHAR buffer[512];
    _stprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR), _T("Sleeped %llu milli sec"), dwEnd - dwBeg);
    MessageBox(NULL, buffer, _T(""), MB_OK);
}

// hook DeviceIoControl
// 定义DeviceIoControl的函数指针
static BOOL (WINAPI *pDeviceIoControl)(
    _In_ HANDLE hDevice,
    _In_ DWORD dwIoControlCode,
    _In_reads_bytes_opt_(nInBufferSize) LPVOID lpInBuffer,
    _In_ DWORD nInBufferSize,
    _Out_writes_bytes_to_opt_(nOutBufferSize, *lpBytesReturned) LPVOID lpOutBuffer,
    _In_ DWORD nOutBufferSize,
    _Out_opt_ LPDWORD lpBytesReturned,
    _Inout_opt_ LPOVERLAPPED lpOverlapped
) = DeviceIoControl;

// 这里直接赋值 不知道地址是不是赋值上了
//也可以这样 Demo 获取到loadlibrary 的函数地址
//static HMODULE(WINAPI* PFnLoadLibraryExW)(LPCWSTR lpLibFileName, HANDLE  hFile, DWORD   dwFlags) = (HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD))DetourFindFunction("KernelBase.dll", "LoadLibraryExW");





BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
        DetourRestoreAfterWith();//恢复原来状态


        DetourTransactionBegin();//拦截开始
        DetourUpdateThread(GetCurrentThread());//刷新当前线程
        //这里可以连续多次调用DetourAttach，表明HOOK多个函数
        DetourAttach(&(PVOID&)TureSleep, hkSleep);//实现函数拦截
        DetourTransactionCommit();//拦截生效
        break;
    case DLL_THREAD_DETACH:
        DetourTransactionBegin();//拦截开始
        DetourUpdateThread(GetCurrentThread());//刷新当前线程
        //这里可以连续多次调用DetourAttach，表明撤销HOOK多个函数
        DetourDetach(&(PVOID&)TureSleep, hkSleep);//撤销拦截
        DetourTransactionCommit();//撤销拦截生效
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

