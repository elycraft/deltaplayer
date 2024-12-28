#ifndef ANDROIDYTDLPMANAGER_H
#define ANDROIDYTDLPMANAGER_H
#include <qmessagebox.h>
#include <QCoreApplication>
#include <QObject>
#include <QString>
#include <QJniObject >
#include <QJniEnvironment>
#include <QDebug>

Q_DECLARE_JNI_CLASS(YoutubeDLRequest, "com/yausername/youtubedl_android/YoutubeDLRequest")

class androidYtdlpManager : public QObject
{
Q_OBJECT


public:
    androidYtdlpManager();
    void init();
    QJniObject ytDlpInstance;
    void get_info(QString url);

};

#endif // ANDROIDYTDLPMANAGER_H
