#include "musicplayer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenuBar>
#include <QMenu>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QImage>
#include <QPixmap>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/flacfile.h>

MusicPlayer::MusicPlayer(QWidget *parent)
    : QMainWindow(parent)
    , currentSongIndex(-1)
    , currentPlaylistName("默認播放清單")
{
    setWindowTitle("音樂播放器");
    setGeometry(100, 100, 1000, 700);
    
    // Initialize media player
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);
    
    // Initialize playlists
    playlists[currentPlaylistName] = QStringList();
    
    initUI();
    connectSignals();
}

MusicPlayer::~MusicPlayer()
{
}

void MusicPlayer::initUI()
{
    // Central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Menu bar
    createMenuBar();
    
    // Create splitter for main content
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    
    // Left panel - Playlist and controls
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    
    // Album cover display
    coverLabel = new QLabel();
    coverLabel->setFixedSize(300, 300);
    coverLabel->setScaledContents(true);
    coverLabel->setStyleSheet("border: 1px solid #ccc; background-color: #f0f0f0;");
    coverLabel->setText("無封面");
    coverLabel->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(coverLabel);
    
    // Current song info
    songInfoLabel = new QLabel("未選擇歌曲");
    songInfoLabel->setAlignment(Qt::AlignCenter);
    songInfoLabel->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
    leftLayout->addWidget(songInfoLabel);
    
    // Progress bar
    QHBoxLayout *progressLayout = new QHBoxLayout();
    currentTimeLabel = new QLabel("00:00");
    progressSlider = new QSlider(Qt::Horizontal);
    progressSlider->setRange(0, 0);
    totalTimeLabel = new QLabel("00:00");
    progressLayout->addWidget(currentTimeLabel);
    progressLayout->addWidget(progressSlider);
    progressLayout->addWidget(totalTimeLabel);
    leftLayout->addLayout(progressLayout);
    
    // Control buttons
    QHBoxLayout *controlLayout = new QHBoxLayout();
    prevButton = new QPushButton("⏮ 上一首");
    playButton = new QPushButton("▶ 播放");
    pauseButton = new QPushButton("⏸ 暫停");
    stopButton = new QPushButton("⏹ 停止");
    nextButton = new QPushButton("⏭ 下一首");
    
    controlLayout->addWidget(prevButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(pauseButton);
    controlLayout->addWidget(stopButton);
    controlLayout->addWidget(nextButton);
    leftLayout->addLayout(controlLayout);
    
    // Volume control
    QHBoxLayout *volumeLayout = new QHBoxLayout();
    QLabel *volumeLabel = new QLabel("音量:");
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeLabelValue = new QLabel("50%");
    volumeLayout->addWidget(volumeLabel);
    volumeLayout->addWidget(volumeSlider);
    volumeLayout->addWidget(volumeLabelValue);
    leftLayout->addLayout(volumeLayout);
    
    // Playlist
    QLabel *playlistLabel = new QLabel("播放清單:");
    playlistLabel->setStyleSheet("font-weight: bold;");
    leftLayout->addWidget(playlistLabel);
    
    playlistWidget = new QListWidget();
    leftLayout->addWidget(playlistWidget);
    
    // Playlist control buttons
    QHBoxLayout *playlistControlLayout = new QHBoxLayout();
    addSongButton = new QPushButton("➕ 添加歌曲");
    removeSongButton = new QPushButton("➖ 刪除歌曲");
    moveUpButton = new QPushButton("⬆ 上移");
    moveDownButton = new QPushButton("⬇ 下移");
    
    playlistControlLayout->addWidget(addSongButton);
    playlistControlLayout->addWidget(removeSongButton);
    playlistControlLayout->addWidget(moveUpButton);
    playlistControlLayout->addWidget(moveDownButton);
    leftLayout->addLayout(playlistControlLayout);
    
    loadLyricsButton = new QPushButton("📝 載入字幕");
    leftLayout->addWidget(loadLyricsButton);
    
    // Right panel - Lyrics
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    QLabel *lyricsLabel = new QLabel("歌詞:");
    lyricsLabel->setStyleSheet("font-weight: bold;");
    rightLayout->addWidget(lyricsLabel);
    
    lyricsWidget = new QListWidget();
    rightLayout->addWidget(lyricsWidget);
    
    // Add panels to splitter
    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(splitter);
}

void MusicPlayer::createMenuBar()
{
    QMenuBar *menubar = this->menuBar();
    
    // File menu
    QMenu *fileMenu = menubar->addMenu("檔案");
    
    QAction *addFilesAction = new QAction("添加音樂檔案", this);
    connect(addFilesAction, &QAction::triggered, this, &MusicPlayer::addSongs);
    fileMenu->addAction(addFilesAction);
    
    QAction *exitAction = new QAction("退出", this);
    connect(exitAction, &QAction::triggered, this, &MusicPlayer::close);
    fileMenu->addAction(exitAction);
    
    // Playlist menu
    QMenu *playlistMenu = menubar->addMenu("播放清單");
    
    QAction *newPlaylistAction = new QAction("新建播放清單", this);
    connect(newPlaylistAction, &QAction::triggered, this, &MusicPlayer::createNewPlaylist);
    playlistMenu->addAction(newPlaylistAction);
    
    QAction *switchPlaylistAction = new QAction("切換播放清單", this);
    connect(switchPlaylistAction, &QAction::triggered, this, &MusicPlayer::switchPlaylist);
    playlistMenu->addAction(switchPlaylistAction);
    
    QAction *savePlaylistAction = new QAction("保存播放清單", this);
    connect(savePlaylistAction, &QAction::triggered, this, &MusicPlayer::savePlaylists);
    playlistMenu->addAction(savePlaylistAction);
    
    QAction *loadPlaylistAction = new QAction("載入播放清單", this);
    connect(loadPlaylistAction, &QAction::triggered, this, &MusicPlayer::loadPlaylists);
    playlistMenu->addAction(loadPlaylistAction);
}

void MusicPlayer::connectSignals()
{
    // Player signals
    connect(player, &QMediaPlayer::positionChanged, this, &MusicPlayer::updatePosition);
    connect(player, &QMediaPlayer::durationChanged, this, &MusicPlayer::updateDuration);
    connect(player, &QMediaPlayer::sourceChanged, this, &MusicPlayer::onMediaChanged);
    
    // Control buttons
    connect(playButton, &QPushButton::clicked, this, &MusicPlayer::play);
    connect(pauseButton, &QPushButton::clicked, this, &MusicPlayer::pause);
    connect(stopButton, &QPushButton::clicked, this, &MusicPlayer::stop);
    connect(prevButton, &QPushButton::clicked, this, &MusicPlayer::previousSong);
    connect(nextButton, &QPushButton::clicked, this, &MusicPlayer::nextSong);
    
    // Playlist buttons
    connect(addSongButton, &QPushButton::clicked, this, &MusicPlayer::addSongs);
    connect(removeSongButton, &QPushButton::clicked, this, &MusicPlayer::removeSong);
    connect(moveUpButton, &QPushButton::clicked, this, &MusicPlayer::moveSongUp);
    connect(moveDownButton, &QPushButton::clicked, this, &MusicPlayer::moveSongDown);
    connect(loadLyricsButton, &QPushButton::clicked, this, &MusicPlayer::loadLyrics);
    
    // Sliders
    connect(progressSlider, &QSlider::sliderMoved, this, &MusicPlayer::setPosition);
    connect(volumeSlider, &QSlider::valueChanged, this, &MusicPlayer::setVolume);
    
    // Playlist widget
    connect(playlistWidget, &QListWidget::itemDoubleClicked, this, &MusicPlayer::playSelectedSong);
    
    // Lyrics widget
    connect(lyricsWidget, &QListWidget::itemClicked, this, &MusicPlayer::jumpToLyricsTime);
}

void MusicPlayer::addSongs()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "選擇音樂檔案",
        "",
        "音頻檔案 (*.mp3 *.wav *.flac *.aac *.m4a *.ogg);;所有檔案 (*.*)"
    );
    
    if (!files.isEmpty()) {
        for (const QString &filePath : files) {
            songList.append(filePath);
            
            // Add to playlist widget
            QFileInfo fileInfo(filePath);
            QString songName = fileInfo.fileName();
            playlistWidget->addItem(songName);
            
            // Save to current playlist
            playlists[currentPlaylistName].append(filePath);
        }
    }
}

