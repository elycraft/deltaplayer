#include "SettingManager.h"
#include "ui_GUI_BASE.h"

SettingManager::SettingManager(MainWindowt *mainWindow) {
    BASESETTINGS = {{"volume", 50},{"ytpath",QDir::currentPath()+"/yt-dlp.exe"}};
    fm = new FileManager();
    settings = load();
    window = mainWindow;

    window->ui->pathToYt->setText(getSetting("ytpath").toString());


    connect(window->ui->pathToYt, &QLineEdit::textChanged, [this]() { SettingManager::setSetting("ytpath",window->ui->pathToYt->text()); });
    connect(window->ui->soundSlider, &QSlider::valueChanged, [this]() { SettingManager::setSetting("volume",window->ui->soundSlider->value()); });
}

void SettingManager::save() {
    QFile file(fm->userDataDir + "/settings.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(settings);
        file.write(doc.toJson());
        file.close();
    } else {
        qWarning() << "Impossible de sauvegarder les paramètres.";
    }
}

QJsonObject SettingManager::load() {
    QFile file(fm->userDataDir + "/settings.json");
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        file.close();
        return doc.object();
    } else {
        // Si le fichier n'existe pas, crée un nouveau fichier avec les paramètres de base
        save();
        return BASESETTINGS;
    }
}

QVariant SettingManager::getSetting(const QString& key) {
    if (settings.contains(key)) {
        return settings.value(key).toVariant();  // Convertir en QVariant pour une utilisation flexible
    } else {
        qWarning() << "Le paramètre" << key << "n'existe pas.";
        return QVariant();  // Renvoie une valeur QVariant vide si la clé n'existe pas
    }
}

void SettingManager::setSetting(const QString& key, const QVariant& value) {
    settings[key] = QJsonValue::fromVariant(value);  // Modifie ou ajoute un paramètre
    save();  // Sauvegarde immédiatement après la modification
}

void SettingManager::appExit() {
    qInfo("Exiting Setting Manager");
    save();
}
