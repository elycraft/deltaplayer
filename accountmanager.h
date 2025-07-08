#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <qobject.h>
#include "GUI_BASE.h"
#include "apimanager.h"
#include "imageloader.h"
#include "playlist_manager.h"
#include <QDir>

class accountManager : public QObject {
    Q_OBJECT

public:
    accountManager(MainWindowt* windowin, playlist_manager* playlistM, SettingManager* settingM);
    void goPage();
    void pushPlaylists();
    void handleLogin();
    void handleCreateAcc();
    void handle_noaccount();
    void getPlaylists();
    void handle_logout();

private:
    MainWindowt* window;
    apiManager* api;
    ImageLoader* il;
    FileManager* fm;
    playlist_manager* pm;
    SettingManager* sm;
};

#endif // ACCOUNTMANAGER_H