void MusicPlayer::removeSong()
{
    int currentRow = playlistWidget->currentRow();
    if (currentRow >= 0) {
        playlistWidget->takeItem(currentRow);
        songList.removeAt(currentRow);
        playlists[currentPlaylistName].removeAt(currentRow);
        
        // Adjust current song index if needed
        if (currentSongIndex == currentRow) {
            stop();
            currentSongIndex = -1;
        } else if (currentSongIndex > currentRow) {
            currentSongIndex--;
        }
    }
}

void MusicPlayer::moveSongUp()
{
    int currentRow = playlistWidget->currentRow();
    if (currentRow > 0) {
        // Move in list widget
        QListWidgetItem *item = playlistWidget->takeItem(currentRow);
        playlistWidget->insertItem(currentRow - 1, item);
        playlistWidget->setCurrentRow(currentRow - 1);
        
        // Move in song list
        songList.swapItemsAt(currentRow, currentRow - 1);
        
        // Move in playlists
        playlists[currentPlaylistName].swapItemsAt(currentRow, currentRow - 1);
        
        // Update current song index
        if (currentSongIndex == currentRow) {
            currentSongIndex = currentRow - 1;
        } else if (currentSongIndex == currentRow - 1) {
            currentSongIndex = currentRow;
        }
    }
}

