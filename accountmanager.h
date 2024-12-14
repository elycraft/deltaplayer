#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <qobject.h>
#include "GUI_BASE.h"
#include "apimanager.h"
#include "imageloader.h"
class accountManager : public QObject {
Q_OBJECT

public:
    accountManager(MainWindowt* windowin);
    void goPage();

    void handleLogin();
private:
    MainWindowt* window;
    apiManager* api;
    ImageLoader* il;
};

#endif // ACCOUNTMANAGER_H
