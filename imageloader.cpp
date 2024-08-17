#include "ImageLoader.h"
#include <QFile>
#include <QDir>
#include <QEventLoop>
#include <QDebug>

ImageLoader::ImageLoader(FileManager* fm) {
    if (fm == nullptr) {
        this->fm = new FileManager();
    } else {
        this->fm = fm;
    }
}

ImageLoader::~ImageLoader() {
    if (fm != nullptr) {
        delete fm;
    }
}

QString ImageLoader::get(const QString& url) {
    QString ph = fm->userCacheDir + "/" + getfn(url);

    if (!QFile::exists(ph)) {
        QNetworkRequest request;
        request.setUrl(QUrl(url));
        QNetworkReply* reply = networkManager.get(request);

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            QFile file(ph);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(reply->readAll());
                file.close();
            }
        } else {
            qDebug() << "Failed to download the file:" << reply->errorString();
        }
        reply->deleteLater();
    }

    return ph.replace("\\", "/");
}

QString ImageLoader::getfn(const QString& url) {
    QByteArray urlBytes = url.toUtf8();
    QByteArray hash = QCryptographicHash::hash(urlBytes, QCryptographicHash::Md5);
    QString hashString = hash.toHex();
    QString extension = url.section('/', -1).section('?', 0, 0).section('.', -1);
    return hashString + "." + extension;
}