void MusicPlayer::moveSongDown()
{
    int currentRow = playlistWidget->currentRow();
    if (currentRow >= 0 && currentRow < playlistWidget->count() - 1) {
        // Move in list widget
        QListWidgetItem *item = playlistWidget->takeItem(currentRow);
        playlistWidget->insertItem(currentRow + 1, item);
        playlistWidget->setCurrentRow(currentRow + 1);
        
        // Move in song list
        songList.swapItemsAt(currentRow, currentRow + 1);
        
        // Move in playlists
        playlists[currentPlaylistName].swapItemsAt(currentRow, currentRow + 1);
        
        // Update current song index
        if (currentSongIndex == currentRow) {
            currentSongIndex = currentRow + 1;
        } else if (currentSongIndex == currentRow + 1) {
            currentSongIndex = currentRow;
        }
    }
}

void MusicPlayer::play()
{
    if (songList.isEmpty()) {
        return;
    }
    
    // If no song is selected, play the first one
    if (currentSongIndex < 0 || currentSongIndex >= songList.size()) {
        currentSongIndex = 0;
    }
    
    // If player is stopped, load the current song
    if (player->playbackState() == QMediaPlayer::StoppedState) {
        QString filePath = songList[currentSongIndex];
        player->setSource(QUrl::fromLocalFile(filePath));
    }
    
    player->play();
}

void MusicPlayer::pause()
{
    player->pause();
}

void MusicPlayer::stop()
{
    player->stop();
}

void MusicPlayer::previousSong()
{
    if (songList.isEmpty()) {
        return;
    }
    
    currentSongIndex--;
    if (currentSongIndex < 0) {
        currentSongIndex = songList.size() - 1;
    }
    
    QString filePath = songList[currentSongIndex];
    player->setSource(QUrl::fromLocalFile(filePath));
    player->play();
}

void MusicPlayer::nextSong()
{
    if (songList.isEmpty()) {
        return;
    }
    
    currentSongIndex++;
    if (currentSongIndex >= songList.size()) {
        currentSongIndex = 0;
    }
    
    QString filePath = songList[currentSongIndex];
    player->setSource(QUrl::fromLocalFile(filePath));
    player->play();
}

void MusicPlayer::playSelectedSong(QListWidgetItem *item)
{
    int index = playlistWidget->row(item);
    if (index >= 0 && index < songList.size()) {
        currentSongIndex = index;
        QString filePath = songList[currentSongIndex];
        player->setSource(QUrl::fromLocalFile(filePath));
        player->play();
    }
}

void MusicPlayer::updatePosition(qint64 position)
{
    progressSlider->setValue(position);
    currentTimeLabel->setText(formatTime(position));
    
    // Update lyrics highlight
    updateLyricsHighlight(position);
}

void MusicPlayer::updateDuration(qint64 duration)
{
    progressSlider->setRange(0, duration);
    totalTimeLabel->setText(formatTime(duration));
}

void MusicPlayer::setPosition(int position)
{
    player->setPosition(position);
}

void MusicPlayer::setVolume(int value)
{
    audioOutput->setVolume(value / 100.0);
    volumeLabelValue->setText(QString("%1%").arg(value));
}

QString MusicPlayer::formatTime(qint64 ms)
{
    qint64 s = ms / 1000;
    qint64 m = s / 60;
    s = s % 60;
    return QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
}

void MusicPlayer::onMediaChanged()
{
    if (currentSongIndex >= 0 && currentSongIndex < songList.size()) {
        QString filePath = songList[currentSongIndex];
        QFileInfo fileInfo(filePath);
        QString songName = fileInfo.fileName();
        songInfoLabel->setText(songName);
        
        // Highlight current song in playlist
        playlistWidget->setCurrentRow(currentSongIndex);
        
        // Load and display album cover
        loadAlbumCover(filePath);
        
        // Load lyrics if available
        loadLyricsForCurrentSong(filePath);
    }
}

