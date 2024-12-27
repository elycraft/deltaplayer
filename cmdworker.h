#ifndef CMDWORKER_H
#define CMDWORKER_H
#include <QObject>
#include <QString>
#include <QJniObject >
#include <QJniEnvironment>
#include <QDebug>

class cmdWorker : public QObject
{
Q_OBJECT
public:
    cmdWorker();
    std::wstring stringToWstring(const std::string& str);
    void execAndCaptureOutput(const std::string& cmd);
signals:
    void resultReady(const QString& result);
};

#endif // CMDWORKER_H
