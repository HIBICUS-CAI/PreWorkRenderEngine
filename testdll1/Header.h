#pragma once

#ifdef DLL_EXPORTS
#define MYTEST_EXPORT __declspec(dllexport)
#else
#define MYTEST_EXPORT __declspec(dllimport)
#endif // DLL_EXPORTS


MYTEST_EXPORT int ADD(int a, int b);
int ADD1(int a, int b);

class MYTEST_EXPORT DllTestClass
{
public:
    DllTestClass();
    ~DllTestClass();
    char* GetName(void);
    void SetName(const char* name);
    void SetAAndB(int a, int b);
    int GetAPlusB(void);

private:
    int mA;
    int mB;
    char mName[30];
};
