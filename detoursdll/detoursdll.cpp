// detoursdll.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "framework.h"
#include "detoursdll.h"


// 这是导出变量的一个示例
DETOURSDLL_API int ndetoursdll=0;

// 这是导出函数的一个示例。
DETOURSDLL_API int fndetoursdll(void)
{
    return 0;
}

// 这是已导出类的构造函数。
Cdetoursdll::Cdetoursdll()
{
    return;
}
