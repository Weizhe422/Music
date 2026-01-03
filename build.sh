#!/bin/bash

# Qt C++ 音樂播放器編譯腳本
# 此腳本會檢查依賴並編譯專案

set -e

echo "========================================="
echo "Qt C++ 音樂播放器編譯腳本"
echo "========================================="
echo ""

# 檢查系統
if [ -f /etc/os-release ]; then
    . /etc/os-release
    echo "檢測到的作業系統: $NAME"
    echo ""
fi

# 檢查依賴
echo "正在檢查依賴..."
echo ""

# 檢查 CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ 錯誤: CMake 未安裝"
    echo "   請安裝 CMake: sudo apt-get install cmake"
    exit 1
else
    CMAKE_VERSION=$(cmake --version | head -1)
    echo "✅ CMake: $CMAKE_VERSION"
fi

# 檢查 g++
if ! command -v g++ &> /dev/null; then
    echo "❌ 錯誤: g++ 未安裝"
    echo "   請安裝 g++: sudo apt-get install build-essential"
    exit 1
else
    GCC_VERSION=$(g++ --version | head -1)
    echo "✅ g++: $GCC_VERSION"
fi

# 檢查 pkg-config
if ! command -v pkg-config &> /dev/null; then
    echo "❌ 錯誤: pkg-config 未安裝"
    echo "   請安裝 pkg-config: sudo apt-get install pkg-config"
    exit 1
else
    echo "✅ pkg-config: $(pkg-config --version)"
fi

echo ""
echo "正在檢查 Qt6..."

# 檢查 Qt6
QT6_FOUND=0
if pkg-config --exists Qt6Core 2>/dev/null; then
    QT6_VERSION=$(pkg-config --modversion Qt6Core)
    echo "✅ Qt6 Core: $QT6_VERSION"
    QT6_FOUND=1
else
    echo "❌ Qt6 未找到"
    echo "   Ubuntu/Debian: sudo apt-get install qt6-base-dev qt6-multimedia-dev"
    echo "   macOS: brew install qt6"
fi

if pkg-config --exists Qt6Widgets 2>/dev/null; then
    echo "✅ Qt6 Widgets: $(pkg-config --modversion Qt6Widgets)"
else
    if [ $QT6_FOUND -eq 1 ]; then
        echo "❌ Qt6 Widgets 未找到"
    fi
fi

if pkg-config --exists Qt6Multimedia 2>/dev/null; then
    echo "✅ Qt6 Multimedia: $(pkg-config --modversion Qt6Multimedia)"
else
    if [ $QT6_FOUND -eq 1 ]; then
        echo "❌ Qt6 Multimedia 未找到"
        echo "   請安裝: sudo apt-get install qt6-multimedia-dev"
    fi
fi

echo ""
echo "正在檢查 TagLib..."

# 檢查 TagLib
if pkg-config --exists taglib 2>/dev/null; then
    TAGLIB_VERSION=$(pkg-config --modversion taglib)
    echo "✅ TagLib: $TAGLIB_VERSION"
else
    echo "❌ TagLib 未找到"
    echo "   Ubuntu/Debian: sudo apt-get install libtag1-dev"
    echo "   macOS: brew install taglib"
    exit 1
fi

echo ""
echo "========================================="

if [ $QT6_FOUND -eq 0 ]; then
    echo "❌ 無法編譯: Qt6 未安裝"
    echo ""
    echo "請先安裝所需的依賴套件："
    echo ""
    echo "Ubuntu/Debian:"
    echo "  sudo apt-get update"
    echo "  sudo apt-get install qt6-base-dev qt6-multimedia-dev libtag1-dev cmake build-essential"
    echo ""
    echo "macOS (使用 Homebrew):"
    echo "  brew install qt6 taglib cmake"
    echo ""
    exit 1
fi

echo "✅ 所有依賴檢查通過！"
echo ""

# 開始編譯
echo "開始編譯..."
echo ""

# 創建 build 目錄
if [ -d "build" ]; then
    echo "清理舊的 build 目錄..."
    rm -rf build
fi

mkdir build
cd build

echo "執行 CMake 配置..."
cmake ..

echo ""
echo "執行編譯..."
cmake --build . -j$(nproc)

echo ""
echo "========================================="
echo "✅ 編譯成功！"
echo ""
echo "執行程式："
echo "  cd build"
echo "  ./MusicPlayer"
echo ""
echo "或者直接執行："
echo "  ./build/MusicPlayer"
echo "========================================="
