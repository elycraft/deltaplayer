#include "playlist_item.h"
#include "playlist_manager.h"
#include "GUI_BASE.h"
#include "./ui_GUI_BASE.h"

bool askUserConfirmation() {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Confirmation");
    msgBox.setText("Are you sure you want to proceed?");
    msgBox.setIcon(QMessageBox::Question);

    // Ajouter les boutons Oui et Non
    QPushButton* yesButton = msgBox.addButton(QMessageBox::Yes);
    QPushButton* noButton = msgBox.addButton(QMessageBox::No);

    // Optionnel : définir le bouton par défaut
    msgBox.setDefaultButton(yesButton);

    // Afficher la boîte de dialogue et attendre la réponse de l'utilisateur
    msgBox.exec();

    // Vérifier quel bouton a été cliqué
    if (msgBox.clickedButton() == yesButton) {
        // Code à exécuter si l'utilisateur clique sur Oui
        return true;
    } else if (msgBox.clickedButton() == noButton) {
        // Code à exécuter si l'utilisateur clique sur Non
        return false;
    }
    qWarning("Error in confirmation");
    return false;
}

QList<QJsonObject> jsonArrayToList(const QJsonArray& array) {
    QList<QJsonObject> list;
    for (const QJsonValue& value : array) {
        if (value.isObject()) {
            list.append(value.toObject());
        }
    }
    return list;
}

playlist_item::playlist_item(QString namein,QJsonArray musicsin,QString thumbin, QString plidin,QGridLayout* layoutin, QScrollArea* gridFramein, dp_audioapi* mpin, MainWindowt* windowin,playlist_manager* parentin ,bool doShufflein) {
    nameI = namein;
    raw_musics = musicsin;
    parent = parentin;
    ytPath=parent->ytPath;

    plid = plidin;
    layout = layoutin;
    gridFrame = gridFramein;
    mp = mpin;
    window = windowin;

    doShuffle = doShufflein;
    fm = new FileManager();
    il = new ImageLoader();
    thumbI = thumbin;
    thumbUrl = il->get(thumbI);

    musicsI = getRealPlaylist(musicsin);




}

void playlist_item::create() {
    PlaylistTemplate = new QFrame(gridFrame);
    PlaylistTemplate->setMaximumSize(QSize(150, 150));
    PlaylistTemplate->setMinimumSize(QSize(150, 150));

    QString style = QString(
                        "#PlaylistTemplate {"
                        "  background-color: rgb(39, 44, 54);"
                        "  border-radius: 15px;"
                        "  border-image: url('%1') 0 0 0 0 stretch stretch;"
                        "  background-repeat: no-repeat;"
                        "  border: none;"
                        "}"
                        ).arg(thumbUrl);

    PlaylistTemplate->setStyleSheet(style);
    PlaylistTemplate->setObjectName("PlaylistTemplate");

    verticalLayout_13 = new QVBoxLayout(PlaylistTemplate);
    verticalLayout_13->setObjectName("verticalLayout_13");

    thumb = new QPushButton(PlaylistTemplate);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHeightForWidth(thumb->sizePolicy().hasHeightForWidth());
    thumb->setSizePolicy(sizePolicy);
    thumb->setStyleSheet("QPushButton {"
                               "    border: none;"
                               "    background-color: rgba(27, 29, 35,0);"
                               "}");
    thumb->setText("");
    thumb->setObjectName("thumb");
    verticalLayout_13->addWidget(thumb);

    options = new QFrame(PlaylistTemplate);
    options->setStyleSheet("background-color: rgba(0, 0, 0, 200);"
                                 "border-radius: 5px;");
    options->setObjectName("options");

    optionLayout = new QHBoxLayout(options);
    optionLayout->setObjectName("optionLayout");

    plname = new QLabel(options);
    QFont font;
    font.setPointSize(8);
    font.setBold(true);
    plname->setFont(font);
    plname->setTextFormat(Qt::AutoText);
    plname->setScaledContents(false);
    plname->setIndent(-1);
    plname->setObjectName("plname");
    optionLayout->addWidget(plname);

    modifyPlaylist = new QPushButton(options);
    modifyPlaylist->setMaximumSize(QSize(20, 16777215));
    modifyPlaylist->setStyleSheet("QPushButton {"
                                        "    background-position: center;"
                                        "    background-repeat: no-repeat;"
                                        "    border: none;"
                                        "    background-image: url(:/16x16/icons/16x16/cil-media-play.png);"
                                        "}");
    modifyPlaylist->setText("");
    modifyPlaylist->setObjectName("modifyPlaylist");
    optionLayout->addWidget(modifyPlaylist);

    verticalLayout_13->addWidget(options);

    pos* mypos = parent->getNewPos();
    layout->addWidget(PlaylistTemplate,mypos->x,mypos->y); // Implémentez cette méthode pour obtenir une nouvelle position
    //static_cast<QGridLayout*>(parentWidget()->layout())->addWidget(PlaylistTemplate, pos.y(), pos.x());
    connect(thumb, &QPushButton::clicked, this, &playlist_item::modify);
    connect(modifyPlaylist, &QPushButton::clicked, this, &playlist_item::play);
    plname->setText(nameI);
}

