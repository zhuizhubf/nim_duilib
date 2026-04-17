@echo off
chcp 65001 >nul
echo ==============================================
echo  切换到：动态运行库模式（/MD /MDd）
echo ==============================================

:: 1. 创建 flag 文件，表示使用 DLL 运行时
echo. > "duilib_dll.flag"
echo 已创建 duilib_dll.flag


echo.
echo "切换完成！"
echo "请关闭 VS 后重新打开工程。"
echo.
pause