#include "accountmanager.h"
#include "GUI_BASE.h"
#include "./ui_GUI_BASE.h"


accountManager::accountManager( MainWindowt* windowin, playlist_manager* playlistM,  SettingManager* settingM) {

    window = windowin;
    fm = new FileManager();
    il = new ImageLoader();
    pm = playlistM;
    sm = settingM;

    window->userIcon("Login", "", true);


    api = new apiManager();

    QString possibleToken = sm->getSetting("loginTkn").toString();
    if (possibleToken!="") {
        connect(api, &apiManager::loginFinished, this,  [this]() {
            if (!api->isLogIn) {return;}
            if (api->avatar == "") {
                window->userIcon(api->name, "", true);
            } else {
                QString avatar = il->get(api->getRessource("users",api->id,api->avatar));
                window->userIcon(api->name, "url("+avatar+")", true);
                QString styleSheet = QString("border-radius: 25px; border-image: url(\"%1\")")
                                         .arg(avatar);
                window->ui->imageAvatar->setStyleSheet(styleSheet);


            }
            window->ui->accountName->setText(api->name);
            sm->setSetting("loginTkn",api->token);
        });
        qInfo("Trying to login...");
        api->authRefresh(possibleToken);

    }


    connect(window->ui->createAcc, &QPushButton::clicked, this, &accountManager::handleCreateAcc);
    connect(window->ui->loginToAcc_2, &QPushButton::clicked, this, &accountManager::handleLogin);
    connect(window->ui->label_user_icon, &QPushButton::clicked, this, &accountManager::goPage);
    connect(window->ui->accountUpload, &QPushButton::clicked, this, &accountManager::pushPlaylists);
    connect(window->ui->accountDownload, &QPushButton::clicked, this, &accountManager::getPlaylists);
    connect(window->ui->goToCreateAcc, &QPushButton::clicked, this, &accountManager::handle_noaccount);
    connect(window->ui->accountLogout, &QPushButton::clicked, this, &accountManager::handle_logout);


}

void accountManager::goPage() {
    if (api->isLogIn) {
        window->ui->stackedWidget->setCurrentWidget(window->ui->page_myaccount);

    } else {
        window->ui->stackedWidget->setCurrentWidget(window->ui->page_login);

    }
}

void accountManager::pushPlaylists() {
    api->uploadFile(QDir(fm->userDataDir).filePath("playlists.json"),"files",api->fileLink,"playlists",api->token);
}

void accountManager::getPlaylists() {
    connect(api, &apiManager::downloadFinished, this,  [this]() {
        pm->raw_playlists = pm->load();

        pm->reload();
        window->ui->stackedWidget->setCurrentWidget(window->ui->page_playlists);

    });

    api->downloadFromServer();
}

void accountManager::handle_logout() {
    api->isLogIn = false;
    api->token = "";
    sm->setSetting("loginTkn","");
    window->ui->stackedWidget->setCurrentWidget(window->ui->page_login);
}

void accountManager::handle_noaccount() {
    window->ui->stackedWidget->setCurrentWidget(window->ui->page_noaccount);
}

void accountManager::handleLogin() {
    connect(api, &apiManager::loginFinished, this,  [this]() {
        if (api->avatar == "") {
            window->userIcon(api->name, "", true);
        } else {
            QString avatar = il->get(api->getRessource("users",api->id,api->avatar));
            window->userIcon(api->name, "url("+avatar+")", true);
            QString styleSheet = QString("border-radius: 25px; border-image: url(\"%1\")")
                                     .arg(avatar);
            window->ui->imageAvatar->setStyleSheet(styleSheet);


        }
        window->ui->accountName->setText(api->name);
        sm->setSetting("loginTkn",api->token);
        goPage();

    });
    qInfo("Trying to login...");
    api->login(window->ui->loginEmail->text(),window->ui->loginPass_2->text());
}

void accountManager::handleCreateAcc() {
    connect(api, &apiManager::createFinished, this,  [this]() {
        if (api->id != "") {
            connect(api, &apiManager::loginFinished, this,  [this]() {
                if (api->avatar == "") {
                    window->userIcon(api->name, "", true);
                } else {
                    QString avatar = il->get(api->getRessource("users",api->id,api->avatar));
                    window->userIcon(api->name, "url("+avatar+")", true);
                    QString styleSheet = QString("border-radius: 25px; border-image: url(\"%1\")")
                                             .arg(avatar);
                    window->ui->imageAvatar->setStyleSheet(styleSheet);



                }
                window->ui->accountName->setText(api->name);
                goPage();

            });
            qInfo("Trying to login...");
            api->login(window->ui->createEmail->text(),window->ui->createPass->text());
        } else {
            qInfo("Cant cteate account");

        }

    });
    qInfo("Trying to create account...");
    api->createAccount(window->ui->createEmail->text(),window->ui->createPass->text(),window->ui->createPseudo->text());
}
