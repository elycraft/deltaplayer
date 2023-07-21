import sys
import platform
from PySide2 import QtCore, QtGui, QtWidgets
from PySide2.QtCore import (QCoreApplication, QPropertyAnimation, QDate, QDateTime, QMetaObject, QObject, QPoint, QRect, QSize, QTime, QUrl, Qt, QEvent)
from PySide2.QtGui import (QBrush, QColor, QConicalGradient, QCursor, QFont, QFontDatabase, QIcon, QKeySequence, QLinearGradient, QPalette, QPainter, QPixmap, QRadialGradient)
from PySide2.QtWidgets import *
from ui_cdialog import Ui_Dialog

# GUI FILE
from app_modules import *

class MyTextEdit(QTextEdit):
#|-----------------------------------------------------------------------------|
# class Variables
#|-----------------------------------------------------------------------------| 
    #no classVariables
#     myFocusOutSignal=QtCore.pyqtSignal(str)
#|-----------------------------------------------------------------------------|
# Constructor  
#|-----------------------------------------------------------------------------|
    def __init__(self,*args):
        #*args to set parent
        QTextEdit.__init__(self,*args)
        font=QtGui.QFont()
        font.setPointSize(12)
        self.setFont(font)
        self.completer = None

        

#|--------------------------End of __init__------------------------------------|
#|-----------------------------------------------------------------------------| 
# setCompleter
#|-----------------------------------------------------------------------------|
    def setCompleter(self, completer):
        if self.completer:
            self.disconnect(self.completer, 0, self, 0)
        if not completer:
            return

        completer.setWidget(self)
        completer.setCompletionMode(QCompleter.PopupCompletion)
        completer.setCaseSensitivity(QtCore.Qt.CaseInsensitive)
        self.completer = completer
#        self.connect(self.completer,
#            QtCore.SIGNAL("activated(const QString&)"), self.insertCompletion)
        self.completer.insertText.connect(self.insertCompletion)
#|-----------------------End of setCompleter-------------------------------------|
#|-----------------------------------------------------------------------------| 
# insertCompletion
#|-----------------------------------------------------------------------------|
    def insertCompletion(self, completion):
        tc = self.textCursor()
        extra = (len(completion) -
            len(self.completer.completionPrefix()))
        tc.movePosition(QtGui.QTextCursor.Left)
        tc.movePosition(QtGui.QTextCursor.EndOfWord)
        tc.insertText(completion[-extra:])
        self.setTextCursor(tc)
#|-----------------------End of insertCompletion-------------------------------|
#|-----------------------------------------------------------------------------| 
# textUnderCursor
#|-----------------------------------------------------------------------------|
    def textUnderCursor(self):
        tc = self.textCursor()
        tc.select(QtGui.QTextCursor.WordUnderCursor)
        return tc.selectedText()
#|-----------------------End of textUnderCursor--------------------------------|
#|-----------------------------------------------------------------------------| 
# focusInEvent
#|-----------------------------------------------------------------------------|
    #---override
    def focusInEvent(self, event):
        if self.completer:
            self.completer.setWidget(self);
        QTextEdit.focusInEvent(self, event)
#|-----------------------End of focusInEvent-------------------------------------|
#|-----------------------------------------------------------------------------| 
# keyPressEvent
#|-----------------------------------------------------------------------------|
    #---override
    def keyPressEvent(self, event):
        if self.completer and self.completer.popup() and self.completer.popup().isVisible():
            if event.key() in (
            QtCore.Qt.Key_Enter,
            QtCore.Qt.Key_Return,
            QtCore.Qt.Key_Escape,
            QtCore.Qt.Key_Tab,
            QtCore.Qt.Key_Backtab):
                event.ignore()
                return
        ## has ctrl-Space been pressed??
        isShortcut = (event.modifiers() == QtCore.Qt.ControlModifier and\
                      event.key() == QtCore.Qt.Key_Space)
        ## modifier to complete suggestion inline ctrl-e
        inline = (event.modifiers() == QtCore.Qt.ControlModifier and \
                  event.key() == QtCore.Qt.Key_E)
        ## if inline completion has been chosen
        if inline:
            # set completion mode as inline
            self.completer.setCompletionMode(QCompleter.InlineCompletion)
            completionPrefix = self.textUnderCursor()
            if (completionPrefix != self.completer.completionPrefix()):
                self.completer.setCompletionPrefix(completionPrefix)
            self.completer.complete()
