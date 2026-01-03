#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QListWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QMap>
#include <QStringList>

class MusicPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MusicPlayer(QWidget *parent = nullptr);
    ~MusicPlayer();

private slots:
    // Playback controls
    void play();
    void pause();
    void stop();
    void previousSong();
    void nextSong();
    void playSelectedSong(QListWidgetItem *item);
    
    // Media player signals
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void onMediaChanged();
    
    // Slider controls
    void setPosition(int position);
    void setVolume(int value);
    
    // Playlist management
    void addSongs();
    void removeSong();
    void moveSongUp();
    void moveSongDown();
    
    // Lyrics
    void loadLyrics();
    void updateLyricsHighlight(qint64 position);
    void jumpToLyricsTime(QListWidgetItem *item);
    
    // Playlists
    void createNewPlaylist();
    void switchPlaylist();
    void savePlaylists();
    void loadPlaylists();

private:
    void initUI();
    void createMenuBar();
    void connectSignals();
    void loadAlbumCover(const QString &filePath);
    void loadLyricsForCurrentSong(const QString &songPath);
    void parseLyricsFile(const QString &filePath);
    QString formatTime(qint64 ms);
    void loadPlaylistSongs(const QString &name);
    
    // UI Components
    QLabel *coverLabel;
    QLabel *songInfoLabel;
    QLabel *currentTimeLabel;
    QLabel *totalTimeLabel;
    QLabel *volumeLabelValue;
    QSlider *progressSlider;
    QSlider *volumeSlider;
    QPushButton *prevButton;
    QPushButton *playButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *nextButton;
    QPushButton *addSongButton;
    QPushButton *removeSongButton;
    QPushButton *moveUpButton;
    QPushButton *moveDownButton;
    QPushButton *loadLyricsButton;
    QListWidget *playlistWidget;
    QListWidget *lyricsWidget;
    
    // Media player
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    
    // Data structures
    QStringList songList;
    QMap<qint64, QString> lyricsData;
    QString currentLyricsFile;
    int currentSongIndex;
    
    // Playlists
    QMap<QString, QStringList> playlists;
    QString currentPlaylistName;
};

#endif // MUSICPLAYER_H
