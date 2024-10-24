#include "yt_playlist.h"

yt_playlist::yt_playlist(QString ytDlpPathin) {
    ytDlpPath = ytDlpPathin.toStdString();
}

QString yt_playlist::get_info(QString furl) {
    std::string getCommand = ytDlpPath + " --no-warnings --dump-json " + furl.toStdString();
    cmdWorker* worker = new cmdWorker();
    QThread* thread = new QThread();
    QEventLoop loop;

    worker->moveToThread(thread);

    // Variable pour stocker l'URL
    QString resultUrl;

    // Connecter pour exécuter la commande quand le thread commence
    QObject::connect(thread, &QThread::started, [=]() {
        worker->execAndCaptureOutput(getCommand);  // Appeler execAndCaptureOutput avec l'argument
    });

    // Connecter pour traiter le résultat quand il est prêt
    QObject::connect(worker, &cmdWorker::resultReady, [&](const QString& result) {
        QString infos = result;
        QJsonDocument doc = QJsonDocument::fromJson(infos.toStdString().c_str());
        QJsonObject video = doc.object();
        qInfo()<<video;


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
    return resultUrl;
}
