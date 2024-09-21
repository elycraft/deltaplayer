#ifndef SETTINGMANAGER_H
#define SETTINGMANAGER_H
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QString>
#include <QDebug>
#include "GUI_BASE.h"
#include "filemanager.h"

class SettingManager : public QObject
{

Q_OBJECT
public:
    SettingManager(MainWindowt* mainWindow);
    void save();
    QJsonObject load();
    void appExit();
    QVariant getSetting(const QString& key);
    void setSetting(const QString& key, const QVariant& value);

private:
    MainWindowt* window;
    QJsonObject settings;
    QJsonObject BASESETTINGS;
    QString userDataDir;
    FileManager* fm;
};

#endif
