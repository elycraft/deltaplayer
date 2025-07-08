#include "cmdworker.h"

cmdWorker::cmdWorker() {

}

std::wstring cmdWorker::stringToWstring(const std::string& str) {
    return std::wstring();
}

void cmdWorker::execAndCaptureOutput(const std::string& cmd) {
    androidYtdlpManager *Ytdlp = qApp->property("YtdlpM").value<androidYtdlpManager*>();

    QString output = Ytdlp->get_info(QString::fromStdString(cmd));

    emit resultReady(output);
}