QList<yt_music*> playlist_item::getRealPlaylist(const QJsonArray& fromjson) {
    QList<yt_music*> result;
    for (const QJsonValue& value : fromjson) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();

            // Convertir QJsonObject en std::map<QString, QString>
            std::map<QString, QString> data;
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                data[it.key()] = it.value().toString();
            }
            yt_music* idwin = new yt_music(ytPath, "", data);

            // Créer un objet yt_music avec data et un urlin vide
            result.append(idwin);
        }
    }
    return result;
}

void playlist_item::play() {
    mp->add_and_play(musicsI);
}

void playlist_item::autoplay() {
    if (doShuffle) {shuffle();}
    else {play();}
}

void playlist_item::shuffle() {
    QList<yt_music*> copmusics = musicsI;
    qInfo() << copmusics;
    std::random_shuffle(copmusics.begin(), copmusics.begin());
    qInfo() << copmusics;
    mp->add_and_play(copmusics);
}

void playlist_item::modify() {
    try {

        disconnect(window->ui->playlistEditName, &QLineEdit::textChanged, nullptr, nullptr);
        disconnect(window->ui->playlistEditChangeImage, &QPushButton::clicked, nullptr, nullptr);
        disconnect(window->ui->playlistEditDelete, &QPushButton::clicked, nullptr, nullptr);
        disconnect(parent->playlistEditList->model(), &QAbstractItemModel::rowsMoved, nullptr, nullptr);
        disconnect(window->ui->btn_peplay, &QPushButton::clicked, nullptr, nullptr);
        disconnect(parent->playlistEditList,  &CustomListWidget::OnButton1, nullptr, nullptr);
        disconnect(parent->playlistEditList,  &CustomListWidget::OnButton2, nullptr, nullptr);
        disconnect(window->ui->btn_pepshuffle, &QPushButton::clicked, nullptr, nullptr);
        disconnect(window->ui->checkBox_2, &QCheckBox::stateChanged, nullptr, nullptr);
        disconnect(window->ui->serachVideo, &QPushButton::clicked, nullptr, nullptr);
        disconnect(window->ui->searchEdit, &QLineEdit::returnPressed, nullptr, nullptr);
        disconnect(parent->serachVL, &CustomListWidget::OnButton1, nullptr, nullptr);
        disconnect(window->ui->playlistEditAdd, &QPushButton::clicked, nullptr, nullptr);
    } catch (...) {
        // Gérer les exceptions si nécessaire
    }

    // Connexion des signaux aux slots

    connect(window->ui->playlistEditName, &QLineEdit::textChanged, this, &playlist_item::getNewName);
    connect(window->ui->playlistEditChangeImage, &QPushButton::clicked, this, &playlist_item::handlerCI);
    connect(window->ui->playlistEditDelete, &QPushButton::clicked, this, &playlist_item::deletePlaylist);
    connect(parent->playlistEditList->model(), &QAbstractItemModel::rowsMoved , this, &playlist_item::updateList);
    connect(window->ui->btn_peplay, &QPushButton::clicked, this, &playlist_item::play);
    connect(parent->playlistEditList, &CustomListWidget::OnButton1, this, &playlist_item::playAt);
    connect(parent->playlistEditList, &CustomListWidget::OnButton2, this, &playlist_item::deleteVid);
    connect(window->ui->btn_pepshuffle, &QPushButton::clicked, this, &playlist_item::shuffle);
    connect(window->ui->checkBox_2, &QCheckBox::stateChanged, this, &playlist_item::handleShuffle);
    connect(window->ui->serachVideo, &QPushButton::clicked, this, &playlist_item::searchAVideo);
    connect(window->ui->searchEdit, &QLineEdit::returnPressed, this, &playlist_item::searchAVideo);
    connect(parent->serachVL, &CustomListWidget::OnButton1, this, &playlist_item::addNewVideo);
    connect(window->ui->playlistEditAdd, &QPushButton::clicked, this, &playlist_item::handlerNM);

    // Mise à jour de l'interface utilisateur
    //window->ui->serachVL->clear();
    window->ui->searchEdit->clear();
    parent->playlistEditList->clear();
    window->ui->playlistEditName->setText(nameI);
    window->ui->checkBox_2->setChecked(doShuffle);

    QString styleSheet = QString("border-radius: 15px; border-image: url(\"%1\") 0 0 0 0 stretch stretch")
                             .arg(thumbUrl);
    window->ui->playlistEditImage->setStyleSheet(styleSheet);
    qRegisterMetaType<yt_music>("yt_music");

    for (const auto& music : musicsI) {
        QString texteametre;
        int maxcara = 60;
        QString a = QString("    %1 - By %2").arg(music->title).arg(music->author);
        if (a.length() > maxcara) {
            int tm = maxcara - (a.length() - music->title.length());
            QString shortText = music->title.left(tm) + "...";
            texteametre = QString("    %1 - By %2").arg(shortText).arg(music->author);
        } else {
            texteametre = QString("    %1 - By %2").arg(music->title).arg(music->author);
        }
        //qRegisterMetaType<yt_music>("yt_music");
        // Encapsulation des données dans un QVariant contenant une QList<QVariant>
        QVariant data = QVariant::fromValue(QList<QVariant>{
            QVariant::fromValue(QString(":/16x16/icons/16x16/cil-media-play.png")),
            QVariant::fromValue(QString(":/20x20/icons/20x20/cil-x-circle.png")),
            QVariant::fromValue(QString("")),
            QVariant::fromValue(texteametre),
            QVariant::fromValue(il->get(music->thumb)),
            QVariant::fromValue(QString("")),
            QVariant::fromValue(music)
        });

        QListWidgetItem* item = new QListWidgetItem();
        item->setData(Qt::UserRole, data);
        parent->playlistEditList->addItem(item);

    }
    window->ui->stackedWidget->setCurrentWidget(window->ui->page_editPlaylist);
}

