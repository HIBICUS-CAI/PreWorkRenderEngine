#include "Header.h"
#include <iostream>
//#include <Windows.h>
//
//BOOL APIENTRY DllMain(HANDLE hModule,
//    DWORD  ul_reason_for_call,
//    LPVOID lpReserved
//)
//{
//    return TRUE;
//}

int ADD(int a, int b)
{
    return a + b;
}

int ADD1(int a, int b)
{
    return a + b;
}

DllTestClass::DllTestClass()
    :mA(0),mB(0),mName("NULL")
{
    std::cout << "一つのDllTestClassを作成" << std::endl;
}

DllTestClass::~DllTestClass()
{
    std::cout << "一つのDllTestClassを削除" << std::endl;
}

void DllTestClass::SetAAndB(int a, int b)
{
    mA = a;
    mB = b;
}

int DllTestClass::GetAPlusB(void)
{
    return mA + mB;
}

char* DllTestClass::GetName(void)
{
    return mName;
}

void DllTestClass::SetName(const char* name)
{
    strcpy_s(mName, sizeof(mName), name);
}
