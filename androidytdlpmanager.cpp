#include "androidytdlpmanager.h"

bool showInfo(QString msg) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Info");
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Question);

    // Ajouter les boutons Oui et Non
    QPushButton* yesButton = msgBox.addButton(QMessageBox::Ok);

    // Optionnel : définir le bouton par défaut
    msgBox.setDefaultButton(yesButton);

    // Afficher la boîte de dialogue et attendre la réponse de l'utilisateur
    msgBox.exec();

    // Vérifier quel bouton a été cliqué
    return true;
}

androidYtdlpManager::androidYtdlpManager() {

}

void androidYtdlpManager::init() {
    // Récupérer l'instance unique de YoutubeDL
    ytDlpInstance = QJniObject::callStaticObjectMethod(
        "com/yausername/youtubedl_android/YoutubeDL", // Nom de la classe Java
        "getInstance",                               // Méthode statique
        "()Lcom/yausername/youtubedl_android/YoutubeDL;" // Signature JNI
        );

    if (!ytDlpInstance.isValid()) {
        qDebug() << "Impossible de récupérer l'instance de YoutubeDL";
        return;
    }

    // Appeler la méthode init(Context)
    QJniObject context = QNativeInterface::QAndroidApplication::context();
    if (context.isValid()) {
        ytDlpInstance.callMethod<void>(
            "init",                                  // Méthode d'instance
            "(Landroid/content/Context;)V",         // Signature JNI
            context.object()                        // Paramètre : le contexte
            );
    } else {
        qDebug() << "Contexte Android invalide.";
        return;
    }

}

void androidYtdlpManager::get_info(QString url) {

    // Préparer l'URL en tant que String Java
    QJniObject jniUrl = QJniObject::fromString(url);


    // Appeler la méthode getInfo(String)
    //QJniObject aaa = QJniObject("com/yausername/youtubedl_android/YoutubeDLRequest");
    QJniObject request(
        "com/yausername/youtubedl_android/YoutubeDLRequest",
        "(Ljava/lang/String;)V",
        jniUrl.object()
        );

    QJniObject result1 = request.callObjectMethod( // Call on 'result'
        "addOption",
        "(Ljava/lang/String;)Lcom/yausername/youtubedl_android/YoutubeDLRequest;",
        QJniObject::fromString("--no-warnings").object()
        );

    QJniObject result2 = result1.callObjectMethod( // Call on 'result'
        "addOption",
        "(Ljava/lang/String;)Lcom/yausername/youtubedl_android/YoutubeDLRequest;",
        QJniObject::fromString("--dump-json").object()
        );

    if (!ytDlpInstance.isValid()) {
        qDebug() << "Impossible de récupérer l'instance de YoutubeDL";
        return;
    }

    QJniObject responseObject = ytDlpInstance.callObjectMethod(
        "execute",
        "(Lcom/yausername/youtubedl_android/YoutubeDLRequest;Ljava/lang/String;Lkotlin/jvm/functions/Function3;)Lcom/yausername/youtubedl_android/YoutubeDLResponse;",
        result2.object(),
        nullptr, // nullptr si processIdObject n'est pas valide
        nullptr // nullptr si callbackObject n'est pas valide.
        );

    if (!responseObject.isValid()) {
        qDebug() << "Erreur : l'appel à execute a échoué. Impossible d'accéder aux champs.";
        // Gérer l'erreur (par exemple, retourner ou lancer une exception)
        return;
    }

    QJniObject outField = responseObject.getObjectField("out","Ljava/lang/String;");

    if (!outField.isValid()) {
        qDebug() << "Erreur : impossible d'accéder au champ 'out'";
        return;
    }
    QString finalResponse = outField.toString();
    showInfo(finalResponse);

}


