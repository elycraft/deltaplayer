#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <qobject.h>
#include "GUI_BASE.h"
#include "apimanager.h"
#include "imageloader.h"
#include <QDir>

class accountManager : public QObject {
Q_OBJECT

public:
    accountManager(MainWindowt* windowin);
    void goPage();
    void pushPlaylists();
    void handleLogin();
private:
    MainWindowt* window;
    apiManager* api;
    ImageLoader* il;
    FileManager* fm;
};

#endif // ACCOUNTMANAGER_H
