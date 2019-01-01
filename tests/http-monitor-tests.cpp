#include "../src/alert-buffer.h"
#include <iostream>

using namespace std;
using namespace HttpMonitorHelper;

// Test helpers
bool assertTrue(bool condition, string funcName)
{
    cout << funcName << (condition ? " passed!" : "failed") << endl;

    return condition;
}
#define AssertTrueRet(c, f) if(!assertTrue(c,f)) { return; }

// Alert Buffer tests
void alertbuffer_basicTest()
{
    int SIZE_OF_BUFFER = 1;
    int ALERTMIN = 10;

    AlertBuffer cb(SIZE_OF_BUFFER, ALERTMIN);

    AlertUnit a = { 0 };
    bool alert = cb.insert(a);
    AssertTrueRet(!alert, __func__);
}

void alertbuffer_largeCorrectness()
{
    int SIZE_OF_BUFFER = 50;
    int ALERTMIN = 10;

    bool passed;
    AlertBuffer cb(SIZE_OF_BUFFER, ALERTMIN);

    AlertUnit a = { 0 };
    for(int i = 0; i < SIZE_OF_BUFFER; i++)
    {
        cb.insert(a);
    }

    for(int i = 0; i < SIZE_OF_BUFFER; i++)
    {
        passed = (cb.insert(a) == false);
    }

    AssertTrueRet(passed, __func__);
}

void alertbuffer_alertBasic()
{
    int SIZE_OF_BUFFER = 1;
    int ALERTMIN = 10;

    AlertBuffer cb(SIZE_OF_BUFFER, ALERTMIN);
    AlertUnit a = { 10 };

    bool alert = cb.insert(a);
    AssertTrueRet(alert, __func__);
}

void alertbuffer_alertLarge()
{
    int SIZE_OF_BUFFER = 10;
    int ALERTMIN = 10;

    AlertBuffer cb(SIZE_OF_BUFFER, ALERTMIN);
    AlertUnit a = { 1 };

    for(int i = 0; i < SIZE_OF_BUFFER - 1; i++)
    {
        cb.insert(a);
    }

    bool alert = cb.insert(a);
    AssertTrueRet(alert, __func__);
}

// Main test driver
// With more time I'd want to make this more customizable,
// Run multiple times, run only certain tests, etc.
// But for now it runs everything

int main()
{
    alertbuffer_basicTest();
    alertbuffer_largeCorrectness();
    alertbuffer_alertBasic();
    alertbuffer_alertLarge();
}