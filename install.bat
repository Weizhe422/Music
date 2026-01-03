@echo off
REM Installation script for Music Player (Windows)

echo ==================================
echo 音樂播放器安裝腳本
echo Music Player Installation Script
echo ==================================
echo.

REM Check Python version
echo 檢查 Python 版本...
echo Checking Python version...
python --version

if errorlevel 1 (
    echo 錯誤: 未找到 Python
    echo Error: Python not found
    echo 請安裝 Python 3.6 或更高版本
    echo Please install Python 3.6 or higher
    pause
    exit /b 1
)

echo.
echo 安裝依賴套件...
echo Installing dependencies...
echo.

REM Install requirements
python -m pip install -r requirements.txt

if errorlevel 1 (
    echo.
    echo ==================================
    echo ✗ 安裝失敗
    echo ✗ Installation failed
    echo ==================================
    echo.
    echo 請手動安裝依賴:
    echo Please install dependencies manually:
    echo   pip install PyQt5
    echo   pip install mutagen
    echo.
    pause
    exit /b 1
)

echo.
echo ==================================
echo ✓ 安裝成功！
echo ✓ Installation successful!
echo ==================================
echo.
echo 執行播放器: python music_player.py
echo Run player: python music_player.py
echo.
echo 測試安裝: python test_music_player.py
echo Test installation: python test_music_player.py
echo.
pause
