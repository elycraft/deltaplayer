#include "playbar_manager.h"
#include "GUI_BASE.h"
#include "./ui_GUI_BASE.h"
#include "qdir.h"

QString maxlen(const QString& s, int l) {
    if (s.length() > l) {
        int tm = l - 3; // Subtract 3 to account for the length of "..."
        return s.left(tm) + "...";
    }
    return s;
}

QString formatDuration(qint64 durationInSeconds) {
    int hours = durationInSeconds / 3600;
    int minutes = (durationInSeconds % 3600) / 60;
    int seconds = durationInSeconds % 60;

    QTime time(hours, minutes, seconds);
    return time.toString("HH:mm:ss");
}


playbar_manager::playbar_manager(MainWindowt* mainWindow, SettingManager *smin) {
    window = mainWindow;
    auto ui = mainWindow->ui;

    mp = new dp_audioapi();
    il = new ImageLoader();
    sm = smin;


    connect(ui->btn_pause, &QPushButton::clicked, this, &playbar_manager::btnPause);
    connect(ui->btn_rewind, &QPushButton::clicked, this, &playbar_manager::btnRewind);
    connect(ui->btn_skip, &QPushButton::clicked, this, &playbar_manager::btnSkip);
    connect(mp->player, &QMediaPlayer::positionChanged, this, &playbar_manager::update_time);
    connect(ui->soundSlider, &QSlider::valueChanged, this, &playbar_manager::slider_sound);
    connect(mp, &dp_audioapi::hasToUpdate, this, &playbar_manager::update_ui);

    update_ui();
    ui->Bar->setValue(0);
    ui->Bar->setFormat(":/:");

    int vol  = sm->getSetting("volume").toInt();
    mp->setVolume(vol);
    ui->soundSlider->setValue(vol);
}

void playbar_manager::btnPause() {
    mp->pause();
    if (mp->paused) {
        auto icon9 = QIcon();
        icon9.addPixmap(QPixmap(":/16x16/icons/gplay_play.png"), QIcon::Normal, QIcon::Off);
        window->ui->btn_pause->setIcon(icon9);
        window->ui->btn_pause->setIconSize(QSize(50, 50));
    }
    else {
        auto icon9 = QIcon();
        icon9.addPixmap(QPixmap(":/16x16/icons/gplay_pause.png"), QIcon::Normal, QIcon::Off);
        window->ui->btn_pause->setIcon(icon9);
        window->ui->btn_pause->setIconSize(QSize(50, 50));
    }


}

void playbar_manager::btnRewind() {
    yt_music *vid = new yt_music("https://www.youtube.com/watch?v=ixRLjjTRczE");
    mp->add_and_play_s(vid);
}

void playbar_manager::btnSkip() {
    mp->stop();
}

void playbar_manager::update_ui() {
    if (mp->current == nullptr) {
        window->ui->MusicName->setText("");
        QString image = il->get("https://upload.wikimedia.org/wikipedia/commons/thumb/4/49/A_black_image.jpg/640px-A_black_image.jpg");
        QString ss = QString("border-radius: 15px;\nborder-image: url(%1) 50 50 50 50").arg(image);
        window->ui->MusicPoster->setStyleSheet(ss);
    } else {
        window->ui->MusicName->setText(maxlen(mp->current->title,50));
        QString image = il->get(mp->current->thumb);
        QString ss = QString("border-radius: 15px;\nborder-image: url(%1) 50 50 50 50").arg(image);
        window->ui->MusicPoster->setStyleSheet(ss);
    }

    if (mp->paused) {
        auto icon9 = QIcon();
        icon9.addPixmap(QPixmap(":/16x16/icons/gplay_play.png"), QIcon::Normal, QIcon::Off);
        window->ui->btn_pause->setIcon(icon9);
        window->ui->btn_pause->setIconSize(QSize(50, 50));
    }
    else {
        auto icon9 = QIcon();
        icon9.addPixmap(QPixmap(":/16x16/icons/gplay_pause.png"), QIcon::Normal, QIcon::Off);
        window->ui->btn_pause->setIcon(icon9);
        window->ui->btn_pause->setIconSize(QSize(50, 50));
    }

}

void playbar_manager::update_time() {
    int pos = mp->player->position()/1000;
    QString sbar = formatDuration(pos)+"/"+formatDuration(mp->current->duration);
    window->ui->Bar->setFormat(sbar);
    int t2 = (pos*100)/(mp->player->duration()/1000);
    window->ui->Bar->setValue(t2);
}

void playbar_manager::slider_sound() {
    int val = window->ui->soundSlider->value();
    mp->setVolume(val);
}



