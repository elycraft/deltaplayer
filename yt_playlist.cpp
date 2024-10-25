#include "yt_playlist.h"
#include <qeventloop.h>
#include <qthread.h>

yt_playlist::yt_playlist(QString furlin, QString ytDlpPathin) {
    furl = furlin;
    ytDlpPath = ytDlpPathin.toStdString();
}

QJsonArray yt_playlist::get_info() {
    std::string getCommand = ytDlpPath + " --no-warnings --flat-playlist --dump-single-json " + furl.toStdString();
    cmdWorker* worker = new cmdWorker();
    QThread* thread = new QThread();
    QEventLoop loop;

    qInfo("Start Query");

    worker->moveToThread(thread);

    // Variable pour stocker l'URL

    QJsonArray resultat;

    // Connecter pour exécuter la commande quand le thread commence
    QObject::connect(thread, &QThread::started, [=]() {
        worker->execAndCaptureOutput(getCommand);  // Appeler execAndCaptureOutput avec l'argument
    });

    // Connecter pour traiter le résultat quand il est prêt
    QObject::connect(worker, &cmdWorker::resultReady, [&](const QString& result) {
        QJsonObject res;
        QString infos = result;

        QJsonDocument doc = QJsonDocument::fromJson(infos.toStdString().c_str());
        QJsonArray videos = doc.object()["entries"].toArray();
        //qInfo()<<infos;

        for (QJsonValue video : videos) {
            res["id"] = video["id"].toString();
            res["furl"] = "https://www.youtube.com/watch?v="+video["id"].toString();
            res["title"] = video["title"].toString();
            res["duration"] = video["duration"].toInt();
            //qInfo()<<video["duration"].toString();

            QJsonArray thumbnailsArray = video["thumbnails"].toArray();
            if (thumbnailsArray.size() > 1) {
                res["thumb"] = thumbnailsArray[thumbnailsArray.size() - 2].toObject()["url"].toString();
            } else {
                res["thumb"] = "";  // Ou une valeur par défaut si nécessaire
            }

            res["author"] = video["uploader"].toString();
            resultat.append(res);
        }

        // Quitter l'event loop quand l'URL est prête
        loop.quit();
    });

    // Connecter pour fermer et nettoyer le thread
    QObject::connect(worker, &cmdWorker::resultReady, thread, &QThread::quit);
    QObject::connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    // Démarrer le thread
    thread->start();

    // Attendre que le signal soit émis (sans bloquer l'interface utilisateur)
    loop.exec();  // L'exécution est suspendue ici jusqu'à ce que `loop.quit()` soit appelé

    // Retourner l'URL obtenue
    return resultat;
}
