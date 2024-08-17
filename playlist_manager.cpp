#include "playlist_manager.h"
#include "GUI_BASE.h"
#include "./ui_GUI_BASE.h"

pos::pos(int xin, int yin) {
    x = xin;
    y = yin;
}





playlist_manager::playlist_manager(MainWindowt* mainWindow, dp_audioapi* mpin) {

    window = mainWindow;
    auto ui = mainWindow->ui;
    fm = new FileManager();
    il = new ImageLoader(fm);
    mp = mpin;




    playlistEditList = new CustomListWidget(ui->verticalFrame1);
    playlistEditList->setDragDropMode(QAbstractItemView::InternalMove);
    playlistEditList->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Set stylesheet and object name
    playlistEditList->setStyleSheet("border: 0px");
    playlistEditList->setObjectName("playlistEditList");

    // Add CustomListWidget to the vertical layout
    ui->verticalLayout_17->addWidget(playlistEditList);

    // Create a QScrollArea
    scrollArea = new QScrollArea(ui->page_playlists);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("border-radius: 0px;");

    scrollAreaWidgetContents = new QWidget();
    gridLayoutPlay = new QGridLayout(scrollAreaWidgetContents);

    scrollArea->setWidget(scrollAreaWidgetContents);

    ui->verticalLayout_10->addWidget(scrollArea);

    nextPos = new pos(0,0);

    addBtnAdd();
    raw_playlists = load();
    drawPlaylists();
}

void playlist_manager::addBtnAdd() {
    AddPlaylist = new QFrame(scrollArea);
    AddPlaylist->setMaximumSize(150, 150);
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

    //connect(btn_addplaylist, &QPushButton::clicked, this, &MainWindow::newPlaylist);
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

pos* playlist_manager::getNewPos() {
    int maxcol = 5;
    nextPos->y += 1;
    if(nextPos->y > maxcol) {
        nextPos->y = 0;
        nextPos->x += 1;
    }
    return nextPos;

}



