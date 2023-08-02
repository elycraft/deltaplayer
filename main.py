################################################################################
##
## BY: WANDERSON M.PIMENTA
## PROJECT MADE WITH: Qt Designer and PySide2
## V: 1.0.0
##
## This project can be used freely for all uses, as long as they maintain the
## respective credits only in the Python scripts, any information in the visual
## interface (GUI) can be modified without any implication.
##
## There are limitations on Qt licenses if you want to use your products
## commercially, I recommend reading them on the official website:
## https://doc.qt.io/qtforpython/licenses.html
##
################################################################################

import sys
import os
import platform
from PySide2 import QtCore, QtGui, QtWidgets
from PySide2.QtCore import (QCoreApplication, QPropertyAnimation, QDate, QDateTime, QMetaObject, QObject, QPoint, QRect, QSize, QTime, QUrl, Qt, QEvent)
from PySide2.QtGui import (QBrush, QColor, QConicalGradient, QCursor, QFont, QFontDatabase, QIcon, QKeySequence, QLinearGradient, QPalette, QPainter, QPixmap, QRadialGradient)
from PySide2.QtWidgets import *
from app_modules import *
import logging
import logging.handlers
import datetime

if getattr(sys, 'frozen', False):
    import pyi_splash

class SplashScreen(QSplashScreen):
    def __init__(self):
        super(QSplashScreen, self).__init__()
        loadUi("splash.py",self)
        self.setWindowFlag(Qt.FramelessWindowHint)


class GZipRotator:
    def __call__(self, source, dest):
        os.rename(source, dest)
        f_in = open(dest, 'rb')
        f_out = gzip.open("%s.gz" % dest, 'wb')
        f_out.writelines(f_in)
        f_out.close()
        f_in.close()
        os.remove(dest)

