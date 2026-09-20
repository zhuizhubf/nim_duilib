#if defined(__FreeBSD__)

#include "TestApplication.h"
#include "duilib/duilib_config_freebsd.h"

//定义应用程序的入口点
int main(int argc, char **argv)
{
    TestApplication app;
    app.Run();
    return 0;
}

#endif
