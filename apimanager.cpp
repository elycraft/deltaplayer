#include "apimanager.h"

apiManager::apiManager() {
    isLogIn = false;
    apiurl = "http://127.0.0.1:8090";
    connect(&networkManager, &QNetworkAccessManager::finished, this, &apiManager::onReplyFinished);
}

void apiManager::login(QString username, QString password) {
    QJsonObject json;
    json["identity"] = username;
    json["password"] = password;

    disconnect(this, &apiManager::jsonReceived, nullptr, nullptr);
    connect(this, &apiManager::jsonReceived, this, [this](const QJsonObject& json) {
        qDebug() << "JSON reçu:" << json;

        if (json.contains("token")) {
            user = json;
            name = user["record"].toObject()["name"].toString();
            avatar = user["record"].toObject()["avatar"].toString();
            id = user["record"].toObject()["id"].toString();
            token = user["token"].toString();


        }
        isLogIn = true;
        emit loginFinished();
    });

    // Envoyer une requête POST avec du JSON
    sendPostRequest(apiurl+"/api/collections/users/auth-with-password", json);

}

QString apiManager::getRessource(QString collection, QString recordId, QString filename) {
    return apiurl+"/api/files/"+collection+"/"+recordId+"/"+filename;
}

void apiManager::sendGetRequest(const QString& url) {
    QUrl qurl(url);
    QNetworkRequest request(qurl);

    // Envoyer une requête GET
    networkManager.get(request);
}

void apiManager::sendPostRequest(const QString& url, const QJsonObject& json) {
    QUrl qurl(url);
    QNetworkRequest request(qurl);

    // Définir l'en-tête HTTP pour indiquer que le corps est au format JSON
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Convertir le JSON en QByteArray
    QJsonDocument jsonDoc(json);
    QByteArray jsonData = jsonDoc.toJson();

    // Envoyer une requête POST avec le JSON
    networkManager.post(request, jsonData);
}


void apiManager::onReplyFinished(QNetworkReply* reply) {
    // Vérifie les erreurs
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "API request error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    // Lire la réponse en texte brut
    QByteArray response = reply->readAll();

    // Convertir en JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    if (jsonDoc.isObject()) {
        emit jsonReceived(jsonDoc.object());
    } else {
        qWarning() << "Invalid JSON received";
    }

    // Libérer la mémoire
    reply->deleteLater();
}
