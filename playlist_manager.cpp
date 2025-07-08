#include "playlist_manager.h"
#include "GUI_BASE.h"
#include "./ui_GUI_BASE.h"

pos::pos(int xin, int yin) {
    x = xin;
    y = yin;
}





playlist_manager::playlist_manager(MainWindowt* mainWindow, dp_audioapi* mpin, SettingManager* smin) {

    window = mainWindow;
    auto ui = mainWindow->ui;
    fm = new FileManager();
    il = new ImageLoader(fm);
    mp = mpin;
    sm = smin;
    ytPath = sm->getSetting("ytpath").toString();


    /// Android Add
    /*ui->checkBox_2->setEnabled(false);
    ui->checkBox_2->setVisible(false);
    */
    ui->verticalLayout_17->removeWidget(ui->serachVideoFrame);
    ui->hLayout_15->removeWidget(ui->leftPanelEditPlaylist);

    ui->hLayout_15->removeWidget(ui->rightPanelEditPlaylist);
    //delete ui->rightPanelEditPlaylist;

    // Create a QScrollArea
    scrollAreaNNN = new QScrollArea(ui->page_editPlaylist);
    scrollAreaNNN->setWidgetResizable(true);
    scrollAreaNNN->setStyleSheet("border-radius: 0px;");
    scrollAreaNNN->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    scrollAreaWidgetContentsNNN = new QWidget();
    scrollAreaWidgetContentsNNN->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    scrollAreaWidgetContentsNNN->setAttribute(Qt::WA_AcceptTouchEvents);
    //scrollAreaWidgetContents->setAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents, false);
    //scrollAreaWidgetContentsNNN->setStyleSheet("background-color: red; border: 2px solid black;");


    bigLayout = new QVBoxLayout(scrollAreaWidgetContentsNNN);
    bigLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    scrollAreaNNN->setWidget(scrollAreaWidgetContentsNNN);


    QScroller* scrollerNNN = QScroller::scroller(scrollAreaWidgetContentsNNN); // Utilisation de la méthode statique scroller()
    if (scrollerNNN) {
        //scroller->grabGesture(scrollAreaWidgetContents);
        scrollerNNN->grabGesture(scrollAreaNNN->viewport(), QScroller::LeftMouseButtonGesture);
        // Configuration optionnelle du QScroller
        QScrollerProperties properties = scrollerNNN->scrollerProperties();
        properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
        properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
        properties.setScrollMetric(QScrollerProperties::DecelerationFactor, 0.5); // Ajuster la décélération
        properties.setScrollMetric(QScrollerProperties::FrameRate, QScrollerProperties::Fps60);
        scrollerNNN->setScrollerProperties(properties);

        qDebug() << "QScroller activé";
        qDebug() << "scrollAreaWidgetContents size:" << scrollAreaWidgetContentsNNN->size();
        qDebug() << "scrollArea viewport size:" << scrollAreaNNN->viewport()->size();
    } else {
        qDebug() << "QScroller non supporté sur cette plateforme";
        // Gestion alternative si QScroller n'est pas supporté
    }

    //Important pour le fonctionnement du QScroller
    scrollAreaNNN->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollAreaNNN->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->hLayout_15->addWidget(scrollAreaNNN);

    bigLayout->addWidget(ui->leftPanelEditPlaylist);
    bigLayout->addWidget(ui->serachVideoFrame);



    //////////////////////////////////////////////////////
    playlistEditList = new CustomListWidget(ui->verticalFrame1);
    playlistEditList->setDragDropMode(QAbstractItemView::InternalMove);
    playlistEditList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    playlistEditList->setMaximumSize(1000000,1000000);
    playlistEditList->setSizeAdjustPolicy(CustomListWidget::SizeAdjustPolicy::AdjustToContents);

    // Set stylesheet and object name
    playlistEditList->setStyleSheet("border: 0px");
    playlistEditList->setObjectName("playlistEditList");

    playlistEditList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    playlistEditList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Add CustomListWidget to the vertical layout
    bigLayout->addWidget(playlistEditList);

    // Create a QScrollArea
    scrollArea = new QScrollArea(ui->page_playlists);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("border-radius: 0px;");
    scrollArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    scrollAreaWidgetContents = new QWidget();
    scrollAreaWidgetContents->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    scrollAreaWidgetContents->setAttribute(Qt::WA_AcceptTouchEvents);
    //scrollAreaWidgetContents->setAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents, false);
    //scrollAreaWidgetContents->setStyleSheet("background-color: red; border: 2px solid black;");


    gridLayoutPlay = new QGridLayout(scrollAreaWidgetContents);
    gridLayoutPlay->setSizeConstraint(QLayout::SetMinAndMaxSize);

    scrollArea->setWidget(scrollAreaWidgetContents);

    QScroller *scroller = QScroller::scroller(scrollAreaWidgetContents); // Utilisation de la méthode statique scroller()
    if (scroller) {
        //scroller->grabGesture(scrollAreaWidgetContents);
        scroller->grabGesture(scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
        // Configuration optionnelle du QScroller
        QScrollerProperties properties = scroller->scrollerProperties();
        properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
        properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
        properties.setScrollMetric(QScrollerProperties::DecelerationFactor, 0.1); // Ajuster la décélération
        scroller->setScrollerProperties(properties);
        qDebug() << "QScroller activé";
        qDebug() << "scrollAreaWidgetContents size:" << scrollAreaWidgetContents->size();
        qDebug() << "scrollArea viewport size:" << scrollArea->viewport()->size();
    } else {
        qDebug() << "QScroller non supporté sur cette plateforme";
        // Gestion alternative si QScroller n'est pas supporté
    }

    //Important pour le fonctionnement du QScroller
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->verticalLayout_10->addWidget(scrollArea);

    //connect(scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, scrollArea->viewport(), scrollArea->update());
    //connect(scrollArea->horizontalScrollBar(), &QScrollBar::valueChanged, scrollArea->viewport(), &QWidget::update);

    /////////////////////////////////////////////////////
    ///     //////////////////////////////////////////////////////
    ///

    serachVL = new CustomListWidget(ui->serachVideoFrame);
    //serachVL->setDragDropMode(QAbstractItemView::InternalMove);
    //serachVL->setSelectionMode(QAbstractItemView::ExtendedSelection);
    serachVL->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    serachVL->setMaximumSize(1000000,1000000);
    serachVL->setSizeAdjustPolicy(CustomListWidget::SizeAdjustPolicy::AdjustToContents);

    // Set stylesheet and object name
    serachVL->setStyleSheet("border: 0px; background: transparent");
    serachVL->setObjectName("serachPlaylistEditList");

    serachVL->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    serachVL->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Add CustomListWidget to the vertical layout
    ui->verticalLayout_21->addWidget(serachVL);

    /*
    // Create a QScrollArea
    scrollAreaserachVL = new QScrollArea(serachVL);
    scrollAreaserachVL->setWidgetResizable(true);
    scrollAreaserachVL->setStyleSheet("border-radius: 0px;");

    scrollAreaWidgetContentsserachVL = new QWidget();
    scrollAreaserachVL->setWidget(scrollAreaWidgetContentsserachVL);

    //ui->verticalLayout_10->addWidget(scrollAreaserachVL);  // A CORIGE
    */
    /////////////////////////////////////////////////////



    nextPos = new pos(0,0);

    addBtnAdd();
    raw_playlists = load();
    drawPlaylists();
}



void playlist_manager::addBtnAdd() {
    AddPlaylist = new QFrame(scrollArea);
    AddPlaylist->setMaximumSize(QSize(1500, 150));
    AddPlaylist->setMinimumSize(QSize(150, 150));
    //AddPlaylist->setMaximumSize(150, 150);
    AddPlaylist->setStyleSheet("background-color: rgb(39, 44, 54);\n border-radius: 15px;");
    AddPlaylist->setObjectName("AddPlaylist");

    AddPlaylistLayout = new QVBoxLayout(AddPlaylist);
    AddPlaylistLayout->setObjectName("AddPlaylistLayout");

    btn_addplaylist = new QPushButton(AddPlaylist);
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setWeight(QFont::Weight(75));
    btn_addplaylist->setFont(font);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(btn_addplaylist->sizePolicy().hasHeightForWidth());
    btn_addplaylist->setSizePolicy(sizePolicy);

    btn_addplaylist->setObjectName("btn_addplaylist");
    btn_addplaylist->setText("Add a Playlist");
    AddPlaylistLayout->addWidget(btn_addplaylist);

    gridLayoutPlay->addWidget(AddPlaylist, 0, 0);

    connect(btn_addplaylist, &QPushButton::clicked, this, &playlist_manager::newPlaylist);
}

QJsonArray playlist_manager::load() {
    QFile file(QDir(fm->userDataDir).filePath("playlists.json"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString jsonData = in.readAll();
        file.close();

        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData.toUtf8());
        if (jsonDoc.isArray()) {
            return jsonDoc.array();
        } else {
            qWarning() << "Invalid JSON format in playlists.json";
        }
    } else {
        // Si l'ouverture du fichier a échoué, créez un nouveau fichier avec un tableau vide
        QFile newFile(QDir(fm->userDataDir).filePath("playlists.json"));
        if (newFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QJsonArray emptyArray;
            QJsonDocument emptyDoc(emptyArray);
            newFile.write(emptyDoc.toJson());
            newFile.close();
        } else {
            qWarning() << "Failed to create playlists.json";
        }
    }

    return QJsonArray(); // Retourne un tableau vide en cas d'échec
}

void playlist_manager::save() {
    QJsonArray rawPlaylistse = QJsonArray();  // Réinitialiser la liste des playlists JSON

    // Parcourir toutes les playlists et les sauvegarder en JSON
    for (playlist_item *pl : playlists) {
        rawPlaylistse.append(pl->save());  // Sauvegarder chaque playlist en tant que QJsonObject
    }

    // Chemin vers le fichier JSON
    QString filePath = QDir(fm->userDataDir).filePath("playlists.json");

    // Ouvrir le fichier en écriture
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        // Créer un document JSON et l'écrire dans le fichier
        QJsonDocument jsonDoc(rawPlaylistse);
        file.write(jsonDoc.toJson(QJsonDocument::Indented));  // Indentation pour une meilleure lisibilité
        file.close();
    } else {
        qWarning() << "Impossible d'ouvrir le fichier pour écrire:" << filePath;
    }
}

