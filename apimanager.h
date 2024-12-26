#ifndef APIMANAGER_H
#define APIMANAGER_H
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qnetworkreply.h>
#include <qnetworkaccessmanager.h>
#include <qobject.h>
#include <QCoreApplication>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>


class apiManager : public QObject
{
    Q_OBJECT
public:
    apiManager();
    void sendGetRequest(const QString& url);
    void sendPostRequest(const QString& url, const QJsonObject& json);
    void login(QString username, QString password);
    QString getRessource(QString collection, QString recordId, QString filename);
    void uploadFile(const QString& filePath, const QString &collection, const QString& collectionName, const QString& fieldName ,const QString& authToken);

    QJsonObject user;
    QString name;
    QString avatar;
    QString id;
    QString fileLink;
    bool isLogIn;
    QString token;

signals:
    // Signal pour transmettre le résultat JSON
    void jsonReceived(const QJsonObject& json);
    void loginFinished();



private slots:
    // Slot pour gérer la réponse de l'API
    void onReplyFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager networkManager;
    QString apiurl;


};

#endif // APIMANAGER_H
