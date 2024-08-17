#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCryptographicHash>
#include "FileManager.h"

class ImageLoader : public QObject {
Q_OBJECT
public:
    FileManager* fm;
    QNetworkAccessManager networkManager;

    ImageLoader(FileManager* fm = nullptr);
    ~ImageLoader();

    QString get(const QString& url);
    QString getfn(const QString& url);
};

#endif // IMAGELOADER_H
