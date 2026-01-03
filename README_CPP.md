# Qt C++ 音樂播放器編譯指南

本專案提供了 Qt C++ 版本的音樂播放器實現，基於原有的 Python/PyQt5 版本。

## 系統要求

### 依賴庫
- **Qt 6.0 或更高版本** (推薦) 或 **Qt 5.15+**
  - Qt Core
  - Qt Widgets
  - Qt Multimedia
- **TagLib** - 用於讀取音頻元數據和封面
- **CMake 3.16+** 或 **qmake** (隨 Qt 提供)

### Linux (Ubuntu/Debian)

```bash
# 安裝 Qt 6 開發工具
sudo apt-get install qt6-base-dev qt6-multimedia-dev

# 安裝 TagLib
sudo apt-get install libtag1-dev

# 安裝 CMake (如果使用 CMake 編譯)
sudo apt-get install cmake
```

### macOS

```bash
# 使用 Homebrew 安裝依賴
brew install qt6
brew install taglib
brew install cmake
```

### Windows

1. 從 [Qt 官網](https://www.qt.io/download) 下載並安裝 Qt
2. 從 [TagLib 官網](https://taglib.org/) 下載並安裝 TagLib
3. 確保 CMake 已安裝或使用 Qt Creator IDE

## 編譯方法

### 方法 1: 使用 CMake

```bash
# 創建 build 目錄
mkdir build
cd build

# 配置專案
cmake ..

# 編譯
cmake --build .

# 運行
./MusicPlayer
```

### 方法 2: 使用 qmake

```bash
# 生成 Makefile
qmake MusicPlayer.pro

# 編譯
make

# 運行
./MusicPlayer
```

### 方法 3: 使用 Qt Creator

1. 打開 Qt Creator
2. 選擇 "開啟專案"
3. 選擇 `CMakeLists.txt` 或 `MusicPlayer.pro`
4. 配置編譯選項
5. 點擊 "編譯" 和 "運行"

## 支援的音頻格式

- MP3
- WAV
- FLAC
- AAC
- M4A
- OGG

## 功能特點

### 核心功能
- ✅ 播放/暫停/停止控制
- ✅ 上一首/下一首切換
- ✅ 播放進度顯示和控制
- ✅ 音量控制
- ✅ 播放清單管理
- ✅ 歌曲順序調整
- ✅ 封面顯示 (使用 TagLib)
- ✅ LRC 字幕同步顯示
- ✅ 多播放清單支援
- ✅ 播放清單保存/載入 (JSON 格式)

### 字幕功能
- 支援 LRC 格式字幕檔案
- 自動載入同名 .lrc 檔案
- 字幕隨音樂播放高亮顯示
- 點擊字幕行可跳轉到對應時間

### 播放清單管理
- 支援創建多個播放清單
- 播放清單之間切換
- 保存和載入播放清單配置

## 與 Python 版本的差異

### 技術架構
- **Python 版本**: 使用 PyQt5 和 mutagen
- **C++ 版本**: 使用 Qt6 和 TagLib

### 主要改進
1. **性能**: C++ 版本運行更快，資源佔用更少
2. **部署**: 可編譯為獨立執行檔，不需要 Python 環境
3. **原生體驗**: 更好的系統整合和原生外觀

## 專案結構

```
Music/
├── main.cpp              # 程式入口
├── musicplayer.h         # MusicPlayer 類別宣告
├── musicplayer.cpp       # MusicPlayer 類別實現
├── CMakeLists.txt        # CMake 編譯配置
├── MusicPlayer.pro       # qmake 編譯配置
├── README_CPP.md         # C++ 版本說明文件 (本文件)
├── sample_lyrics.lrc     # 範例字幕檔案
└── music_player.py       # 原始 Python 版本
```

## 使用說明

程式啟動後，使用方式與 Python 版本相同：

1. **添加歌曲**: 點擊 "➕ 添加歌曲" 或使用選單 "檔案 → 添加音樂檔案"
2. **播放控制**: 使用介面上的播放/暫停/停止按鈕
3. **調整順序**: 選擇歌曲後使用 "⬆ 上移" 或 "⬇ 下移" 按鈕
4. **載入字幕**: 點擊 "📝 載入字幕" 或將同名 .lrc 檔案放在音樂檔案旁邊
5. **播放清單管理**: 使用選單中的 "播放清單" 選項

## 故障排除

### 編譯錯誤

**找不到 Qt 庫**:
```bash
# 設定 Qt 環境變數
export Qt6_DIR=/path/to/qt6/lib/cmake/Qt6
```

**找不到 TagLib**:
```bash
# 確保 TagLib 已安裝並且可以被 pkg-config 找到
pkg-config --cflags --libs taglib
```

### 運行時錯誤

**無法播放音頻**:
- 確保系統已安裝必要的音頻編解碼器
- Linux: 安裝 `gstreamer1.0-plugins-good` 和 `gstreamer1.0-plugins-bad`

**封面無法顯示**:
- 確保音頻檔案包含嵌入式封面圖片
- 某些格式可能不支援封面顯示

## 授權

本專案使用 MIT 授權。

## 參考資料

- [Qt Documentation](https://doc.qt.io/)
- [TagLib Documentation](https://taglib.org/)
- [原始 Python 版本](README.md)
