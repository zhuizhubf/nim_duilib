@echo off
chcp 65001 >nul
setlocal

set "SCRIPT_DIR=%~dp0"
set "FLAG_FILE=%SCRIPT_DIR%duilib_dll.flag"

echo ==============================================
echo  切换到：静态运行库模式（/MT /MTd）
echo ==============================================

if exist "%FLAG_FILE%" (
    del /f /q "%FLAG_FILE%"
    echo 已删除：%FLAG_FILE%
)

echo.
echo 切换完成！
echo 请关闭 VS 后重新打开工程。
echo 本脚本将在 7 秒后自动退出...
echo.

timeout /t 7 >nul
endlocal
