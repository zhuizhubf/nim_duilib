#include "TestApplication.h"

#include "MainThread.h"

TestApplication::TestApplication() = default;
TestApplication::~TestApplication() = default;

void TestApplication::Run()
{
    MainThread thread;
    thread.RunMessageLoop();
}
