#include "cmdworker.h"
#include <qmessagebox.h>

bool showInfo(QString msg) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Info");
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Question);

    // Ajouter les boutons Oui et Non
    QPushButton* yesButton = msgBox.addButton(QMessageBox::Yes);
    QPushButton* noButton = msgBox.addButton(QMessageBox::No);

    // Optionnel : définir le bouton par défaut
    msgBox.setDefaultButton(yesButton);

    // Afficher la boîte de dialogue et attendre la réponse de l'utilisateur
    msgBox.exec();

    // Vérifier quel bouton a été cliqué
    qWarning("Error in confirmation");
    return false;
}

cmdWorker::cmdWorker() {

}

std::wstring cmdWorker::stringToWstring(const std::string& str) {

}
void cmdWorker::execAndCaptureOutput(const std::string& cmd) {
    QString url = QString::fromStdString(cmd);
    QJniObject ytDlp = QJniObject::callStaticObjectMethod(
        "com/yausername/youtubedl_android/YoutubeDL", // Classe Java
        "getInstance",                       // Méthode statique
        "()Lcom/yausername/youtubedl_android/YoutubeDL;" // Signature JNI
        );

    if (!ytDlp.isValid()) {
        qDebug() << "Impossible d'initialiser YoutubeDL";
        return;
    }

    // Appeler la méthode getInfo() pour obtenir les informations de la vidéo
    QJniObject videoInfo = ytDlp.callObjectMethod(
        "getInfo",                                    // Nom de la méthode
        "(Ljava/lang/String;)Lcom/yausername/youtubedl_android/VideoInfo;", // Signature JNI
        QJniObject::fromString(url).object()   // URL de la vidéo
        );

    if (videoInfo.isValid()) {
        // Extraire le titre de la vidéo à partir de l'objet VideoInfo
        QJniObject title = videoInfo.callObjectMethod(
            "getTitle",                                  // Méthode pour obtenir le titre
            "()Ljava/lang/String;"                       // Signature JNI
            );

        if (title.isValid()) {
            QString videoTitle = title.toString();
            showInfo(videoTitle);
            qDebug() << "Titre de la vidéo : " << videoTitle;
        } else {
            showInfo("Impossible d'obtenir le titre de la vidéo");
            qDebug() << "Impossible d'obtenir le titre de la vidéo";
        }
    } else {
        showInfo("Impossible d'obtenir le titre de la vidéo 2");
        qDebug() << "Impossible d'obtenir les informations de la vidéo";
    }

}
