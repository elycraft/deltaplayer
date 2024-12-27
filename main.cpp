#include "GUI_BASE.h"
#include "playbar_manager.h"
#include "filemanager.h"
#include "imageloader.h"
#include "playlist_manager.h"
#include "accountmanager.h"
#include "exitprogram.h"
#include "cmdworker.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowt w;

    SettingManager* sm = new SettingManager(&w);

    playbar_manager *playbarM = new playbar_manager(&w, sm);
    playlist_manager *playlistM = new playlist_manager(&w, playbarM->mp,sm);

    accountManager *accountM = new accountManager(&w);

    ExitProgram *exitP = new ExitProgram();
    exitP->addExitFunction([&playlistM](){playlistM->save();});
    exitP->addExitFunction([&sm](){sm->appExit();});

    cmdWorker* test = new cmdWorker();
    test->execAndCaptureOutput("https://www.youtube.com/watch?v=yYKXvpjppsE");

    QObject::connect(&a, &QApplication::aboutToQuit, [&exitP]() {
        exitP->appExit();  // Appel de toutes les fonctions avant de quitter
    });
    w.show();
    return a.exec();
}
