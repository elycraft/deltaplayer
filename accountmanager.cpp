#include "accountmanager.h"
#include "GUI_BASE.h"
#include "./ui_GUI_BASE.h"


accountManager::accountManager( MainWindowt* windowin) {

    window = windowin;
    il = new ImageLoader();

    api = new apiManager();
    connect(window->ui->loginToAcc, &QPushButton::clicked, this, &accountManager::handleLogin);
    connect(window->ui->label_user_icon, &QPushButton::clicked, this, &accountManager::goPage);

}

void accountManager::goPage() {
    if (api->isLogIn) {
        window->ui->stackedWidget->setCurrentWidget(window->ui->page_myaccount);

    } else {
        window->ui->stackedWidget->setCurrentWidget(window->ui->page_noaccount);

    }
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
            window->ui->accountName->setText(api->name);

        }

    });
    qInfo("Trying to login...");
    api->login(window->ui->loginEmail->text(),window->ui->loginPass->text());
}
