#include "YoutubeSearch.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QDebug>
#include <QThread>
#include <QUrl>
#include <QUrlQuery>

YoutubeSearch::YoutubeSearch(const QString& searchTerms, int maxResults, QObject* parent)
    : QObject(parent), searchTerms(searchTerms), maxResults(maxResults) {
    connect(&networkManager, &QNetworkAccessManager::finished, this, &YoutubeSearch::onSearchFinished);

}

QJsonArray YoutubeSearch::toDict(bool clearCache) {
    QJsonArray resultArray;
    for (const auto& video : videos) {
        resultArray.append(video);
    }
    if (clearCache) {
        videos.clear();
    }
    return resultArray;
}

QString YoutubeSearch::toJson(bool clearCache) {
    QJsonObject jsonObject;
    jsonObject["videos"] = toDict(false);
    QJsonDocument jsonDoc(jsonObject);
    QString result = jsonDoc.toJson(QJsonDocument::Compact);
    if (clearCache) {
        videos.clear();
    }
    return result;
}

void YoutubeSearch::search() {
    QString encodedSearch = QUrl::toPercentEncoding(searchTerms);
    QString url = QString("https://youtube.com/results?search_query=%1").arg(encodedSearch);
    //qInfo() << url;
    QUrl qUrl(url);
    QNetworkRequest request(qUrl);
    networkManager.get(request);
    //qInfo("am");
}

void YoutubeSearch::parseHtml(const QString& response) {
    // Trouver le début de la chaîne "ytInitialData"
    int start = response.indexOf("ytInitialData") + QString("ytInitialData").length() + 3;

    // Trouver la fin du bloc de données JSON
    int end = response.indexOf("};", start) + 1;

    // Extraire la sous-chaîne JSON entre les index 'start' et 'end'
    QString json_str = response.mid(start, end - start).replace("\n","").replace("\r\n","");

    QJsonObject jsonObject;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json_str.toUtf8());
    QJsonObject data = jsonDoc.object();

    QJsonArray contents = data["contents"].toObject()["twoColumnSearchResultsRenderer"]
                              .toObject()["primaryContents"].toObject()["sectionListRenderer"]
                              .toObject()["contents"].toArray();
    //qInfo() << json_str;
    //qInfo() << data;
    for (const QJsonValue& value : contents) {

        QJsonObject item = value.toObject()["itemSectionRenderer"].toObject();
        QJsonArray videoContents = item["contents"].toArray();
        for (const QJsonValue& videoValue : videoContents) {
            QJsonObject video = videoValue.toObject()["videoRenderer"].toObject();
            QJsonObject res;
            res["id"] = video.value("videoId").toString();
            if (res["id"] == "") {continue;}
            QJsonArray thumbnails = video["thumbnail"].toObject().value("thumbnails").toArray();
            QJsonArray thumbArray;
            for (const QJsonValue& thumb : thumbnails) {
                thumbArray.append(thumb.toObject().value("url").toString());
            }
            res["thumbnails"] = thumbArray;
            res["title"] = video["title"].toObject().value("runs").toArray().first().toObject().value("text").toString();
            res["long_desc"] = video["descriptionSnippet"].toObject().value("runs").toArray().first().toObject().value("text").toString();
            res["channel"] = video["longBylineText"].toObject().value("runs").toArray().first().toObject().value("text").toString();
            res["duration"] = video["lengthText"].toObject().value("simpleText").toString();
            res["views"] = video["viewCountText"].toObject().value("simpleText").toString();
            res["publish_time"] = video["publishedTimeText"].toObject().value("simpleText").toString();
            res["url_suffix"] = video["navigationEndpoint"].toObject().value("commandMetadata").toObject().value("webCommandMetadata").toObject().value("url").toString();
            //qInfo() << video.value("videoId").toString();;
            videos.append(res);
        }
    }
    if (maxResults > 0 && videos.size() > maxResults) {
        videos = videos.mid(0, maxResults);
    }
    emit this->onComplete(this);
}

void YoutubeSearch::onSearchFinished(QNetworkReply* reply) {
    //qInfo("oo");
    QString response = reply->readAll();
    reply->deleteLater();


    // Wait for the content to be fully loaded
    while (!response.contains("ytInitialData")) {
        QThread::msleep(10); // Simple delay; adjust as needed
        response = reply->readAll();
    }
    //qInfo() << "response";
    parseHtml(response);
}
