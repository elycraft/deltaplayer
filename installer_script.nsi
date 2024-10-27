!define APP_NAME "Deltaplayer"
!define APP_VERSION "B2.0"
!define INSTALL_DIR "$PROGRAMFILES\Deltaplayer"

SetCompressor /SOLID lzma

Name "${APP_NAME} ${APP_VERSION}"
OutFile "DeltaplayerInstaller.exe"
InstallDir "${INSTALL_DIR}"

Page directory
Page instfiles

Section "Install"
    SetOutPath "$INSTDIR"
    File /r "build\Desktop_Qt_6_7_2_MinGW_64_bit-MinSizeRel\*.*"
    CreateShortcut "$DESKTOP\Deltaplayer.lnk" "$INSTDIR\Deltaplayer.exe"
    CreateShortcut "$SMPROGRAMS\Deltaplayer.lnk" "$INSTDIR\Deltaplayer.exe" 
SectionEnd