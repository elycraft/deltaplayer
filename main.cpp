#include "GUI_BASE.h"
#include "playbar_manager.h"
#include "filemanager.h"
#include "imageloader.h"
#include "playlist_manager.h"
#include "accountmanager.h"
#include "exitprogram.h"
#include "androidytdlpmanager.h"
#include "androidnotification.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    //QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setOption(QSurfaceFormat::DebugContext);
    QSurfaceFormat::setDefaultFormat(format);

    MainWindowt w;



    SettingManager* sm = new SettingManager(&w);

    androidYtdlpManager* YtdlpM = new androidYtdlpManager();
    a.setProperty("YtdlpM", QVariant::fromValue(YtdlpM));

    androidnotification* aNotif = new androidnotification();
    a.setProperty("aNotif", QVariant::fromValue(aNotif));

    playbar_manager *playbarM = new playbar_manager(&w, sm);
    playlist_manager *playlistM = new playlist_manager(&w, playbarM->mp,sm);

    accountManager *accountM = new accountManager(&w,playlistM,sm);


    ExitProgram *exitP = new ExitProgram();
    exitP->addExitFunction([&playlistM](){playlistM->save();});
    exitP->addExitFunction([&sm](){sm->appExit();});

    QObject::connect(&a, &QApplication::aboutToQuit, [&exitP]() {
        exitP->appExit();  // Appel de toutes les fonctions avant de quitter
    });

    QTimer *refreshTimer = new QTimer();
    QObject::connect(refreshTimer, &QTimer::timeout, []() {
        for (QWidget *widget : QApplication::topLevelWidgets()) {
            widget->update(); // Force une mise à jour
        }
    });
    refreshTimer->start(33); // Rafraîchissement à environ 60 FPS


    w.show();
    return a.exec();
}
