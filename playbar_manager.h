#ifndef PLAYBAR_MANAGER_H
#define PLAYBAR_MANAGER_H
#include "GUI_BASE.h"
#include "QtMultimedia/QMediaPlayer"
#include "QtMultimedia/QAudioOutput"
#include "dp_audioapi.h"
#include <QObject>
#include "yt_music.h"
#include "imageloader.h"
#include "QScrollArea"
#include "QVBoxLayout"
#include "QPushButton"



class playbar_manager : public QObject
{
public:
    playbar_manager(MainWindowt* mainWindow);
    void update_ui();
    dp_audioapi* mp;



public slots:
    void btnPause();
    void btnRewind();
    void btnSkip();
    void update_time();
    void slider_sound();




private:
    MainWindowt* window;// Pointeur vers MainWindowt

    ImageLoader* il;



};

#endif // PLAYBAR_MANAGER_H
