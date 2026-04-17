@echo off
chcp 65001 >nul
echo ==============================================
echo  切换到：静态运行库模式（/MT /MTd）
echo ==============================================

:: 1. 删除 flag 文件，表示使用静态库
if exist "duilib_dll.flag" (
    del /f /q "duilib_dll.flag"
    echo 已删除 duilib_dll.flag
)

echo.
echo "切换完成！"
echo "请关闭 VS 后重新打开工程。"
echo.
pause