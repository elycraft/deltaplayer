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
#include "DraggableItem.h"

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
    playlist_manager(MainWindowt* mainWindow,dp_audioapi* mpin);
    pos* getNewPos();
    CustomListWidget *playlistEditList;

private:
    FileManager* fm;
    ImageLoader* il;
    MainWindowt* window;
    dp_audioapi* mp;
    QJsonArray raw_playlists;
    QList<playlist_item*> playlists;
    void addBtnAdd();
    QJsonArray load();
    void drawPlaylists();
    pos* nextPos;

    QScrollArea *scrollArea;
    QFrame *AddPlaylist;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *AddPlaylistLayout;
    QPushButton *btn_addplaylist;
    QGridLayout *gridLayoutPlay;



};

#endif // PLAYLIST_MANAGER_H
