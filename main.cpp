#include "GUI_BASE.h"
#include "playbar_manager.h"
#include "filemanager.h"
#include "imageloader.h"
#include "playlist_manager.h"


#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowt w;
    playbar_manager *o = new playbar_manager(&w);
    playlist_manager *playlistM = new playlist_manager(&w, o->mp);

    w.show();
    return a.exec();
}
