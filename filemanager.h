#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>

class FileManager {
public:
    QString appName;
    QString appAuthor;
    QString userDataDir;
    QString userCacheDir;
    QString userLogDir;

    FileManager();
    void createDir(const QString& path);
};

#endif // FILEMANAGER_H
