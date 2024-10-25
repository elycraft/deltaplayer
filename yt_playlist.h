#ifndef YT_PLAYLIST_H
#define YT_PLAYLIST_H
#include <QObject>
#include <QString>
#include <map>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMetaType>
#include "cmdworker.h"

class yt_playlist : public QObject {
Q_OBJECT
public:
    yt_playlist(QString furl, QString ytDlPath);

    QJsonArray toDict(bool clearCache = true);
    QString toJson(bool clearCache = true);
    QJsonArray get_info();

private:
    QString furl;
    QList<QJsonObject> videos;
    std::string ytDlpPath;

};

#endif // YT_PLAYLIST_H
