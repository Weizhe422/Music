# 快速開始指南

## 一分鐘開始使用

### Linux/macOS

```bash
# 1. 安裝依賴
# Ubuntu/Debian:
sudo apt-get install qt6-base-dev qt6-multimedia-dev libtag1-dev cmake

# macOS:
brew install qt6 taglib cmake

# 2. 編譯
./build.sh

# 3. 運行
./build/MusicPlayer
```

### Windows

```batch
REM 1. 確保已安裝 Qt6 和 TagLib
REM 2. 在 Qt 命令提示符中運行:
build.bat

REM 3. 運行
build\MusicPlayer.exe
```

## 基本使用

### 1. 添加音樂
- 點擊 "➕ 添加歌曲" 按鈕
- 選擇音樂檔案 (支援 MP3, FLAC, WAV, AAC, M4A, OGG)

### 2. 播放音樂
- 雙擊播放清單中的歌曲
- 或選擇歌曲後點擊 "▶ 播放"

### 3. 控制播放
- **⏮ 上一首**: 播放上一首歌曲
- **▶ 播放**: 開始播放
- **⏸ 暫停**: 暫停播放
- **⏹ 停止**: 停止播放
- **⏭ 下一首**: 播放下一首歌曲

### 4. 歌詞顯示
- 自動: 將 .lrc 檔案放在音樂檔案旁邊 (同名)
- 手動: 點擊 "📝 載入字幕" 選擇 LRC 檔案

### 5. 播放清單管理
- **新建**: 選單 → 播放清單 → 新建播放清單
- **切換**: 選單 → 播放清單 → 切換播放清單
- **保存**: 選單 → 播放清單 → 保存播放清單
- **載入**: 選單 → 播放清單 → 載入播放清單

## 檔案結構

```
Music/
├── main.cpp              # 主程式入口
├── musicplayer.h         # 播放器標頭檔
├── musicplayer.cpp       # 播放器實現
├── CMakeLists.txt        # CMake 配置
├── MusicPlayer.pro       # qmake 配置
├── build.sh              # Linux/macOS 編譯腳本
├── build.bat             # Windows 編譯腳本
├── README_CPP.md         # 詳細說明
└── sample_lyrics.lrc     # 範例歌詞檔案
```

## 常見問題

### Q: 編譯失敗，提示找不到 Qt
**A**: 確保已安裝 Qt6 並設定環境變數：
```bash
export Qt6_DIR=/path/to/qt6/lib/cmake/Qt6
```

### Q: 無法播放音頻
**A**: 安裝音頻編解碼器：
```bash
# Linux:
sudo apt-get install gstreamer1.0-plugins-good gstreamer1.0-plugins-bad
```

### Q: 封面無法顯示
**A**: 確保音頻檔案包含嵌入式封面，目前支援 MP3 和 FLAC 格式

### Q: 歌詞不同步
**A**: 確認 LRC 檔案格式正確且編碼為 UTF-8

## 更多資訊

- **完整文檔**: [README_CPP.md](README_CPP.md)
- **實現細節**: [CPP_IMPLEMENTATION.md](CPP_IMPLEMENTATION.md)
- **版本對比**: [COMPARISON.md](COMPARISON.md)
- **專案總結**: [PROJECT_SUMMARY.md](PROJECT_SUMMARY.md)

## 需要幫助？

如有問題，請在 GitHub 上開啟 Issue。
