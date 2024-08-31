#include "GUI_BASE.h"
#include "playbar_manager.h"
#include "filemanager.h"
#include "imageloader.h"
#include "playlist_manager.h"
#include "exitprogram.h"


#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowt w;
    playbar_manager *playbarM = new playbar_manager(&w);
    playlist_manager *playlistM = new playlist_manager(&w, playbarM->mp);

    ExitProgram *exitP = new ExitProgram();
    exitP->addExitFunction([&playlistM](){playlistM->save();});

    QObject::connect(&a, &QApplication::aboutToQuit, [&exitP]() {
        exitP->appExit();  // Appel de toutes les fonctions avant de quitter
    });
    w.show();
    return a.exec();
}
