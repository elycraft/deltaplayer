#include "FileManager.h"
#include <QStandardPaths>
#include <QDir>

FileManager::FileManager() {
    appName = "deltaplayer";
    appAuthor = "CGS";

    userDataDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    userCacheDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/Cache";
    userLogDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/Logs";

    createDir(userDataDir);
    createDir(userCacheDir);
    createDir(userLogDir);
}

void FileManager::createDir(const QString& path) {
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}
