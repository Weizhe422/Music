@echo off
REM Qt C++ 音樂播放器編譯腳本 (Windows)

echo =========================================
echo Qt C++ 音樂播放器編譯腳本 (Windows)
echo =========================================
echo.

REM 檢查 CMake
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo 錯誤: CMake 未安裝
    echo 請從 https://cmake.org/download/ 下載並安裝 CMake
    pause
    exit /b 1
)

echo ✅ CMake 已安裝

REM 檢查是否在 Qt 環境中
where qmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo.
    echo 警告: 未找到 qmake
    echo 請確保您已經：
    echo 1. 安裝了 Qt6
    echo 2. 在 Qt 命令提示符中運行此腳本
    echo    或已將 Qt bin 目錄添加到 PATH
    echo.
    echo 例如：
    echo   C:\Qt\6.x.x\msvc2019_64\bin
    echo.
    pause
    exit /b 1
)

echo ✅ Qt 環境已配置
echo.

REM 檢查 TagLib
echo 注意: 確保 TagLib 已安裝並配置
echo   下載地址: https://taglib.org/
echo   或使用 vcpkg: vcpkg install taglib
echo.

REM 創建 build 目錄
if exist build (
    echo 清理舊的 build 目錄...
    rmdir /s /q build
)

mkdir build
cd build

echo.
echo 執行 CMake 配置...
cmake .. -G "NMake Makefiles"

if %ERRORLEVEL% neq 0 (
    echo.
    echo 配置失敗！
    echo 請檢查：
    echo 1. Qt6 是否正確安裝
    echo 2. TagLib 是否正確安裝
    echo 3. CMakeLists.txt 中的路徑是否正確
    pause
    exit /b 1
)

echo.
echo 執行編譯...
cmake --build .

if %ERRORLEVEL% neq 0 (
    echo.
    echo 編譯失敗！
    pause
    exit /b 1
)

echo.
echo =========================================
echo ✅ 編譯成功！
echo.
echo 執行程式：
echo   cd build
echo   MusicPlayer.exe
echo.
echo 或者直接執行：
echo   build\MusicPlayer.exe
echo =========================================
echo.
pause
