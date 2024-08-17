cmake_minimum_required(VERSION 3.5)

project(Deltaplayer2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
    main.cpp
    GUI_BASE.ui
    GUI_BASE.cpp
    GUI_BASE.h
    playbar_manager.h
    playbar_manager.cpp
    dp_audioapi.h
    dp_audioapi.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Deltaplayer2
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        icons/delta.png icons/delta_new.png icons/gplay_no-image.png icons/gplay_pause.png icons/gplay_play.png icons/gplay_rewind.png icons/gplay_skip.png icons/logo.png icons/logo_new.png
        GUI_BASE.ui
        files.qrc
        GUI_BASE.cpp
        playbar_manager.h playbar_manager.cpp
        dp_audioapi.h dp_audioapi.cpp
        yt_music.h yt_music.cpp
        imageloader.h imageloader.cpp
        filemanager.h filemanager.cpp
        playlist_manager.h playlist_manager.cpp
        playlist_item.h playlist_item.cpp
        customlistwidget.h customlistwidget.cpp
        draggableitem.h draggableitem.cpp


    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Deltaplayer2 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Deltaplayer2 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Deltaplayer2
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Deltaplayer2 PRIVATE
    Qt6::Widgets
    Qt6::Multimedia

)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Deltaplayer2)
endif()
set_target_properties(Deltaplayer2 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Deltaplayer2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Deltaplayer2)
endif()
