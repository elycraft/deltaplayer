#ifndef DP_AUDIOAPI_H
#define DP_AUDIOAPI_H
#include "QtMultimedia/QMediaPlayer"
#include "QtMultimedia/QAudioOutput"
#include "yt_music.h"
#include "QTimer"


class dp_audioapi : public QObject
{
Q_OBJECT
public:
    dp_audioapi();
    bool isPlaying;
    bool paused;
    yt_music * current;
    void play(yt_music * vid);
    void setVolume(int vol);
    void stop();
    void pause();
    QMediaPlayer* player;
    std::list <yt_music*> queue;
    void add_to_queue(yt_music* mus);
    void add_and_play_s(yt_music* mus);
    void add_and_play(QList<yt_music*> mus);


public slots:
    void play_queue();

signals:
    void hasToUpdate();

private:

    QAudioOutput* audio;
};

#endif // DP_AUDIOAPI_H
