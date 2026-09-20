#if defined(linux) || defined(__linux) || defined(__linux__)

#include "TestApplication.h"
#include "duilib/duilib_config_linux.h"

//定义应用程序的入口点
int main(int argc, char **argv)
{
    TestApplication app;
    app.Run();
    return 0;
}

#endif
