#include "playlist_item.h"
#include "playlist_manager.h"
#include "GUI_BASE.h"
#include "./ui_GUI_BASE.h"

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
    musicsI = getRealPlaylist(musicsin);
    plid = plidin;
    layout = layoutin;
    gridFrame = gridFramein;
    mp = mpin;
    window = windowin;
    parent = parentin;
    fm = new FileManager();
    il = new ImageLoader();

    thumbI = thumbin;
    thumbUrl = il->get(thumbI);

    connect(parent->playlistEditList, &CustomListWidget::hasPlayAt, this, &playlist_item::playAt);

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
            yt_music* idwin = new yt_music("", data);

            // Créer un objet yt_music avec data et un urlin vide
            result.append(idwin);
        }
    }
    return result;
}

void playlist_item::play() {
    mp->add_and_play(musicsI);
}

void playlist_item::modify() {
    try {
        /*
        disconnect(window->ui->playlistEditName, &QLineEdit::textChanged, this, &playlist_item::getNewName);
        disconnect(window->ui->playlistEditList->model(), &QAbstractItemModel::rowsMoved, this, &playlist_item::updateList);
        disconnect(window->ui->playlistEditChangeImage, &QPushButton::clicked, this, &playlist_item::handlerCI);

        disconnect(window->ui->playlistEditDelete, &QPushButton::clicked, this, [=]() { deletePlaylist(); });
        */
        disconnect(window->ui->btn_peplay, &QPushButton::clicked, this, &playlist_item::play);
        /*
        disconnect(window->ui->btn_pepshuffle, &QPushButton::clicked, this, &playlist_item::shuffle);
        disconnect(window->ui->playlistEditList, &QListWidget::itemSelectionChanged, this, &playlist_item::updateList);
        disconnect(window->ui->checkBox_2, &QCheckBox::stateChanged, this, &playlist_item::handleShuffle);
        disconnect(window->ui->serachVideo, &QPushButton::clicked, this, &playlist_item::searchAVideo);
        disconnect(window->ui->searchEdit, &QLineEdit::returnPressed, this, &playlist_item::searchAVideo);
        disconnect(window->ui->playlistEditAdd, &QPushButton::clicked, this, &playlist_item::handlerNM);
        */
    } catch (...) {
        // Gérer les exceptions si nécessaire
    }

    // Connexion des signaux aux slots
    /*
    connect(window->ui->playlistEditName, &QLineEdit::textChanged, this, &playlist_item::getNewName);
    connect(window->ui->playlistEditList->model(), &QAbstractItemModel::rowsMoved, this, &playlist_item::updateList);
    connect(window->ui->playlistEditChangeImage, &QPushButton::clicked, this, &playlist_item::handlerCI);
    connect(window->ui->playlistEditDelete, &QPushButton::clicked, this, [=]() { deletePlaylist(); });
    */
    connect(window->ui->btn_peplay, &QPushButton::clicked, this, &playlist_item::play);

    //connect(window->ui->btn_pepshuffle, &QPushButton::clicked, this, &playlist_item::shuffle);
    //parent->playlistEditList->addListener(updateList,playAt);
    parent->playlistEditList->addListener(
        std::bind(&playlist_item::updateList, this),
        std::bind(&playlist_item::playAt, this, std::placeholders::_1)
        );
    /*
    connect(window->ui->checkBox_2, &QCheckBox::stateChanged, this, &playlist_item::handleShuffle);
    connect(window->ui->serachVideo, &QPushButton::clicked, this, &playlist_item::searchAVideo);
    connect(window->ui->searchEdit, &QLineEdit::returnPressed, this, &playlist_item::searchAVideo);
    connect(window->ui->playlistEditAdd, &QPushButton::clicked, this, &playlist_item::handlerNM);
    */

    // Mise à jour de l'interface utilisateur
    //window->ui->serachVL->clear();
    window->ui->searchEdit->clear();
    parent->playlistEditList->clear();
    window->ui->playlistEditName->setText(nameI);
    window->ui->checkBox_2->setChecked(doShuffle);

    QString styleSheet = QString("border-radius: 15px; border-image: url(\"%1\") 0 0 0 0 stretch stretch")
                             .arg(thumbUrl);
    window->ui->playlistEditImage->setStyleSheet(styleSheet);

    for (const auto& music : musicsI) {
        qRegisterMetaType<yt_music>("yt_music");
        // Encapsulation des données dans un QVariant contenant une QList<QVariant>
        QVariant data = QVariant::fromValue(QList<QVariant>{
            QVariant::fromValue(music),
            QVariant::fromValue(music->title),
            QVariant::fromValue(music->author),
            QVariant::fromValue(il->get(music->thumb))
        });

        // Créez l'élément de la QListWidget et y associez les données encapsulées
        QListWidgetItem* item = new QListWidgetItem();
        item->setData(Qt::UserRole, data);
        QVariant retrievedData = item->data(Qt::UserRole);
        qInfo() << retrievedData;
        parent->playlistEditList->addItem(item);

    }
    window->ui->stackedWidget->setCurrentWidget(window->ui->page_editPlaylist);
}

void playlist_item::updateList() {
    musicsI = parent->playlistEditList->getNewList();
}

void playlist_item::playAt(QListWidgetItem* wo) {
    qInfo() << "dd";
    int row = parent->playlistEditList->row(wo);
    mp->add_and_play(musicsI.mid(row));
}

