#ifndef PLAYLIST_ITEM_H
#define PLAYLIST_ITEM_H
#include "QString"
#include "QJsonArray"
#include "QLayout"
#include "QScrollArea"
#include "dp_audioapi.h"
#include "GUI_BASE.h"
#include "filemanager.h"
#include "imageloader.h"
#include "QPushButton"
#include "QLabel"
#include "QLayout"
#include "QFrame"
#include "QListWidgetItem"

class playlist_manager;

class playlist_item : public QObject
{
Q_OBJECT
public:
    playlist_item(QString namein,QJsonArray musicsin,QString thumbin, QString plidin,QGridLayout* layoutin, QScrollArea* gridFramein, dp_audioapi* mpin, MainWindowt* windowin, playlist_manager* parentin,bool doShufflein = false);
    void create();
    void modify();
    void updateList();



public slots:
    void playAt(QListWidgetItem* wo);

private:
    QString nameI;
    QJsonArray raw_musics;
    QList<yt_music*> musicsI;
    QString thumbI;
    QString plid;
    QGridLayout* layout;
    QScrollArea* gridFrame;
    dp_audioapi* mp;
    MainWindowt* window;
    playlist_manager* parent;
    bool doShuffle;
    FileManager* fm;
    ImageLoader* il;
    QString thumbUrl;
    QList<yt_music*> getRealPlaylist(const QJsonArray& fromjson);
    void play();


    QFrame *PlaylistTemplate;
    QVBoxLayout *verticalLayout_13;
    QPushButton *thumb;
    QFrame *options;
    QHBoxLayout *optionLayout;
    QLabel *plname;
    QPushButton *modifyPlaylist;




};

#endif // PLAYLIST_ITEM_H
