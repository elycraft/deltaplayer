#ifndef CMDWORKER_H
#define CMDWORKER_H
#include <QObject>

class cmdWorker : public QObject
{
Q_OBJECT
public:
    cmdWorker();
    std::wstring stringToWstring(const std::string& str);
    std::string execAndCaptureOutput(const std::string& cmd);
};

#endif // CMDWORKER_H
