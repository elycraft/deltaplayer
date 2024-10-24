#include "yt_music.h"
#include <map>
#include <qeventloop.h>
#include <qthread.h>
#include <windows.h>
#include <iostream>
#include <string>


yt_music::yt_music(QString ytDlpPathin, QString urlin, std::map<QString, QString> data) {
    ytDlpPath = ytDlpPathin.toStdString();

    if (data.size() != 0) {
        id = data["id"];
        furl = data["furl"];
        title = data["title"];
        duration = data["length"].toInt();
        thumb = data["thumb"];
        author = data["author"];
    } else {
        /*
        std::string getCommand = ytDlpPath + " --no-warnings --dump-json " + urlin.toStdString();
        //qInfo() << getCommand;
        cmdWorker* worker = new cmdWorker();
        QThread* thread = new QThread();

        worker->moveToThread(thread);

        QObject::connect(thread, &QThread::started, [=]() {
            worker->execAndCaptureOutput(getCommand);  // Appeler doWork avec l'argument
        });

        QObject::connect(worker, &cmdWorker::resultReady, [this, urlin](const QString& result){
            infos = result;
            QJsonDocument doc = QJsonDocument::fromJson(infos.toStdString().c_str());
            QJsonObject video = doc.object();
            //qInfo("jj");
            //qInfo() << video;

            id = video["id"].toString();
            furl = urlin;
            title = video["title"].toString();
            duration = video["duration"].toInt();
            //qInfo()<<video["duration"].toString();

            QJsonArray thumbnailsArray = video["thumbnails"].toArray();
            if (thumbnailsArray.size() > 1) {
                thumb = thumbnailsArray[thumbnailsArray.size() - 2].toObject()["url"].toString();
            } else {
                thumb = "";  // Ou une valeur par défaut si nécessaire
            }

            author = video["uploader"].toString();
        });

        QObject::connect(worker, &cmdWorker::resultReady, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, worker, &QObject::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

        thread->start();

        ///////////////////////

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
            infos = result;
            QJsonDocument doc = QJsonDocument::fromJson(infos.toStdString().c_str());
            QJsonObject video = doc.object();
            //qInfo("jj");
            //qInfo() << video;

            id = video["id"].toString();
            furl = urlin;
            title = video["title"].toString();
            duration = video["duration"].toInt();
            //qInfo()<<video["duration"].toString();

            QJsonArray thumbnailsArray = video["thumbnails"].toArray();
            if (thumbnailsArray.size() > 1) {
                thumb = thumbnailsArray[thumbnailsArray.size() - 2].toObject()["url"].toString();
            } else {
                thumb = "";  // Ou une valeur par défaut si nécessaire
            }

            author = video["uploader"].toString();
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
        */
        qInfo("ERROR: no data");
    }
}

QString yt_music::print() {
    QString a = "id:" + id + " furl:" + furl + " title:" + title + " duration:" + QString::number(duration) + " thumb:" + thumb + " author:" + author;
    return a;
}

QString yt_music::get_url() {
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
        infos = result;
        QJsonDocument doc = QJsonDocument::fromJson(infos.toStdString().c_str());
        QJsonObject video = doc.object();

        QJsonArray allstreams = video["formats"].toArray();
        QJsonArray audiostreams;

        for (const QJsonValue& streamValue : allstreams) {
            QJsonObject stream = streamValue.toObject();
            QString acodec = stream["acodec"].toString();
            QString vcodec = stream["vcodec"].toString();

            if (acodec != "none" && vcodec == "none") {
                audiostreams.append(stream);
            }
        }

        QJsonObject firstAudioStream;
        if (!audiostreams.isEmpty()) {
            firstAudioStream = audiostreams[0].toObject();
        }

        resultUrl = firstAudioStream["url"].toString();
        title = video["title"].toString();
        duration = video["duration"].toInt();

        QJsonArray thumbnailsArray = video["thumbnails"].toArray();
        if (thumbnailsArray.size() > 1) {
            thumb = thumbnailsArray[thumbnailsArray.size() - 2].toObject()["url"].toString();
        } else {
            thumb = "";  // Ou une valeur par défaut si nécessaire
        }

        author = video["uploader"].toString();

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


/*
std::wstring yt_music::stringToWstring(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string yt_music::execAndCaptureOutput(const std::string& cmd) {
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;
    if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0)) {
        std::cerr << "StdoutRd CreatePipe" << std::endl;
        return "";
    }

    if (!SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
        std::cerr << "Stdout SetHandleInformation" << std::endl;
        return "";
    }

    PROCESS_INFORMATION piProcInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    STARTUPINFO siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    siStartInfo.wShowWindow = SW_HIDE;

    // Conversion de std::string en std::wstring pour cmd
    std::wstring wcmd = stringToWstring(cmd);

    // Utilisation de CreateProcessW pour les chaînes de caractères larges
    if (!CreateProcessW(NULL, const_cast<LPWSTR>(wcmd.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo)) {
        std::cerr << "CreateProcess "+cmd << std::endl;
        return "";
    }

    CloseHandle(hChildStd_OUT_Wr);

    const int bufferSize = 4096;
    char buffer[bufferSize];
    DWORD dwRead;
    std::string output;

    while (ReadFile(hChildStd_OUT_Rd, buffer, bufferSize, &dwRead, NULL) && dwRead > 0) {
        output.append(buffer, dwRead);
    }

    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    return output;
}
*/

QJsonObject yt_music::save() {
    QJsonObject jsonObject;
    jsonObject["id"] = id;
    jsonObject["furl"] = furl;
    jsonObject["title"] = title;
    jsonObject["length"] = QString::number(duration);
    jsonObject["thumb"] = thumb;
    jsonObject["author"] = author;

    return jsonObject;
}