#            self.completer.setCurrentRow(0)
#            self.completer.activated.emit(self.completer.currentCompletion())
            # set the current suggestion in the text box
            self.completer.insertText.emit(self.completer.currentCompletion())
            # reset the completion mode
            self.completer.setCompletionMode(QCompleter.PopupCompletion)
            return
        if (not self.completer or not isShortcut):
            pass
            QTextEdit.keyPressEvent(self, event)
        # debug
#        print("After controlspace")
#        print("isShortcut is: {}".format(isShortcut))
        # debug over
        ## ctrl or shift key on it's own??
        ctrlOrShift = event.modifiers() in (QtCore.Qt.ControlModifier ,\
                QtCore.Qt.ShiftModifier)
        if ctrlOrShift and event.text()== '':
#             ctrl or shift key on it's own
            return
        # debug
#        print("After on its own")
#        print("isShortcut is: {}".format(isShortcut))
        # debug over
#         eow = QtCore.QString("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=") #end of word
#        eow = "~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=" #end of word
        eow = "~!@#$%^&*+{}|:\"<>?,./;'[]\\-=" #end of word

        hasModifier = ((event.modifiers() != QtCore.Qt.NoModifier) and\
                        not ctrlOrShift)

        completionPrefix = self.textUnderCursor()
#         print('event . text = {}'.format(event.text().right(1)))
#         if (not isShortcut and (hasModifier or event.text()=='' or\
#                                 len(completionPrefix) < 3 or \
#        """                      eow.contains(event.text().right(1)))):
        """
        if not isShortcut :
            if self.completer.popup():
                self.completer.popup().hide()
            return
        """
#        print("complPref: {}".format(completionPrefix))
#        print("completer.complPref: {}".format(self.completer.completionPrefix()))
#        print("mode: {}".format(self.completer.completionMode()))
#        if (completionPrefix != self.completer.completionPrefix()):
        self.completer.setCompletionPrefix(completionPrefix)
        popup = self.completer.popup()
        popup.setCurrentIndex(
            self.completer.completionModel().index(0,0))
        cr = self.cursorRect()
        cr.setWidth(self.completer.popup().sizeHintForColumn(0)
            + self.completer.popup().verticalScrollBar().sizeHint().width())
        self.completer.complete(cr) ## popup it up!

class MyDictionaryCompleter(QCompleter):
#|-----------------------------------------------------------------------------|
# class Variables
#|-----------------------------------------------------------------------------| 
    insertText = QtCore.Signal(str)
    #no classVariables
#|-----------------------------------------------------------------------------|
# Constructor  
#|-----------------------------------------------------------------------------|
    def __init__(self, myKeywords=None,parent=None):


        myKeywords =['apple','aggresive','ball','bat','cat','cycle','dog','dumb',\
                     'elephant','engineer','food','file','good','great',\
                     'hippopotamus','hyper','india','ireland','just','just',\
                     'key','kid','lemon','lead','mute','magic',\
                     'news','newyork','orange','oval','parrot','patriot',\
                     'question','queue','right','rest','smile','simple',\
                     'tree','urban','very','wood','xylophone','yellow',\
                     'zebra']
        QCompleter.__init__(self, myKeywords, parent)
        self.connect(self,
            QtCore.SIGNAL("activated(const QString&)"), self.changeCompletion)
#|--------------------------End of Constructor---------------------------------| 
#|-----------------------------------------------------------------------------| 
# changeCompletion
#|-----------------------------------------------------------------------------|
    def changeCompletion(self, completion):
        if completion.find("(") != -1:
            completion = completion[:completion.find("(")]
        print(completion)
        self.insertText.emit(completion)



