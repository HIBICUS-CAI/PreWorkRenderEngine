#include <iostream>
#include "Header.h"

int main(void)
{
    std::cout << "���ɷ��" << std::endl;
    std::cout << "�������Ǥ⡢���ä��{���Ť�����" << std::endl;
    std::cout << "�Է֤��Ť��Ƥ�������" << std::endl;

    std::cout << "DLL�ե�����ǣ��㤹���νY����Ӌ�㤷�Ʊ�ʾ�����룺��"
        << ADD(5, 6) << std::endl;

    std::cout << "dll�ե������class��ʹ�ä���" << std::endl;
    DllTestClass* test = new DllTestClass;
    std::cout << test->GetAPlusB() << std::endl;
    test->SetAAndB(10, 10);
    std::cout << test->GetAPlusB() << std::endl;
    delete test;

    return 0;
}
