#ifndef APIMANAGER_H
#define APIMANAGER_H
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qnetworkreply.h>
#include <qnetworkaccessmanager.h>
#include <qobject.h>

class apiManager : public QObject
{
    Q_OBJECT
public:
    apiManager();
    void sendGetRequest(const QString& url);
    void sendPostRequest(const QString& url, const QJsonObject& json);
    void login(QString username, QString password);
    QString getRessource(QString collection, QString recordId, QString filename);

    QJsonObject user;
    QString name;
    QString avatar;
    QString id;
    bool isLogIn;

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
    QString token;

};

#endif // APIMANAGER_H
