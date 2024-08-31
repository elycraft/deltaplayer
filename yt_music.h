#ifndef YT_MUSIC_H
#define YT_MUSIC_H
#include <QString>
#include <map>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMetaType>


class yt_music
{

public:
    yt_music(QString ytDlpPathin, QString url = "", std::map<QString, QString>  data = {});

    QString id;
    QString furl;
    QString title;
    QString thumb;
    QString author;
    int duration;
    QString url;

    QString print();
    QString get_url();
    QJsonObject save();


private:
    std::wstring stringToWstring(const std::string& str);
    std::string execAndCaptureOutput(const std::string& cmd);
    std::string ytDlpPath;
    std::string infos;
};

#endif // YT_MUSIC_H
