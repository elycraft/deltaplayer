#include "apimanager.h"
#include <qmessagebox.h>

apiManager::apiManager() {
    isLogIn = false;
    apiurl = "https://dpapi.elydev.fr";
    fm = new FileManager;
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

            if (fileLink=="") {
                createFilesAccount();
            }
            isLogIn = true;
            emit loginFinished(true);



        } else {
            QMessageBox messageBox;
            messageBox.warning(0,"Login impossible","Couldn't login. Are \nyour credentials correct ?");
            emit loginFinished(false);

        }

    });

    // Envoyer une requête POST avec du JSON
    sendPostRequest(apiurl+"/api/collections/users/auth-with-password", json);

}

void apiManager::authRefresh(QString possibleToken) {
    QJsonObject json;
    token = possibleToken;

    disconnect(this, &apiManager::jsonReceived, nullptr, nullptr);
    connect(this, &apiManager::jsonReceived, this, [this](const QJsonObject& json) {
        qDebug() << "JSON reçu:" << json;
        token = "";

        if (json.contains("token")) {
            user = json;
            name = user["record"].toObject()["name"].toString();
            avatar = user["record"].toObject()["avatar"].toString();
            id = user["record"].toObject()["id"].toString();
            fileLink = user["record"].toObject()["fileLink"].toString();
            token = user["token"].toString();

            if (fileLink=="") {
                createFilesAccount();
            }
            isLogIn = true;



        }
        emit loginFinished(true);

    });

    // Envoyer une requête POST avec du JSON
    sendPostRequest(apiurl+"/api/collections/users/auth-refresh", json);

}

void apiManager::createAccount(QString username, QString password,QString nameen) {
    QJsonObject json;
    json["email"] = username;
    json["password"] = password;
    json["passwordConfirm"] = password;
    json["name"] = nameen;

    disconnect(this, &apiManager::jsonReceived, nullptr, nullptr);
    connect(this, &apiManager::jsonReceived, this, [this](const QJsonObject& json) {
        qDebug() << "JSON reçu:" << json;

        id = json["id"].toString();
        if (json.contains("token")) {
            user = json;



        }
        isLogIn = true;
        emit createFinished();
    });

    // Envoyer une requête POST avec du JSON
    sendPostRequest(apiurl+"/api/collections/users/records", json);

}

void apiManager::createFilesAccount() {
    QJsonObject json;
    disconnect(this, &apiManager::jsonReceived, nullptr, nullptr);
    connect(this, &apiManager::jsonReceived, this, [this](const QJsonObject& json) {
        qDebug() << "JSON reçu:" << json;

        fileLink = json["id"].toString();



        QNetworkAccessManager* networkManagerb = new QNetworkAccessManager;
        QUrl qurl(apiurl+"/api/collections/users/records/"+id);
        QNetworkRequest request(qurl);

        QByteArray fd = QByteArray::fromStdString(QString("fileLink=%1").arg(fileLink).toStdString());

        // Définir l'en-tête HTTP pour indiquer que le corps est au format JSON
        if (token!="") {request.setRawHeader(QByteArray("Authorization"), QByteArray::fromStdString(token.toStdString()));}
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        // Envoyer la requête PATCH
        QNetworkReply* reply = networkManagerb->sendCustomRequest(request, "PATCH", fd);
        // Gérer la réponse
        qDebug()<<fd;

        QObject::connect(reply, &QNetworkReply::finished, [reply]() {
            if (reply->error() == QNetworkReply::NoError) {
                qDebug() << "succès !" << reply->readAll();
            } else {
                qWarning() << "Erreur lors de l'upload :" << reply->errorString();
            }
            reply->deleteLater();
        });

    });

    // Envoyer une requête POST avec du JSON
    sendPostRequest(apiurl+"/api/collections/files/records", json);
}

void apiManager::downloadFromServer() {
    disconnect(this, &apiManager::jsonReceived, nullptr, nullptr);
    connect(this, &apiManager::jsonReceived, this, [this](const QJsonObject& json) {
        qDebug() << "JSON reçu:" << json;

        QString playlists = json["playlists"].toString();
        QString playlistsUrl = getRessource("files",fileLink,playlists);
        downloadFile(playlistsUrl,"playlists.json");

    });

    // Envoyer une requête POST avec du JSON
    sendGetRequest(apiurl+"/api/collections/files/records/"+fileLink);
}


void apiManager::downloadFile(const QString& url, const QString& filename) {
    QNetworkAccessManager networkManagerb;
    QString ph = fm->userDataDir + "/" + filename;

    if (QFile::exists(ph)) {
        qDebug("deleting existing playlists.json...");
        QFile::remove(ph);
    }

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    QNetworkReply* reply = networkManagerb.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QFile file(ph);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            qDebug("Download playlists.json finished");
            emit downloadFinished();
        }
    } else {
        qDebug() << "Failed to download the file:" << reply->errorString();
    }
    reply->deleteLater();
}

void apiManager::uploadFile(const QString& filePath, const QString& collection, const QString& recordId, const QString& fieldName,const QString& authToken) {
    QNetworkAccessManager* networkManagerb = new QNetworkAccessManager;

    // URL cible (remplace l'adresse si besoin)
    QString url = QString(apiurl+"/api/collections/%1/records/%2").arg(collection,recordId);

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
    if (token!="") {request.setRawHeader(QByteArray("Authorization"), QByteArray::fromStdString(token.toStdString()));}
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
            QMessageBox messageBox;
            messageBox.critical(0,"Upload Error","An error has occured during upload.\nPlease try again, or restart the app.");
            //messageBox.setFixedSize(500,200);
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
    if (token!="") {request.setRawHeader(QByteArray("Authorization"), QByteArray::fromStdString(token.toStdString()));}


    // Envoyer une requête GET
    networkManager.get(request);
}

void apiManager::sendPostRequest(const QString& url, const QJsonObject& json) {
    QUrl qurl(url);
    QNetworkRequest request(qurl);

    // Définir l'en-tête HTTP pour indiquer que le corps est au format JSON
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (token!="") {request.setRawHeader(QByteArray("Authorization"), QByteArray::fromStdString(token.toStdString()));}

    // Convertir le JSON en QByteArray
    QJsonDocument jsonDoc(json);
    QByteArray jsonData = jsonDoc.toJson();

    // Envoyer une requête POST avec le JSON
    networkManager.post(request, jsonData);
}

QString apiManager::sendPatchRequest(const QString& url, const QJsonObject& json) {
    QNetworkAccessManager* networkManagerb = new QNetworkAccessManager;
    QUrl qurl(url);
    QNetworkRequest request(qurl);

    // Définir l'en-tête HTTP pour indiquer que le corps est au format JSON
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (token!="") {request.setRawHeader(QByteArray("Authorization"), QByteArray::fromStdString(token.toStdString()));}


    // Convertir le JSON en QByteArray
    QJsonDocument jsonDoc(json);
    QByteArray jsonData = jsonDoc.toJson();

    // Envoyer une requête POST avec le JSON
    QNetworkReply* reply = networkManagerb->sendCustomRequest(request, "PATCH", jsonData);
    QObject::connect(reply, &QNetworkReply::finished, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            return QString(reply->readAll());
        } else {
            return QString("");        }
        reply->deleteLater();
    });
    networkManagerb->deleteLater();
}





void apiManager::onReplyFinished(QNetworkReply* reply) {
    // Vérifie les erreurs
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "API request error:" << reply->errorString();
        reply->deleteLater();
        QJsonObject err;
        emit jsonReceived(err);
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
        qDebug()<<response;
    }

    // Libérer la mémoire
    reply->deleteLater();
}
