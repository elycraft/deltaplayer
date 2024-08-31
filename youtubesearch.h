#ifndef YOUTUBESEARCH_H
#define YOUTUBESEARCH_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonArray>

class YoutubeSearch : public QObject {
    Q_OBJECT

public:
    explicit YoutubeSearch(const QString& searchTerms, int maxResults = -1, QObject* parent = nullptr);

    QJsonArray toDict(bool clearCache = true);
    QString toJson(bool clearCache = true);
    void search();

private:
    QString searchTerms;
    int maxResults;
    QList<QJsonObject> videos;
    QNetworkAccessManager networkManager;


    void parseHtml(const QString& response);

private slots:
    void onSearchFinished(QNetworkReply* reply);

signals:
    void onComplete(YoutubeSearch* ah);
};



#endif // YOUTUBESEARCH_H