void playlist_item::updateList() {
    musicsI = parent->playlistEditList->getNewList();
}

void playlist_item::playAt(QListWidgetItem* wo) {
    int row = parent->playlistEditList->row(wo);
    mp->add_and_play(musicsI.mid(row));
}

void playlist_item::getNewName(const QString& text) {
    nameI = text;
    plname->setText(text);
    //save();
}

void playlist_item::handleShuffle() {
    doShuffle = window->ui->checkBox_2->isChecked();
    //qInfo() << doShuffle;
}

void playlist_item::handlerCI() {
    bool ok;
    QString name = QInputDialog::getText(
        window->ui->frame_main,
        tr("Deltaplayer Dialog"),
        tr("Please give an image URL for the new thumbnail:"),
        QLineEdit::Normal,
        QString(),
        &ok
        );

    if (ok) {
        getNewImage(name);
    }
}


void playlist_item::handlerNM() {
    bool ok;
    QString name = QInputDialog::getText(
        window->ui->frame_main,
        tr("Deltaplayer Dialog"),
        tr("Please give a Youtube Playlist url to import (This can took a while !) : "),
        QLineEdit::Normal,
        QString(),
        &ok
        );

    if (ok) {
        //getNewImage(name);
    }
}

void playlist_item::getNewImage(QString text) {

    thumbI = text;
    thumbUrl = il->get(thumbI);
    PlaylistTemplate->setStyleSheet(QString(
                                            "#PlaylistTemplate {\n"
                                            "  background-color: rgb(39, 44, 54);\n"
                                            "  border-radius: 15px;\n"
                                            "  border-image: url(\"%1\") 0 0 0 0 stretch stretch;\n"
                                            "  background-repeat: no-repeat;\n"
                                            "  border: none;\n"
                                            "}"
                                            ).arg(thumbUrl));
    window->ui->playlistEditImage->setStyleSheet(QString(
                                                 "border-radius: 15px;\n"
                                                 "border-image: url(\"%1\") 0 0 0 0 stretch stretch"
                                                 ).arg(thumbUrl));
}

void playlist_item::deletePlaylist() {
    if (!askUserConfirmation()) {return;}
    // Retirer PlaylistTemplate de son parent
    this->PlaylistTemplate->setParent(nullptr);

    // Changer le widget courant du stackedWidget
    window->ui->stackedWidget->setCurrentWidget(window->ui->page_playlists);

    // Retirer l'élément de la liste des playlists
    auto& playlists = parent->playlists; // Assurez-vous que 'playlists' est bien défini et accessible
    playlists.removeOne(this);

    // Supprimer l'objet
    delete this;
}

