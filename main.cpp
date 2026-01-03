#include "musicplayer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    MusicPlayer player;
    player.show();
    
    return app.exec();
}
