#include "dp_audioapi.h"
#include "QtMultimedia/QMediaPlayer"
#include "QtMultimedia/QAudioOutput"


dp_audioapi::dp_audioapi() {
    isPlaying = false;
    paused = true;
    current = nullptr;
    player = new QMediaPlayer;
    audio = new QAudioOutput;
    player->setAudioOutput(audio);
    //connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &dp_audioapi::play_queue);
    timer->start(500);
}

void dp_audioapi::play(yt_music * vid) {
    current = vid;
    isPlaying = true;
    paused = false;
    player->stop();
    QString url = vid->get_url();

    player->setSource(QUrl(url));
    player->play();
}

void dp_audioapi::stop() {
    isPlaying = false;
    paused = true;
    player->stop();
}

void dp_audioapi::pause() {
    if (!paused) {
        player->pause();
        paused = true;
    }
    else {
        player->play();
        paused = false;
    }
}

void dp_audioapi::setVolume(int vol) {
    float truevol  = vol/100.0;
    audio->setVolume(truevol);
}

void dp_audioapi::play_queue() {


    if (queue.size() == 0) { return;}
    if (isPlaying == true) {
        //qInfo()<< current->duration;
        //qInfo()<< player->position();
        if (player->position() >= (current->duration-1)*1000) {
            isPlaying = false;
            paused = true;
        }
        else {
            return;
        }
    }
    play(queue.front());
    queue.pop_front();
    emit hasToUpdate();
}

void dp_audioapi::add_to_queue(yt_music* mus) {
    queue.push_back(mus);
}

void dp_audioapi::add_and_play(QList<yt_music*> mus) {
    QList<yt_music*> mus1 = mus;
    std::reverse(mus1.begin(), mus1.end());
    for (yt_music* item : mus1) {
        queue.push_front(item);
    }
    stop();
}

void dp_audioapi::add_and_play_s(yt_music* mus) {
    queue.push_front(mus);
    stop();
}
