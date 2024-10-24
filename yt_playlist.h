#ifndef YT_PLAYLIST_H
#define YT_PLAYLIST_H
#include "cmdworker.h"
#include <qeventloop.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qthread.h>

class yt_playlist
{
public:
    yt_playlist(QString ytDlpPathin);
    QString get_info(QString furl);
private:
    std::string ytDlpPath;
};

#endif // YT_PLAYLIST_H
