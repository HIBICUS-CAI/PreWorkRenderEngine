//#include <iostream>
//#include "Header.h"
//
//int main(void)
//{
//    std::cout << "���ɷ��" << std::endl;
//    std::cout << "�������Ǥ⡢���ä��{���Ť�����" << std::endl;
//    std::cout << "�Է֤��Ť��Ƥ�������" << std::endl;
//
//    std::cout << "DLL�ե�����ǣ��㤹���νY����Ӌ�㤷�Ʊ�ʾ�����룺��"
//        << ADD(5, 6) << std::endl;
//
//    std::cout << "dll�ե������class��ʹ�ä���" << std::endl;
//    DllTestClass* test = new DllTestClass;
//    std::cout << test->GetAPlusB() << std::endl;
//    test->SetAAndB(10, 10);
//    std::cout << test->GetAPlusB() << std::endl;
//    delete test;
//
//    return 0;
//}

#include <Windows.h>
#include "WindowWIN32.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR szCmdLine,
    _In_ int iCmdShow
)
{
    WindowWIN32* w = new WindowWIN32();
    w->CreateMyWindow("test", "a test window",
        hInstance, iCmdShow, false);

    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {

        }
    }

    return (int)msg.wParam;
}