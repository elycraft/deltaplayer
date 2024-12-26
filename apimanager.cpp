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
            fileLink = user["record"].toObject()["fileLink"].toString();
            token = user["token"].toString();


        }
        isLogIn = true;
        emit loginFinished();
    });

    // Envoyer une requête POST avec du JSON
    sendPostRequest(apiurl+"/api/collections/users/auth-with-password", json);

}

void apiManager::uploadFile(const QString& filePath, const QString& collection, const QString& recordId, const QString& fieldName,const QString& authToken) {
    QNetworkAccessManager* networkManagerb = new QNetworkAccessManager;

    // URL cible (remplace l'adresse si besoin)
    QString url = QString("http://127.0.0.1:8090/api/collections/%1/records/%2").arg(collection,recordId);

    // Préparer la requête
    QNetworkRequest request((QUrl(url)));

    // Préparer le multipart/form-data
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // Charger le fichier
    QFile* file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        qWarning() << "Impossible d'ouvrir le fichier :" << filePath;
        delete file;
        delete multiPart;
        return;
    }

    // Ajouter la partie fichier
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant(QString("form-data; name=\"%1\"; filename=\"%2\"")
                                    .arg(fieldName)
                                    .arg(file->fileName())));
    filePart.setBodyDevice(file);
    file->setParent(multiPart); // Gérer la mémoire
    multiPart->append(filePart);

    // Définir le Content-Type avec le boundary généré automatiquement
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + multiPart->boundary());

    // Envoyer la requête PATCH
    QNetworkReply* reply = networkManagerb->sendCustomRequest(request, "PATCH", multiPart);
    multiPart->setParent(reply); // Gérer la mémoire

    // Gérer la réponse
    QObject::connect(reply, &QNetworkReply::finished, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Fichier uploadé avec succès !" << reply->readAll();
        } else {
            qWarning() << "Erreur lors de l'upload :" << reply->errorString();
        }
        reply->deleteLater();
    });
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
