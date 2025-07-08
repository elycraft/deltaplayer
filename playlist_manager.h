#ifndef PLAYLIST_MANAGER_H
#define PLAYLIST_MANAGER_H
#include "QObject"
#include "GUI_BASE.h"
#include "filemanager.h"
#include "imageloader.h"
#include "dp_audioapi.h"
#include "playbar_manager.h"
#include "QDir"
#include "playlist_item.h"
#include "customlistwidget.h"
#include "draggableitem.h"
#include "settingmanager.h"
#include <QScroller>

class pos {
public:
    pos(int xin, int yin);
    int x;
    int y;
};


class playlist_manager : public QObject
{

Q_OBJECT
public:
    playlist_manager(MainWindowt* mainWindow,dp_audioapi* mpin, SettingManager* smin);
    pos* getNewPos();
    CustomListWidget *playlistEditList;
    CustomListWidget *serachVL;
    QList<playlist_item*> playlists;
    void save();
    QString ytPath;

private:
    FileManager* fm;
    ImageLoader* il;
    MainWindowt* window;
    dp_audioapi* mp;
    QJsonArray raw_playlists;
    SettingManager* sm;

    void addBtnAdd();
    QJsonArray load();
    void drawPlaylists();
    void newPlaylist();
    pos* nextPos;

    QScrollArea *scrollArea;
    QScrollArea* scrollAreaNNN;

    QFrame *AddPlaylist;
    QWidget *scrollAreaWidgetContents;
    QWidget* scrollAreaWidgetContentsNNN;
    QVBoxLayout *AddPlaylistLayout;
    QPushButton *btn_addplaylist;
    QGridLayout *gridLayoutPlay;
    QVBoxLayout* bigLayout;

    QScrollArea *scrollAreaserachVL;
    QWidget *scrollAreaWidgetContentsserachVL;



};

#endif // PLAYLIST_MANAGER_H