void MusicPlayer::loadAlbumCover(const QString &filePath)
{
    try {
        TagLib::FileRef file(filePath.toStdString().c_str());
        
        if (!file.isNull() && file.file()) {
            // Try MP3 files
            TagLib::MPEG::File *mpegFile = dynamic_cast<TagLib::MPEG::File*>(file.file());
            if (mpegFile && mpegFile->ID3v2Tag()) {
                TagLib::ID3v2::FrameList frames = mpegFile->ID3v2Tag()->frameListMap()["APIC"];
                if (!frames.isEmpty()) {
                    TagLib::ID3v2::AttachedPictureFrame *frame = 
                        static_cast<TagLib::ID3v2::AttachedPictureFrame*>(frames.front());
                    
                    QByteArray imageData(frame->picture().data(), frame->picture().size());
                    QImage image;
                    if (image.loadFromData(imageData)) {
                        QPixmap pixmap = QPixmap::fromImage(image);
                        coverLabel->setPixmap(pixmap);
                        return;
                    }
                }
            }
            
            // Try FLAC files
            TagLib::FLAC::File *flacFile = dynamic_cast<TagLib::FLAC::File*>(file.file());
            if (flacFile && !flacFile->pictureList().isEmpty()) {
                TagLib::FLAC::Picture *picture = flacFile->pictureList().front();
                QByteArray imageData(picture->data().data(), picture->data().size());
                QImage image;
                if (image.loadFromData(imageData)) {
                    QPixmap pixmap = QPixmap::fromImage(image);
                    coverLabel->setPixmap(pixmap);
                    return;
                }
            }
        }
        
        // No cover found
        coverLabel->setText("無封面");
        coverLabel->setPixmap(QPixmap());
    } catch (...) {
        coverLabel->setText("無法載入封面");
        coverLabel->setPixmap(QPixmap());
    }
}

void MusicPlayer::loadLyrics()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "選擇字幕檔案",
        "",
        "LRC字幕檔 (*.lrc);;文字檔 (*.txt);;所有檔案 (*.*)"
    );
    
    if (!filePath.isEmpty()) {
        parseLyricsFile(filePath);
    }
}

void MusicPlayer::loadLyricsForCurrentSong(const QString &songPath)
{
    // Try to find lyrics file with same name
    QFileInfo fileInfo(songPath);
    QString basePath = fileInfo.absolutePath() + "/" + fileInfo.completeBaseName();
    QString lrcPath = basePath + ".lrc";
    
    if (QFile::exists(lrcPath)) {
        parseLyricsFile(lrcPath);
    } else {
        lyricsWidget->clear();
        lyricsData.clear();
    }
}

void MusicPlayer::parseLyricsFile(const QString &filePath)
{
    lyricsWidget->clear();
    lyricsData.clear();
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "錯誤", "無法打開字幕檔案");
        return;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        if (line.isEmpty()) {
            continue;
        }
        
        // Parse LRC format: [mm:ss.xx]lyrics
        if (line.startsWith('[')) {
            int endBracket = line.indexOf(']');
            if (endBracket > 0) {
                QString timestampStr = line.mid(1, endBracket - 1);
                QString lyricsText = line.mid(endBracket + 1).trimmed();
                
                // Parse timestamp
                if (timestampStr.contains(':')) {
                    QStringList parts = timestampStr.split(':');
                    if (parts.size() >= 2) {
                        int minutes = parts[0].toInt();
                        QStringList secondsParts = parts[1].split('.');
                        int seconds = secondsParts[0].toInt();
                        int milliseconds = secondsParts.size() > 1 ? secondsParts[1].toInt() * 10 : 0;
                        
                        qint64 totalMs = (minutes * 60 + seconds) * 1000 + milliseconds;
                        
                        // Store lyrics with timestamp
                        lyricsData[totalMs] = lyricsText;
                        
                        // Add to list widget
                        QListWidgetItem *item = new QListWidgetItem(lyricsText);
                        item->setData(Qt::UserRole, totalMs);
                        lyricsWidget->addItem(item);
                    }
                }
            }
        } else {
            // Non-timestamp line
            lyricsWidget->addItem(line);
        }
    }
    
    file.close();
    currentLyricsFile = filePath;
}