void playlist_manager::drawPlaylists() {
    for (const auto& pl : raw_playlists) {

        playlist_item* n = new playlist_item(
            pl.toObject().value("name").toString(),
            pl.toObject().value("musics").toArray(),
            pl.toObject().value("thumb").toString(),
            pl.toObject().value("plid").toString(),
            gridLayoutPlay,
            scrollArea,
            mp,
            window,
            this,
            pl.toObject().value("doShuffle").toBool()
            );

        playlists.append(n);
        //mapping[pl["plid"].toString()] = n;
        playlists.last()->create();
    }
}

void playlist_manager::newPlaylist() {
    QJsonArray* ab = new QJsonArray();
    QString timestamp = QString::fromStdString(std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
    playlist_item* n = new playlist_item(
        "New Playlist",
        *ab,
        "https://cdn.pixabay.com/photo/2018/08/14/09/59/mountains-3605113_1280.jpg",
        timestamp,
        gridLayoutPlay,
        scrollArea,
        mp,
        window,
        this,
        false
        );
    playlists.append(n);
    playlists.last()->create();
    playlists.last()->modify();
}

void playlist_manager::reload() {
    qDebug("Reloading playlists...");
    for (const auto& pl : playlists) {
        pl->PlaylistTemplate->setParent(nullptr);
        gridLayoutPlay->removeWidget(pl->PlaylistTemplate);

        // Retirer l'élément de la liste des playlists
        playlists.removeOne(pl);

        // Supprimer l'objet
        delete pl;
    }
    nextPos = new pos(0,0);
    drawPlaylists();
}

pos* playlist_manager::getNewPos() {
    int maxcol = 0;
    nextPos->y += 1;
    if(nextPos->y > maxcol) {
        nextPos->y = 0;
        nextPos->x += 1;
    }
    return nextPos;

}