class CdialogWin(QMainWindow):
    def __init__(self,callback,ty=0,keyw=None):
        QMainWindow.__init__(self)
        self.ui = Ui_Dialog()
        self.ui.setupUi(self)

        if ty == 0:
            self.ui.horizontalLayout_2 = QtWidgets.QHBoxLayout()
            self.ui.horizontalLayout_2.setObjectName("horizontalLayout_2")
            spacerItem = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Minimum)
            self.ui.horizontalLayout_2.addItem(spacerItem)
            self.ui.lineEdit = QtWidgets.QLineEdit(self.ui.verticalFrame_2)
            self.ui.lineEdit.setObjectName("lineEdit")
            self.ui.horizontalLayout_2.addWidget(self.ui.lineEdit)
            spacerItem1 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Minimum)
            self.ui.horizontalLayout_2.addItem(spacerItem1)
            self.ui.verticalLayout_3.addLayout(self.ui.horizontalLayout_2)
            
        else:
            self.ui.horizontalLayout_2 = QtWidgets.QHBoxLayout()
            self.ui.horizontalLayout_2.setObjectName("horizontalLayout_2")
            spacerItem = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Minimum)
            self.ui.horizontalLayout_2.addItem(spacerItem)
            completer = MyDictionaryCompleter(self.ui.verticalFrame_2,myKeywords=keyw)
            te = MyTextEdit()
            te.setCompleter(completer)
            self.ui.horizontalLayout_2.addWidget(te)
            spacerItem1 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Minimum)
            self.ui.horizontalLayout_2.addItem(spacerItem1)
            self.ui.verticalLayout_3.addLayout(self.ui.horizontalLayout_2)

        self.ui.buttonBox = QtWidgets.QDialogButtonBox(self.ui.verticalFrame_2)
        self.ui.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.ui.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.ui.buttonBox.setObjectName("buttonBox")
        self.ui.verticalLayout_3.addWidget(self.ui.buttonBox)
        self.ui.verticalLayout_2.addWidget(self.ui.verticalFrame_2)

        ## REMOVE ==> STANDARD TITLE BAR
        self.setWindowFlags(QtCore.Qt.CustomizeWindowHint)
        ## ==> END ##
        self.ui.label_title_bar_top.setText("  GPlay Dialog")
        ## SET ==> WINDOW TITLE
        self.setWindowTitle('GPlay Dialog')

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
        self.ui.label_title_bar_top.mouseMoveEvent = moveWindow

        self.response = None

        self.callback = callback

        self.ui.buttonBox.accepted.connect(self.btnOk)
        self.ui.buttonBox.rejected.connect(self.btnCancel)
        
        self.show()


    def btnOk(self):
        self.response = {"user":True,"msg":self.ui.lineEdit.text()}
        self.callback(self.response)
        self.close()

    def btnCancel(self):
        self.response = {"user":False,"msg":""}
        self.callback(self.response)
        self.close()


    
    def eventFilter(self, watched, event):
        if watched == self.le and event.type() == QtCore.QEvent.MouseButtonDblClick:
            print("pos: ", event.pos())
    ## ==> END ##

    ## EVENT ==> MOUSE CLICK
    ########################################################################
    def mousePressEvent(self, event):
        self.dragPos = event.globalPos()
        if event.buttons() == Qt.LeftButton:
            print('Mouse click: LEFT CLICK')
        if event.buttons() == Qt.RightButton:
            print('Mouse click: RIGHT CLICK')
        if event.buttons() == Qt.MidButton:
            print('Mouse click: MIDDLE BUTTON')
    ## ==> END ##

    ## EVENT ==> KEY PRESSED
    ########################################################################
    def keyPressEvent(self, event):
        print('Key: ' + str(event.key()) + ' | Text Press: ' + str(event.text()))
    ## ==> END ##


    def setTxt(self,txt):
        self.txt = txt
        self.ui.label.setText(self.txt)





if __name__ == "__main__":

    def ppprint(msg):
        print(msg)

    app = QApplication(sys.argv)
    QtGui.QFontDatabase.addApplicationFont('fonts/segoeui.ttf')
    QtGui.QFontDatabase.addApplicationFont('fonts/segoeuib.ttf')

    window = CdialogWin(ppprint)
    
    sys.exit(app.exec_())