void MusicPlayer::updateLyricsHighlight(qint64 position)
{
    if (lyricsData.isEmpty()) {
        return;
    }
    
    // Find the current lyrics line
    QListWidgetItem *currentItem = nullptr;
    for (int i = 0; i < lyricsWidget->count(); i++) {
        QListWidgetItem *item = lyricsWidget->item(i);
        QVariant timestampVar = item->data(Qt::UserRole);
        
        if (!timestampVar.isNull()) {
            qint64 timestamp = timestampVar.toLongLong();
            
            if (timestamp <= position) {
                currentItem = item;
            } else {
                break;
            }
        }
    }
    
    // Highlight current line
    for (int i = 0; i < lyricsWidget->count(); i++) {
        QListWidgetItem *item = lyricsWidget->item(i);
        if (item == currentItem) {
            item->setBackground(Qt::yellow);
            // Auto-scroll to current line
            lyricsWidget->scrollToItem(item);
        } else {
            item->setBackground(Qt::white);
        }
    }
}

void MusicPlayer::jumpToLyricsTime(QListWidgetItem *item)
{
    QVariant timestampVar = item->data(Qt::UserRole);
    if (!timestampVar.isNull()) {
        qint64 timestamp = timestampVar.toLongLong();
        player->setPosition(timestamp);
    }
}

void MusicPlayer::createNewPlaylist()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新建播放清單", "輸入播放清單名稱:", 
                                          QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        if (!playlists.contains(name)) {
            playlists[name] = QStringList();
            QMessageBox::information(this, "成功", QString("播放清單 '%1' 已創建").arg(name));
        } else {
            QMessageBox::warning(this, "錯誤", "播放清單名稱已存在");
        }
    }
}

void MusicPlayer::switchPlaylist()
{
    if (playlists.isEmpty()) {
        QMessageBox::warning(this, "錯誤", "沒有可用的播放清單");
        return;
    }
    
    QStringList names = playlists.keys();
    bool ok;
    QString name = QInputDialog::getItem(this, "切換播放清單", "選擇播放清單:", 
                                         names, 0, false, &ok);
    
    if (ok && !name.isEmpty()) {
        currentPlaylistName = name;
        loadPlaylistSongs(name);
    }
}

void MusicPlayer::loadPlaylistSongs(const QString &name)
{
    // Clear current playlist
    playlistWidget->clear();
    songList.clear();
    
    // Stop current playback
    stop();
    currentSongIndex = -1;
    
    // Load songs from selected playlist
    for (const QString &filePath : playlists[name]) {
        if (QFile::exists(filePath)) {
            songList.append(filePath);
            QFileInfo fileInfo(filePath);
            QString songName = fileInfo.fileName();
            playlistWidget->addItem(songName);
        }
    }
    
    setWindowTitle(QString("音樂播放器 - %1").arg(name));
}

void MusicPlayer::savePlaylists()
{
    QString filePath = QFileDialog::getSaveFileName(
        this, "保存播放清單", "", "JSON檔案 (*.json)"
    );
    
    if (!filePath.isEmpty()) {
        QJsonObject rootObj;
        
        for (auto it = playlists.begin(); it != playlists.end(); ++it) {
            QJsonArray songArray;
            for (const QString &song : it.value()) {
                songArray.append(song);
            }
            rootObj[it.key()] = songArray;
        }
        
        QJsonDocument doc(rootObj);
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(doc.toJson(QJsonDocument::Indented));
            file.close();
            QMessageBox::information(this, "成功", "播放清單已保存");
        } else {
            QMessageBox::warning(this, "錯誤", "無法保存播放清單");
        }
    }
}

void MusicPlayer::loadPlaylists()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "載入播放清單", "", "JSON檔案 (*.json)"
    );
    
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray data = file.readAll();
            file.close();
            
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject()) {
                playlists.clear();
                QJsonObject rootObj = doc.object();
                
                for (auto it = rootObj.begin(); it != rootObj.end(); ++it) {
                    QStringList songs;
                    if (it.value().isArray()) {
                        QJsonArray songArray = it.value().toArray();
                        for (const QJsonValue &val : songArray) {
                            songs.append(val.toString());
                        }
                    }
                    playlists[it.key()] = songs;
                }
                
                if (!playlists.isEmpty()) {
                    // Switch to first playlist
                    QString firstName = playlists.keys().first();
                    currentPlaylistName = firstName;
                    loadPlaylistSongs(firstName);
                }
                
                QMessageBox::information(this, "成功", "播放清單已載入");
            } else {
                QMessageBox::warning(this, "錯誤", "無效的JSON格式");
            }
        } else {
            QMessageBox::warning(this, "錯誤", "無法載入播放清單");
        }
    }
}
