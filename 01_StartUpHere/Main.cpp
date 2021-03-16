#include <iostream>
#include "Header.h"

int main(void)
{
    std::cout << "大丈夫だ" << std::endl;
    std::cout << "厳しいでも、きっと辿り着けるんだ" << std::endl;
    std::cout << "自分に信じていこう！" << std::endl;

    std::cout << "DLLファイルで５足す６の結果を計算して表示させる：　"
        << ADD(5, 6) << std::endl;

    std::cout << "dllファイルのclassを使用する" << std::endl;
    DllTestClass* test = new DllTestClass;
    std::cout << test->GetAPlusB() << std::endl;
    test->SetAAndB(10, 10);
    std::cout << test->GetAPlusB() << std::endl;
    delete test;

    return 0;
}
