import sys
import os
import json
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QPushButton, QSlider, QLabel, QListWidget, 
                             QFileDialog, QMessageBox, QListWidgetItem, QInputDialog,
                             QMenuBar, QMenu, QAction, QSplitter)
from PyQt5.QtCore import Qt, QUrl
from PyQt5.QtGui import QPixmap, QImage
from PyQt5.QtMultimedia import QMediaPlayer, QMediaContent, QMediaPlaylist
from mutagen import File as MutagenFile
from mutagen.id3 import ID3, APIC
from mutagen.mp3 import MP3
from mutagen.flac import FLAC


class MusicPlayer(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("音樂播放器")
        self.setGeometry(100, 100, 1000, 700)
        
        # Initialize media player and playlist
        self.player = QMediaPlayer()
        self.playlist = QMediaPlaylist()
        self.player.setPlaylist(self.playlist)
        
        # Store song information
        self.song_list = []  # List of file paths
        self.lyrics_data = {}  # Dictionary to store lyrics with timestamps
        self.current_lyrics_file = None
        
        # Playlists management
        self.playlists = {"默認播放清單": []}
        self.current_playlist_name = "默認播放清單"
        
        self.init_ui()
        self.connect_signals()
        
    def init_ui(self):
        # Central widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        
        # Menu bar
        self.create_menu_bar()
        
        # Create splitter for main content
        splitter = QSplitter(Qt.Horizontal)
        
        # Left panel - Playlist and controls
        left_panel = QWidget()
        left_layout = QVBoxLayout(left_panel)
        
        # Album cover display
        self.cover_label = QLabel()
        self.cover_label.setFixedSize(300, 300)
        self.cover_label.setScaledContents(True)
        self.cover_label.setStyleSheet("border: 1px solid #ccc; background-color: #f0f0f0;")
        self.cover_label.setText("無封面")
        self.cover_label.setAlignment(Qt.AlignCenter)
        left_layout.addWidget(self.cover_label)
        
        # Current song info
        self.song_info_label = QLabel("未選擇歌曲")
        self.song_info_label.setAlignment(Qt.AlignCenter)
        self.song_info_label.setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;")
        left_layout.addWidget(self.song_info_label)
        
        # Progress bar
        progress_layout = QHBoxLayout()
        self.current_time_label = QLabel("00:00")
        self.progress_slider = QSlider(Qt.Horizontal)
        self.progress_slider.setRange(0, 0)
        self.total_time_label = QLabel("00:00")
        progress_layout.addWidget(self.current_time_label)
        progress_layout.addWidget(self.progress_slider)
        progress_layout.addWidget(self.total_time_label)
        left_layout.addLayout(progress_layout)
        
        # Control buttons
        control_layout = QHBoxLayout()
        self.prev_button = QPushButton("⏮ 上一首")
        self.play_button = QPushButton("▶ 播放")
        self.pause_button = QPushButton("⏸ 暫停")
        self.stop_button = QPushButton("⏹ 停止")
        self.next_button = QPushButton("⏭ 下一首")
        
        control_layout.addWidget(self.prev_button)
        control_layout.addWidget(self.play_button)
        control_layout.addWidget(self.pause_button)
        control_layout.addWidget(self.stop_button)
        control_layout.addWidget(self.next_button)
        left_layout.addLayout(control_layout)
        
        # Volume control
        volume_layout = QHBoxLayout()
        volume_label = QLabel("音量:")
        self.volume_slider = QSlider(Qt.Horizontal)
        self.volume_slider.setRange(0, 100)
        self.volume_slider.setValue(50)
        self.volume_label_value = QLabel("50%")
        volume_layout.addWidget(volume_label)
        volume_layout.addWidget(self.volume_slider)
        volume_layout.addWidget(self.volume_label_value)
        left_layout.addLayout(volume_layout)
        
        # Playlist
        playlist_label = QLabel("播放清單:")
        playlist_label.setStyleSheet("font-weight: bold;")
        left_layout.addWidget(playlist_label)
        
        self.playlist_widget = QListWidget()
        left_layout.addWidget(self.playlist_widget)
        
        # Playlist control buttons
        playlist_control_layout = QHBoxLayout()
        self.add_song_button = QPushButton("➕ 添加歌曲")
        self.remove_song_button = QPushButton("➖ 刪除歌曲")
        self.move_up_button = QPushButton("⬆ 上移")
        self.move_down_button = QPushButton("⬇ 下移")
        self.load_lyrics_button = QPushButton("📝 載入字幕")
        
        playlist_control_layout.addWidget(self.add_song_button)
        playlist_control_layout.addWidget(self.remove_song_button)
        playlist_control_layout.addWidget(self.move_up_button)
        playlist_control_layout.addWidget(self.move_down_button)
        left_layout.addLayout(playlist_control_layout)
        left_layout.addWidget(self.load_lyrics_button)
        
        # Right panel - Lyrics
        right_panel = QWidget()
        right_layout = QVBoxLayout(right_panel)
        
        lyrics_label = QLabel("歌詞:")
        lyrics_label.setStyleSheet("font-weight: bold;")
        right_layout.addWidget(lyrics_label)
        
        self.lyrics_widget = QListWidget()
        right_layout.addWidget(self.lyrics_widget)
        
        # Add panels to splitter
        splitter.addWidget(left_panel)
        splitter.addWidget(right_panel)
        splitter.setStretchFactor(0, 2)
        splitter.setStretchFactor(1, 1)
        
        main_layout.addWidget(splitter)
        
    def create_menu_bar(self):
        menubar = self.menuBar()
        
        # File menu
        file_menu = menubar.addMenu("檔案")
        
        add_files_action = QAction("添加音樂檔案", self)
        add_files_action.triggered.connect(self.add_songs)
        file_menu.addAction(add_files_action)
        
        exit_action = QAction("退出", self)
        exit_action.triggered.connect(self.close)
        file_menu.addAction(exit_action)
        
        # Playlist menu
        playlist_menu = menubar.addMenu("播放清單")
        
        new_playlist_action = QAction("新建播放清單", self)
        new_playlist_action.triggered.connect(self.create_new_playlist)
        playlist_menu.addAction(new_playlist_action)
        
        switch_playlist_action = QAction("切換播放清單", self)
        switch_playlist_action.triggered.connect(self.switch_playlist)
        playlist_menu.addAction(switch_playlist_action)
        
        save_playlist_action = QAction("保存播放清單", self)
        save_playlist_action.triggered.connect(self.save_playlists)
        playlist_menu.addAction(save_playlist_action)
        
        load_playlist_action = QAction("載入播放清單", self)
        load_playlist_action.triggered.connect(self.load_playlists)
        playlist_menu.addAction(load_playlist_action)
        
    def connect_signals(self):
        # Player signals
        self.player.positionChanged.connect(self.update_position)
        self.player.durationChanged.connect(self.update_duration)
        self.player.currentMediaChanged.connect(self.on_song_changed)
        
        # Control buttons
        self.play_button.clicked.connect(self.play)
        self.pause_button.clicked.connect(self.pause)
        self.stop_button.clicked.connect(self.stop)
        self.prev_button.clicked.connect(self.previous_song)
        self.next_button.clicked.connect(self.next_song)
        
        # Playlist buttons
        self.add_song_button.clicked.connect(self.add_songs)
        self.remove_song_button.clicked.connect(self.remove_song)
        self.move_up_button.clicked.connect(self.move_song_up)
        self.move_down_button.clicked.connect(self.move_song_down)
        self.load_lyrics_button.clicked.connect(self.load_lyrics)
        
        # Sliders
        self.progress_slider.sliderMoved.connect(self.set_position)
        self.volume_slider.valueChanged.connect(self.set_volume)
        
        # Playlist widget
        self.playlist_widget.itemDoubleClicked.connect(self.play_selected_song)
        
        # Lyrics widget
        self.lyrics_widget.itemClicked.connect(self.jump_to_lyrics_time)
        
    def add_songs(self):
        files, _ = QFileDialog.getOpenFileNames(
            self, 
            "選擇音樂檔案", 
            "", 
            "音頻檔案 (*.mp3 *.wav *.flac *.aac *.m4a *.ogg);;所有檔案 (*.*)"
        )
        
        if files:
            for file_path in files:
                self.song_list.append(file_path)
                self.playlist.addMedia(QMediaContent(QUrl.fromLocalFile(file_path)))
                
                # Add to playlist widget
                song_name = os.path.basename(file_path)
                self.playlist_widget.addItem(song_name)
                
                # Save to current playlist
                self.playlists[self.current_playlist_name].append(file_path)
    
    def remove_song(self):
        current_row = self.playlist_widget.currentRow()
        if current_row >= 0:
            self.playlist_widget.takeItem(current_row)
            self.playlist.removeMedia(current_row)
            del self.song_list[current_row]
            del self.playlists[self.current_playlist_name][current_row]
    
    def move_song_up(self):
        current_row = self.playlist_widget.currentRow()
        if current_row > 0:
            # Move in list widget
            item = self.playlist_widget.takeItem(current_row)
            self.playlist_widget.insertItem(current_row - 1, item)
            self.playlist_widget.setCurrentRow(current_row - 1)
            
            # Move in song list
            self.song_list[current_row], self.song_list[current_row - 1] = \
                self.song_list[current_row - 1], self.song_list[current_row]
            
            # Move in playlists
            pl = self.playlists[self.current_playlist_name]
            pl[current_row], pl[current_row - 1] = pl[current_row - 1], pl[current_row]
            
            # Rebuild media playlist
            self.rebuild_media_playlist()
    
    def move_song_down(self):
        current_row = self.playlist_widget.currentRow()
        if current_row >= 0 and current_row < self.playlist_widget.count() - 1:
            # Move in list widget
            item = self.playlist_widget.takeItem(current_row)
            self.playlist_widget.insertItem(current_row + 1, item)
            self.playlist_widget.setCurrentRow(current_row + 1)
            
            # Move in song list
            self.song_list[current_row], self.song_list[current_row + 1] = \
                self.song_list[current_row + 1], self.song_list[current_row]
            
            # Move in playlists
            pl = self.playlists[self.current_playlist_name]
            pl[current_row], pl[current_row + 1] = pl[current_row + 1], pl[current_row]
            
            # Rebuild media playlist
            self.rebuild_media_playlist()
    
    def rebuild_media_playlist(self):
        # Clear and rebuild the QMediaPlaylist
        current_index = self.playlist.currentIndex()
        self.playlist.clear()
        for file_path in self.song_list:
            self.playlist.addMedia(QMediaContent(QUrl.fromLocalFile(file_path)))
        if current_index >= 0 and current_index < len(self.song_list):
            self.playlist.setCurrentIndex(current_index)
    
    def play(self):
        if self.player.state() == QMediaPlayer.StoppedState and self.playlist.mediaCount() > 0:
            self.playlist.setCurrentIndex(0)
        self.player.play()
    
    def pause(self):
        self.player.pause()
    
    def stop(self):
        self.player.stop()
    
    def previous_song(self):
        self.playlist.previous()
    
    def next_song(self):
        self.playlist.next()
    
    def play_selected_song(self, item):
        index = self.playlist_widget.row(item)
        self.playlist.setCurrentIndex(index)
        self.player.play()
    
    def update_position(self, position):
        self.progress_slider.setValue(position)
        self.current_time_label.setText(self.format_time(position))
        
        # Update lyrics highlight
        self.update_lyrics_highlight(position)
    
    def update_duration(self, duration):
        self.progress_slider.setRange(0, duration)
        self.total_time_label.setText(self.format_time(duration))
    
    def set_position(self, position):
        self.player.setPosition(position)
    
    def set_volume(self, value):
        self.player.setVolume(value)
        self.volume_label_value.setText(f"{value}%")
    
    def format_time(self, ms):
        s = ms // 1000
        m = s // 60
        s = s % 60
        return f"{m:02d}:{s:02d}"
    
    def on_song_changed(self, media):
        if media.isNull():
            return
        
        current_index = self.playlist.currentIndex()
        if current_index >= 0 and current_index < len(self.song_list):
            file_path = self.song_list[current_index]
            song_name = os.path.basename(file_path)
            self.song_info_label.setText(song_name)
            
            # Highlight current song in playlist
            self.playlist_widget.setCurrentRow(current_index)
            
            # Load and display album cover
            self.load_album_cover(file_path)
            
            # Load lyrics if available
            self.load_lyrics_for_current_song(file_path)
    
    def load_album_cover(self, file_path):
        try:
            audio_file = MutagenFile(file_path)
            
            cover_data = None
            
            # Try to extract cover for different formats
            if isinstance(audio_file, MP3) or hasattr(audio_file, 'tags'):
                tags = audio_file.tags
                if tags:
                    for tag in tags.values():
                        if isinstance(tag, APIC):
                            cover_data = tag.data
                            break
            elif isinstance(audio_file, FLAC):
                if audio_file.pictures:
                    cover_data = audio_file.pictures[0].data
            
            if cover_data:
                # Convert to QPixmap
                image = QImage()
                image.loadFromData(cover_data)
                pixmap = QPixmap.fromImage(image)
                self.cover_label.setPixmap(pixmap)
            else:
                self.cover_label.setText("無封面")
                self.cover_label.setPixmap(QPixmap())
        except Exception as e:
            self.cover_label.setText("無法載入封面")
            self.cover_label.setPixmap(QPixmap())
    
    def load_lyrics(self):
        file_path, _ = QFileDialog.getOpenFileName(
            self, 
            "選擇字幕檔案", 
            "", 
            "LRC字幕檔 (*.lrc);;文字檔 (*.txt);;所有檔案 (*.*)"
        )
        
        if file_path:
            self.parse_lyrics_file(file_path)
    
    def load_lyrics_for_current_song(self, song_path):
        # Try to find lyrics file with same name
        base_path = os.path.splitext(song_path)[0]
        lrc_path = base_path + ".lrc"
        
        if os.path.exists(lrc_path):
            self.parse_lyrics_file(lrc_path)
        else:
            self.lyrics_widget.clear()
            self.lyrics_data = {}
    
    def parse_lyrics_file(self, file_path):
        self.lyrics_widget.clear()
        self.lyrics_data = {}
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                lines = f.readlines()
            
            for line in lines:
                line = line.strip()
                if not line:
                    continue
                
                # Parse LRC format: [mm:ss.xx]lyrics
                if line.startswith('['):
                    try:
                        # Extract timestamp
                        end_bracket = line.index(']')
                        timestamp_str = line[1:end_bracket]
                        lyrics_text = line[end_bracket + 1:].strip()
                        
                        # Parse timestamp
                        if ':' in timestamp_str:
                            parts = timestamp_str.split(':')
                            minutes = int(parts[0])
                            seconds_parts = parts[1].split('.')
                            seconds = int(seconds_parts[0])
                            milliseconds = int(seconds_parts[1]) * 10 if len(seconds_parts) > 1 else 0
                            
                            total_ms = (minutes * 60 + seconds) * 1000 + milliseconds
                            
                            # Store lyrics with timestamp
                            self.lyrics_data[total_ms] = lyrics_text
                            
                            # Add to list widget
                            item = QListWidgetItem(lyrics_text)
                            item.setData(Qt.UserRole, total_ms)
                            self.lyrics_widget.addItem(item)
                    except (ValueError, IndexError):
                        # If parsing fails, just add the line as is
                        self.lyrics_widget.addItem(line)
                else:
                    # Non-timestamp line
                    self.lyrics_widget.addItem(line)
            
            self.current_lyrics_file = file_path
            
        except Exception as e:
            QMessageBox.warning(self, "錯誤", f"無法載入字幕檔案: {str(e)}")
    
    def update_lyrics_highlight(self, position):
        if not self.lyrics_data:
            return
        
        # Find the current lyrics line
        current_item = None
        for i in range(self.lyrics_widget.count()):
            item = self.lyrics_widget.item(i)
            timestamp = item.data(Qt.UserRole)
            
            if timestamp and timestamp <= position:
                current_item = item
            elif timestamp and timestamp > position:
                break
        
        # Highlight current line
        for i in range(self.lyrics_widget.count()):
            item = self.lyrics_widget.item(i)
            if item == current_item:
                item.setBackground(Qt.yellow)
                # Auto-scroll to current line
                self.lyrics_widget.scrollToItem(item)
            else:
                item.setBackground(Qt.white)
    
    def jump_to_lyrics_time(self, item):
        timestamp = item.data(Qt.UserRole)
        if timestamp is not None:
            self.player.setPosition(timestamp)
    
    def create_new_playlist(self):
        name, ok = QInputDialog.getText(self, "新建播放清單", "輸入播放清單名稱:")
        if ok and name:
            if name not in self.playlists:
                self.playlists[name] = []
                QMessageBox.information(self, "成功", f"播放清單 '{name}' 已創建")
            else:
                QMessageBox.warning(self, "錯誤", "播放清單名稱已存在")
    
    def switch_playlist(self):
        if not self.playlists:
            QMessageBox.warning(self, "錯誤", "沒有可用的播放清單")
            return
        
        names = list(self.playlists.keys())
        name, ok = QInputDialog.getItem(
            self, "切換播放清單", "選擇播放清單:", names, 0, False
        )
        
        if ok and name:
            self.current_playlist_name = name
            self.load_playlist_songs(name)
    
    def load_playlist_songs(self, name):
        # Clear current playlist
        self.playlist_widget.clear()
        self.playlist.clear()
        self.song_list = []
        
        # Load songs from selected playlist
        for file_path in self.playlists[name]:
            if os.path.exists(file_path):
                self.song_list.append(file_path)
                self.playlist.addMedia(QMediaContent(QUrl.fromLocalFile(file_path)))
                song_name = os.path.basename(file_path)
                self.playlist_widget.addItem(song_name)
        
        self.setWindowTitle(f"音樂播放器 - {name}")
    
    def save_playlists(self):
        file_path, _ = QFileDialog.getSaveFileName(
            self, "保存播放清單", "", "JSON檔案 (*.json)"
        )
        
        if file_path:
            try:
                with open(file_path, 'w', encoding='utf-8') as f:
                    json.dump(self.playlists, f, ensure_ascii=False, indent=2)
                QMessageBox.information(self, "成功", "播放清單已保存")
            except Exception as e:
                QMessageBox.warning(self, "錯誤", f"無法保存播放清單: {str(e)}")
    
    def load_playlists(self):
        file_path, _ = QFileDialog.getOpenFileName(
            self, "載入播放清單", "", "JSON檔案 (*.json)"
        )
        
        if file_path:
            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    self.playlists = json.load(f)
                
                if self.playlists:
                    # Switch to first playlist
                    first_name = list(self.playlists.keys())[0]
                    self.current_playlist_name = first_name
                    self.load_playlist_songs(first_name)
                    
                QMessageBox.information(self, "成功", "播放清單已載入")
            except Exception as e:
                QMessageBox.warning(self, "錯誤", f"無法載入播放清單: {str(e)}")


def main():
    app = QApplication(sys.argv)
    player = MusicPlayer()
    player.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
