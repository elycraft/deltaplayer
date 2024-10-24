#include "cmdworker.h"
#include <windows.h>
#include <iostream>

cmdWorker::cmdWorker() {

}

std::wstring cmdWorker::stringToWstring(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
void cmdWorker::execAndCaptureOutput(const std::string& cmd) {
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;
    if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0)) {
        std::cerr << "StdoutRd CreatePipe" << std::endl;
        emit resultReady("");
        return;
    }

    if (!SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
        std::cerr << "Stdout SetHandleInformation" << std::endl;
        emit resultReady("");
        return;
    }

    PROCESS_INFORMATION piProcInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    STARTUPINFO siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    siStartInfo.wShowWindow = SW_HIDE;

    // Conversion de std::string en std::wstring pour cmd
    std::wstring wcmd = stringToWstring(cmd);

    // Utilisation de CreateProcessW pour les chaînes de caractères larges
    if (!CreateProcessW(NULL, const_cast<LPWSTR>(wcmd.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo)) {
        std::cerr << "CreateProcess "+cmd << std::endl;
        emit resultReady("");
        return;
    }

    CloseHandle(hChildStd_OUT_Wr);

    const int bufferSize = 4096;
    char buffer[bufferSize];
    DWORD dwRead;
    std::string output;

    while (ReadFile(hChildStd_OUT_Rd, buffer, bufferSize, &dwRead, NULL) && dwRead > 0) {
        output.append(buffer, dwRead);
    }

    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    emit resultReady(QString::fromStdString(output));
}