void playlist_item::searchAVideo() {
    //window->ui->serachVL->clear();
    qInfo("Start query");

    // Obtenez les termes de recherche depuis le QLineEdit
    QString searchQuery = window->ui->searchEdit->text();

    // Instanciez la classe YoutubeSearch avec les termes de recherche et un nombre maximal de résultats
    YoutubeSearch *search =  new YoutubeSearch(searchQuery, 5);
    connect(search, &YoutubeSearch::onComplete, this, &playlist_item::showSearch);
       // Recherche avec un maximum de 5 résultats
    search->search();
    // Récupérez les résultats en format dictionnaire


        //qDebug() << "Results in JSON format:" << search->toJson();// Assurez-vous que `toDict()` retourne les données au bon format

    // Traitez les résultats comme vous le souhaitez
    //delete search;
}

void playlist_item::showSearch(YoutubeSearch* sh) {
    disconnect(sh, &YoutubeSearch::onComplete, this, &playlist_item::showSearch);
    //Debug() << "Results in dictionary format:" << sh->toDict();
    parent->serachVL->clear();

    // Exécuter la recherche YouTube et récupérer les résultats

    for (auto v : sh->toDict()) {
        QJsonObject video = v.toObject();

        QString thumbnails = video["thumbnails"].toArray()[0].toString();
        QString tit = video["title"].toString();
        QString channel = video["channel"].toString();
        QString ideh = video["id"].toString();


        QString texteametre;
        int maxcara = 60;
        QString a = QString("    %1 - By %2").arg(tit).arg(channel);
        if (a.length() > maxcara) {
            int tm = maxcara - (a.length() - tit.length());
            QString shortText = tit.left(tm) + "...";
            texteametre = QString("    %1 - By %2").arg(shortText).arg(channel);
        } else {
            texteametre = QString("    %1 - By %2").arg(tit).arg(channel);
        }

        QVariant data = QVariant::fromValue(QList<QVariant>{
            QVariant::fromValue(QString(":/16x16/icons/16x16/cil-plus.png")),
            QVariant::fromValue(QString("")),
            QVariant::fromValue(QString("")),
            QVariant::fromValue(QString(texteametre)),
            QVariant::fromValue(il->get(thumbnails)),
            QVariant::fromValue(QString(ideh)),
        });

        QListWidgetItem* itembis = new QListWidgetItem();
        itembis->setData(Qt::UserRole, data);
        parent->serachVL->addItem(itembis);
    }
}

void playlist_item::addNewVideo(QListWidgetItem* wo) {
    QVariant data = wo->data(Qt::UserRole);
    QList<QVariant> dataList = data.toList();
    if (wo && data.isValid()) {//if (item && !itemWidget(item) && data.isValid()) {
        QString id = dataList[5].toString();
        std::map<QString, QString> dataoo;
        yt_music *music = new yt_music(ytPath,"https://www.youtube.com/watch?v="+id,dataoo);
        musicsI.append(music);
        QString texteametre;
        int maxcara = 60;
        QString a = QString("    %1 - By %2").arg(music->title).arg(music->author);
        if (a.length() > maxcara) {
            int tm = maxcara - (a.length() - music->title.length());
            QString shortText = music->title.left(tm) + "...";
            texteametre = QString("    %1 - By %2").arg(shortText).arg(music->author);
        } else {
            texteametre = QString("    %1 - By %2").arg(music->title).arg(music->author);
        }
        //qRegisterMetaType<yt_music>("yt_music");
        // Encapsulation des données dans un QVariant contenant une QList<QVariant>
        QVariant data = QVariant::fromValue(QList<QVariant>{
            QVariant::fromValue(QString(":/16x16/icons/16x16/cil-media-play.png")),
            QVariant::fromValue(QString(":/20x20/icons/20x20/cil-x-circle.png")),
            QVariant::fromValue(QString("")),
            QVariant::fromValue(texteametre),
            QVariant::fromValue(il->get(music->thumb)),
            QVariant::fromValue(QString("")),
            QVariant::fromValue(music)
        });

        QListWidgetItem* item = new QListWidgetItem();
        item->setData(Qt::UserRole, data);
        parent->playlistEditList->addItem(item);
    }
}

void playlist_item::deleteVid(QListWidgetItem* wo) {
    qInfo("aaa");
    int row = parent->playlistEditList->row(wo);
    parent->playlistEditList->takeItem(row);
    musicsI = parent->playlistEditList->getNewList();
}


QJsonObject playlist_item::save() {
    QJsonArray musicsArray;
    for (const auto& music : musicsI) {
        musicsArray.append(music->save());
    }

    QJsonObject jsonObject;
    jsonObject["name"] = nameI;
    jsonObject["musics"] = musicsArray;
    jsonObject["thumb"] = thumbI;
    jsonObject["plid"] = plid;
    jsonObject["doShuffle"] = doShuffle;

    return jsonObject;
}

