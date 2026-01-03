#!/bin/bash

# Installation script for Music Player
# This script helps install dependencies for the music player

echo "=================================="
echo "音樂播放器安裝腳本"
echo "Music Player Installation Script"
echo "=================================="
echo ""

# Check Python version
echo "檢查 Python 版本..."
echo "Checking Python version..."
python3 --version

if [ $? -ne 0 ]; then
    echo "錯誤: 未找到 Python 3"
    echo "Error: Python 3 not found"
    echo "請安裝 Python 3.6 或更高版本"
    echo "Please install Python 3.6 or higher"
    exit 1
fi

echo ""
echo "安裝依賴套件..."
echo "Installing dependencies..."
echo ""

# Install requirements
pip3 install -r requirements.txt

if [ $? -eq 0 ]; then
    echo ""
    echo "=================================="
    echo "✓ 安裝成功！"
    echo "✓ Installation successful!"
    echo "=================================="
    echo ""
    echo "執行播放器: python3 music_player.py"
    echo "Run player: python3 music_player.py"
    echo ""
    echo "測試安裝: python3 test_music_player.py"
    echo "Test installation: python3 test_music_player.py"
    echo ""
else
    echo ""
    echo "=================================="
    echo "✗ 安裝失敗"
    echo "✗ Installation failed"
    echo "=================================="
    echo ""
    echo "請手動安裝依賴:"
    echo "Please install dependencies manually:"
    echo "  pip3 install PyQt5"
    echo "  pip3 install mutagen"
    echo ""
    exit 1
fi