class MainWindow(QMainWindow):
    def __init__(self,logger):
        QMainWindow.__init__(self)
        self.logger = logger
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        ## PRINT ==> SYSTEM
        self.logger.info('System: ' + platform.system())
        self.logger.info('Version: ' +platform.release())

        ########################################################################
        ## START - WINDOW ATTRIBUTES
        ########################################################################

        ## REMOVE ==> STANDARD TITLE BAR
        UIFunctions.removeTitleBar(True)
        ## ==> END ##

        ## SET ==> WINDOW TITLE
        self.setWindowTitle('Deltaplayer')
        self.setWindowIcon(QtGui.QIcon(':/16x16/icons/logo_new.png'))
        UIFunctions.labelTitle(self, 'Deltaplayer')
        UIFunctions.labelDescription(self, '')
        ## ==> END ##

        ## WINDOW SIZE ==> DEFAULT SIZE
        startSize = QSize(1500, 900)
        self.resize(startSize)
        self.setMinimumSize(startSize)
        # UIFunctions.enableMaximumSize(self, 500, 720)
        ## ==> END ##

        ## ==> CREATE MENUS
        ########################################################################

        ## ==> TOGGLE MENU SIZE
        self.ui.btn_toggle_menu.clicked.connect(lambda: UIFunctions.toggleMenu(self, 220, True))
        ## ==> END ##

        ## ==> ADD CUSTOM MENUS
        self.ui.stackedWidget.setMinimumWidth(20)
        UIFunctions.addNewMenu(self, "Playlists", "btn_playlists", "url(:/20x20/icons/20x20/cil-music-note.png)", True)
        UIFunctions.addNewMenu(self, "Games", "btn_games", "url(:/16x16/icons/16x16/cil-gamepad.png)", True)
        UIFunctions.addNewMenu(self, "Settings","btn_settings","url(:/16x16/icons/16x16/cil-settings.png)",True)
        #UIFunctions.addNewMenu(self, "Custom Widgets", "btn_widgets", "url(:/16x16/icons/16x16/cil-equalizer.png)", False)
        ## ==> END ##

        # START MENU => SELECTION
        UIFunctions.selectStandardMenu(self, "btn_playlists")
        ## ==> END ##

        ## ==> START PAGE
        self.ui.stackedWidget.setCurrentWidget(self.ui.page_playlists)
        ## ==> END ##

        ## USER ICON ==> SHOW HIDE
        UIFunctions.userIcon(self, "ED", "", True)
        ## ==> END ##


        ## ==> MOVE WINDOW / MAXIMIZE / RESTORE
        ########################################################################
        def moveWindow(event):
            # IF MAXIMIZED CHANGE TO NORMAL
            if UIFunctions.returStatus() == 1:
                UIFunctions.maximize_restore(self)

            # MOVE WINDOW
            if event.buttons() == Qt.LeftButton:
                self.move(self.pos() + event.globalPos() - self.dragPos)
                self.dragPos = event.globalPos()
                event.accept()

        # WIDGET TO MOVE
        self.ui.frame_label_top_btns.mouseMoveEvent = moveWindow
        ## ==> END ##

        ## ==> LOAD DEFINITIONS
        ########################################################################
        UIFunctions.uiDefinitions(self)
        ## ==> END ##

        ########################################################################
        ## END - WINDOW ATTRIBUTES
        ############################## ---/--/--- ##############################




        ########################################################################
        #                                                                      #
        ## START -------------- WIDGETS FUNCTIONS/PARAMETERS ---------------- ##
        #                                                                      #
        ## ==> USER CODES BELLOW                                              ##
        ########################################################################



        ## ==> QTableWidget RARAMETERS
        ########################################################################
        self.ui.tableWidget.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        ## ==> END ##



        ########################################################################
        #                                                                      #
        ## END --------------- WIDGETS FUNCTIONS/PARAMETERS ----------------- ##
        #                                                                      #
        ############################## ---/--/--- ##############################


        ## SHOW ==> MAIN WINDOW
        ########################################################################
        self.show()
        ## ==> END ##

    ########################################################################
    ## MENUS ==> DYNAMIC MENUS FUNCTIONS
    ########################################################################
    def Button(self):
        # GET BT CLICKED
        btnWidget = self.sender()

        # PAGE Playlists
        if btnWidget.objectName() == "btn_playlists":
            self.ui.stackedWidget.setCurrentWidget(self.ui.page_playlists)
            UIFunctions.resetStyle(self, "btn_playlists")
            UIFunctions.labelPage(self, "My Playlists")
            btnWidget.setStyleSheet(UIFunctions.selectMenu(btnWidget.styleSheet()))

        # PAGE Games
        if btnWidget.objectName() == "btn_games":
            self.ui.stackedWidget.setCurrentWidget(self.ui.page_games)
            UIFunctions.resetStyle(self, "btn_games")
            UIFunctions.labelPage(self, "My Games")
            btnWidget.setStyleSheet(UIFunctions.selectMenu(btnWidget.styleSheet()))

        # Settings
        if btnWidget.objectName() == "btn_settings":
            self.ui.stackedWidget.setCurrentWidget(self.ui.page_settings)
            UIFunctions.resetStyle(self, "btn_settings")
            UIFunctions.labelPage(self, "Settings")
            btnWidget.setStyleSheet(UIFunctions.selectMenu(btnWidget.styleSheet()))

        self.logger.info(f"Go to {btnWidget.objectName()}")


    ## ==> END ##

    ########################################################################
    ## START ==> APP EVENTS
    ########################################################################

    ## EVENT ==> MOUSE DOUBLE CLICK
    ########################################################################
    def eventFilter(self, watched, event):
        if watched == self.le and event.type() == QtCore.QEvent.MouseButtonDblClick:
            self.logger.info("pos: ", event.pos())
    ## ==> END ##

    ## EVENT ==> MOUSE CLICK
    ########################################################################
    def mousePressEvent(self, event):
        self.dragPos = event.globalPos()
        if event.buttons() == Qt.LeftButton:
            pass
        if event.buttons() == Qt.RightButton:
           pass
        if event.buttons() == Qt.MidButton:
            pass
    ## ==> END ##

    ## EVENT ==> KEY PRESSED
    ########################################################################
    def keyPressEvent(self, event):
        pass#('Key: ' + str(event.key()) + ' | Text Press: ' + str(event.text()))
    ## ==> END ##

    ## EVENT ==> RESIZE EVENT
    ########################################################################
    def resizeEvent(self, event):
        self.resizeFunction()
        return super(MainWindow, self).resizeEvent(event)

    def resizeFunction(self):
        self.logger.info('Height: ' + str(self.height()) + ' | Width: ' + str(self.width()))

    
    ## ==> END ##

    
    ########################################################################
    ## END ==> APP EVENTS
    ############################## ---/--/--- ##############################



if __name__ == "__main__":
    app = QApplication(sys.argv)

    splash = SplashScreen()
    splash.show()
    
    fm = FileManager()

    root = logging.getLogger()
    root.setLevel(logging.DEBUG)

    handler = logging.StreamHandler(sys.stdout)
    handler.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(name)s:[%(levelname)s] at %(asctime)s : %(message)s')
    handler.setFormatter(formatter)
    handler.rotator = GZipRotator()
    root.addHandler(handler)

    now = datetime.datetime.now()
    log = logging.handlers.TimedRotatingFileHandler(os.path.join(fm.user_log_dir,f"dp-root-log-{now.strftime('%Y%m%d')}.log"), 'midnight', 1, backupCount=5)
    log.setLevel(logging.DEBUG)
    log.setFormatter(formatter)
    log.rotator = GZipRotator()
    root.addHandler(log)

    
    QtGui.QFontDatabase.addApplicationFont('fonts/segoeui.ttf')
    QtGui.QFontDatabase.addApplicationFont('fonts/segoeuib.ttf')
    window = MainWindow(root)
    
    settingM = SettingManager(window.ui,root)
    playBar = PlayBarManager(window.ui,settingM,root)
    playlistM = PlaylistManager(window.ui,playBar.mp,root)
    gameM = GameManager(window.ui,playlistM,root)
    detectorM = DetectorManager(gameM,playlistM,playBar,root)
    
    exitM = ExitProgram([playBar,playlistM,settingM,gameM,detectorM])

    app.aboutToQuit.connect(exitM.appExit)
    root.info("App started")
    if getattr(sys, 'frozen', False):
        pyi_splash.close()
    sys.exit(app.exec_())
