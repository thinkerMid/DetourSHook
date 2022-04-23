#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include "detours.h"
#pragma comment(lib,"detours.lib")  


static int (WINAPI *oldmessagebox)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) = MessageBoxA;
//int(WINAPI *oldmessagebox)(
//     HWND hWnd,
//     LPCSTR lpText,
//     LPCSTR lpCaption,
//     UINT uType) = MessageBoxA;


//新函数
int
WINAPI
newMessageBoxA(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCSTR lpText,
	_In_opt_ LPCSTR lpCaption,
	_In_ UINT uType)
{
	return 0;
}

void hook() 
{
	DetourRestoreAfterWith();//恢复原来状态
	DetourTransactionBegin();//拦截开始
	DetourUpdateThread(GetCurrentThread());//刷新当前线程
	// 这里可以多次连续调用DetourAttrach 表明撤销hook 多个函数
	DetourAttach((PVOID*)&oldmessagebox, newMessageBoxA);
	DetourTransactionCommit();//拦截生效
}


void unhook() 
{
	DetourTransactionBegin();//拦截开始
	DetourUpdateThread(GetCurrentThread());              //刷新当前线程
	DetourDetach((void**)&oldmessagebox, newMessageBoxA);  //撤销拦截
	DetourTransactionCommit();                           //拦截生效
}


int main()
{
	MessageBoxA(0, "abcd", "test", 0);
	hook();
	MessageBoxA(0, "1234", "test", 0);
	system("pause");
}
