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

## ==> GUI FILE
from main import *
import api
from time import strftime, gmtime
from PySide2 import QtCore, QtGui, QtWidgets
from PySide2.QtGui import QDrag
from PySide2.QtCore import QMimeData
from PySide2.QtCore import Qt
import json
from platformdirs import *
from os import path
import os
import requests
from cdialog import CdialogWin
import hashlib
from fast_autocomplete import AutoComplete
import time
import detector
import threading
from pypresence import Presence
import random



def remove_non_ascii(s):
    return "".join(c for c in s if ord(c)<128)

def maxlen(s,l):
    if len(s)>l:
        tm = l-len(s)
        s = s[:tm]+"..."
    return s

class Functions(MainWindow):
    pass

class PlayBarManager():

    def __init__(self,ui,sm,logger) -> None:
        self.ui = ui
        self.sm = sm
        self.logger = logger
        self.il = ImageLoader()

        
        self.mp = api.Player(self.logger)
        self.mp.update.connect(self.updateUi)
        self.mp.updateTime.connect(self.updateTime)
        self.mp.playQueue()

        self.ui.btn_rewind.clicked.connect(lambda: self.btnRewind())
        self.ui.btn_pause.clicked.connect(lambda: self.btnPause())
        self.ui.btn_skip.clicked.connect(lambda: self.btnSkip())

        self.soundSlider = self.ui.soundSlider
        self.soundSlider.valueChanged.connect(self.sliderSound)
        self.soundSlider.setValue(sm.settings["volume"])

        client_id = '1132717098561441862'  # Fake ID, put your real one here
        self.RPC = Presence(client_id)  # Initialize the client class
        self.RPC.connect()

        self.updateTime()
        self.updateUi()
        
    
    def appExit(self):
        self.logger.info("Exiting API Player")
        self.mp.stopPlayQueue()

    def btnPause(self):
        self.logger.info("Play")
        self.mp.pause()
        if self.mp.paused:
            icon9 = QtGui.QIcon()
            icon9.addPixmap(QtGui.QPixmap(":/16x16/icons/gplay_play.svg"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
            self.ui.btn_pause.setIcon(icon9)
            self.ui.btn_pause.setIconSize(QtCore.QSize(50, 50))
        else:
            icon9 = QtGui.QIcon()
            icon9.addPixmap(QtGui.QPixmap(":/16x16/icons/gplay_pause.svg"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
            self.ui.btn_pause.setIcon(icon9)
            self.ui.btn_pause.setIconSize(QtCore.QSize(50, 50))


    def btnRewind(self):
        self.mp.addAndPlay(self.mp.backQueue[-2])
        del(self.mp.backQueue[-2])

    def btnSkip(self):
        self.logger.info("Skip")
        self.mp.skip()
    
    def sliderSound(self):
        self.sm.settings["volume"] = self.soundSlider.value()
        self.mp.updateSound(self.soundSlider.value())
    
    def updateTime(self):
        try: 
            a = strftime("%H:%M:%S", gmtime(self.mp.time[0]))
            b = strftime("%H:%M:%S", gmtime(self.mp.current.duration))
            self.ui.Bar.setFormat(f"{a}/{b}")
        except: self.ui.Bar.setFormat(f":/:")
        self.ui.Bar.setValue(self.mp.time[1])
    
    def updateUi(self):
        try: self.ui.MusicName.setText(maxlen(str(self.mp.current.title),50))
        except: self.ui.MusicName.setText("")
        try:
            image = self.il.get(self.mp.current.thumb)
            self.ui.MusicPoster.setStyleSheet(f"""border-radius: 15px;\nborder-image: url("{image}") 50 50 50 50""")
            self.RPC.update(
                details="Listening on Deltaplayer",
                state=f"{self.mp.current.author} - {self.mp.current.title}",
                end=int(time.time() + self.mp.current.duration),
                large_image=self.mp.current.thumb,
                small_image="dpicon"
            )
            if self.mp.paused:
                icon9 = QtGui.QIcon()
                icon9.addPixmap(QtGui.QPixmap(":/16x16/icons/gplay_play.svg"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
                self.ui.btn_pause.setIcon(icon9)
                self.ui.btn_pause.setIconSize(QtCore.QSize(50, 50))
            else:
                icon9 = QtGui.QIcon()
                icon9.addPixmap(QtGui.QPixmap(":/16x16/icons/gplay_pause.svg"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
                self.ui.btn_pause.setIcon(icon9)
                self.ui.btn_pause.setIconSize(QtCore.QSize(50, 50))
        except Exception as e:
            self.logger.error(e)
            image = self.il.get("https://upload.wikimedia.org/wikipedia/commons/thumb/4/49/A_black_image.jpg/640px-A_black_image.jpg")
            self.ui.MusicPoster.setStyleSheet(f"""border-radius: 15px;\nborder-image: url("{image}") 0 0 0 0 stretch stretch""")

class PlaylistManager():

    def __init__(self,ui,mp,logger) -> None:

        self.logger = logger
        self.fm = FileManager()
        self.ui = ui
        self.raw_playlists = self.load()
        self.playlists = []
        self.mp = mp
        self.mapping = {}

        self.ui.playlistEditList = CustomListWidget(self.ui.verticalFrame1,wid=DraggableItem)
        self.ui.playlistEditList.setDragDropMode(QListWidget.InternalMove)
        self.ui.playlistEditList.setSelectionMode(QListWidget.ExtendedSelection)

        self.ui.playlistEditList.setStyleSheet("border: 0px")
        self.ui.playlistEditList.setObjectName("playlistEditList")
        self.ui.verticalLayout_17.addWidget(self.ui.playlistEditList)

        self.scrollArea = QtWidgets.QScrollArea(self.ui.page_playlists)
        self.scrollArea.setWidgetResizable(True)
        self.scrollArea.setStyleSheet("border-radius: 0px;")
        self.scrollAreaWidgetContents = QtWidgets.QWidget()
        self.gridLayoutPlay = QtWidgets.QGridLayout(self.scrollAreaWidgetContents)
        self.scrollArea.setWidget(self.scrollAreaWidgetContents)
        self.ui.verticalLayout_10.addWidget(self.scrollArea)

        self.ui.pushButton_dbrefresh.clicked.connect(lambda: self.reloadall())

        self.nextPos = [0,0]

        self.addBtnAdd()

        self.drawPlaylists()

    def addBtnAdd(self):
        self.AddPlaylist = QtWidgets.QFrame(self.scrollArea)
        self.AddPlaylist.setMaximumSize(QtCore.QSize(150, 150))
        self.AddPlaylist.setStyleSheet("background-color: rgb(39, 44, 54);\n"
        " border-radius: 15px;")
        self.AddPlaylist.setObjectName("AddPlaylist")
        self.AddPlaylistLayout = QtWidgets.QVBoxLayout(self.AddPlaylist)
        self.AddPlaylistLayout.setObjectName("AddPlaylistLayout")
        self.btn_addplaylist = QtWidgets.QPushButton(self.AddPlaylist)
        font = QtGui.QFont()
        font.setPointSize(10)
        font.setBold(True)
        font.setWeight(75)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.btn_addplaylist.sizePolicy().hasHeightForWidth())
        self.btn_addplaylist.setSizePolicy(sizePolicy)
        self.btn_addplaylist.setFont(font)
        self.btn_addplaylist.setObjectName("btn_addplaylist")
        self.btn_addplaylist.setText("Add a Playlist")
        self.AddPlaylistLayout.addWidget(self.btn_addplaylist)
        self.gridLayoutPlay.addWidget(self.AddPlaylist, 0, 0)
        self.btn_addplaylist.clicked.connect(lambda : self.newPlaylist())
        
    
    def save(self):
        self.raw_playlists = []
        for pl in self.playlists:
            self.raw_playlists.append(pl.save())
        with open(path.join(self.fm.user_data_dir,"playlists.json"),"w") as file:
            file.write(json.dumps(self.raw_playlists))
    
    def load(self):
        try:
            with open(path.join(self.fm.user_data_dir,"playlists.json"),"r") as file:
                h = file.read()
            return json.loads(h)
        except:
            with open(path.join(self.fm.user_data_dir,"playlists.json"),"w") as file:
                file.write(json.dumps([]))
            return []
        
    def drawPlaylists(self):
        
        for pl in self.raw_playlists:
            try:
                doShuffle=pl["doShuffle"]
            except:
                doShuffle=False
            n = PlaylistItem(pl["name"],pl["musics"],pl["thumb"],pl["plid"],self.gridLayoutPlay,self.scrollArea,self.mp,self.ui,self,doShuffle=doShuffle)
            self.playlists.append(n)
            self.mapping[pl["plid"]] = n
            self.playlists[-1].create()

    def reloadall(self):
        self.logger.info("Starting reloading all playlists")
        for pl in self.playlists:
            self.logger.info(f"Reloading {pl}")
            pl.reload()
        self.logger.info(f"Reload finished.")

    def appExit(self):
        self.logger.info("Exiting Playlist Manager")
        self.save()
            
        
    def newPlaylist(self):
        n = PlaylistItem("New Playlist",[],"https://d1csarkz8obe9u.cloudfront.net/posterpreviews/lo-fi-car-dashboard-album-song-cover-art-design-template-683f268e5d3f08906caef3ac37efc21a_screen.jpg",str(time.time()),self.gridLayoutPlay,self.scrollArea,self.mp,self.ui,self)
        self.playlists.append(n)
        self.playlists[-1].create()
        self.playlists[-1].modify()
        self.save()

    def getNewPos(self):
        maxcol = 5
        self.nextPos[0]+=1
        if self.nextPos[0] > maxcol:
            self.nextPos[0] = 0
            self.nextPos[1] += 1
        return self.nextPos

class PlaylistItem():

    def __init__(self,name,musics,thumb,plid,layout,gridFrame,mp,ui,parent,doShuffle=False) -> None:
        self.nameI = name
        self.musicsI = self.getRealPlaylist(musics)
        self.raw_musics = musics
        self.plid = plid
        self.layout = layout
        self.gridFrame = gridFrame
        self.process = []
        self.mp = mp
        self.ui = ui
        self.pere = parent
        self.fm = FileManager()
        self.il = ImageLoader(self.fm)
        self.thumbI = thumb
        self.thumbUrl = self.il.get(self.thumbI)
        self.doShuffle = doShuffle
        

            

    def create(self):
        self.PlaylistTemplate = QtWidgets.QFrame(self.gridFrame)
        self.PlaylistTemplate.setMaximumSize(QtCore.QSize(150, 150))
        self.PlaylistTemplate.setMinimumSize(QtCore.QSize(150, 150))
        self.PlaylistTemplate.setStyleSheet("\n"
        "#PlaylistTemplate {\n"
        "  background-color: rgb(39, 44, 54);\n"
        "  border-radius: 15px;\n"
        '  border-image: url("'+self.thumbUrl+'") 0 0 0 0 stretch stretch;\n'
        "  background-repeat: no-reperat;\n"
        "  border: none;\n"
        "}")
        self.PlaylistTemplate.setObjectName("PlaylistTemplate")
        self.verticalLayout_13 = QtWidgets.QVBoxLayout(self.PlaylistTemplate)
        self.verticalLayout_13.setObjectName("verticalLayout_13")
        self.thumb = QtWidgets.QPushButton(self.PlaylistTemplate)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.thumb.sizePolicy().hasHeightForWidth())
        self.thumb.setSizePolicy(sizePolicy)
        self.thumb.setStyleSheet("QPushButton {\n"
        "    border: none;\n"
        "    background-color: rgba(27, 29, 35,0);\n"
        "}")
        self.thumb.setText("")
        self.thumb.setObjectName("thumb")
        self.verticalLayout_13.addWidget(self.thumb)
        self.options = QtWidgets.QFrame(self.PlaylistTemplate)
        self.options.setStyleSheet("\n"
        "background-color: rgba(0, 0, 0, 200);\nborder-radius: 5px;")
        self.options.setObjectName("options")
        self.optionLayout = QtWidgets.QHBoxLayout(self.options)
        self.optionLayout.setObjectName("optionLayout")
        self.plname = QtWidgets.QLabel(self.options)
        font = QtGui.QFont()
        font.setPointSize(8)
        font.setBold(True)
        font.setWeight(75)
        self.plname.setFont(font)
        self.plname.setTextFormat(QtCore.Qt.AutoText)
        self.plname.setScaledContents(False)
        self.plname.setIndent(-1)
        self.plname.setObjectName("plname")
        self.optionLayout.addWidget(self.plname)
        self.modifyPlaylist = QtWidgets.QPushButton(self.options)
        self.modifyPlaylist.setMaximumSize(QtCore.QSize(20, 16777215))
        self.modifyPlaylist.setStyleSheet("QPushButton {\n"
        "    background-position: center;\n"
        "    background-repeat: no-reperat;\n"
        "    border: none;\n"
        "    background-image: url(:/16x16/icons/16x16/cil-media-play.png);\n"
        "}\n")
        self.modifyPlaylist.setText("")
        self.modifyPlaylist.setObjectName("modifyPlaylist")
        self.optionLayout.addWidget(self.modifyPlaylist)
        self.verticalLayout_13.addWidget(self.options)

        pos = self.pere.getNewPos()

        self.layout.addWidget(self.PlaylistTemplate, pos[1], pos[0])

        self.thumb.clicked.connect(lambda: self.modify())
        self.modifyPlaylist.clicked.connect(lambda:self.play())

        self.plname.setText(self.nameI)
        

    def getRealPlaylist(self,fromjson):
        return [api.YtMusic(data=x) for x in fromjson]
    
    def reload(self):
        a = []
        for i in self.raw_musics:
            a.append(api.YtMusic(furl=i["furl"]))
        self.musicsI = a
    
    def save(self):
        return {"name":self.nameI,"musics":[x.save() for x in self.musicsI],"thumb":self.thumbI,"plid":self.plid,"doShuffle":self.doShuffle}

    def play(self):
        self.mp.addAndPlay(self.musicsI)

    def autoplay(self):
        if self.doShuffle: self.shuffle()
        else: self.play()
    
    def shuffle(self):
        shuffled_list = random.sample(self.musicsI, len(self.musicsI))
        self.mp.addAndPlay(shuffled_list)
    
    def modify(self):

        try:
            self.ui.playlistEditName.textChanged.disconnect() 
            self.ui.playlistEditList.model().rowsMoved.disconnect() 
            self.ui.playlistEditAdd.clicked.disconnect() 
            self.ui.playlistEditChangeImage.clicked.disconnect() 
            self.ui.playlistEditDelete.clicked.disconnect() 
            self.ui.btn_peplay.clicked.disconnect() 
            self.ui.btn_pepshuffle.clicked.disconnect()
            self.ui.playlistEditList.removeListener()
            self.ui.checkBox_2.stateChanged.disconnect()
        except Exception as e:
            pass
        
        self.ui.playlistEditName.textChanged.connect(self.getNewName)
        self.ui.playlistEditList.model().rowsMoved.connect(self.updateList)
        self.ui.playlistEditAdd.clicked.connect(lambda: self.handlerNM())
        self.ui.playlistEditChangeImage.clicked.connect(lambda: self.handlerCI())
        self.ui.playlistEditDelete.clicked.connect(lambda: self.delete(self))
        self.ui.btn_peplay.clicked.connect(lambda: self.play())
        self.ui.btn_pepshuffle.clicked.connect(lambda: self.shuffle())
        self.ui.playlistEditList.addListener(self.updateList,self.playAt)
        self.ui.checkBox_2.stateChanged.connect(lambda: self.hanndleShuffle())
        


        self.ui.playlistEditList.clear()
        self.ui.playlistEditName.setText(self.nameI)
        self.ui.checkBox_2.setChecked(self.doShuffle)
        self.ui.playlistEditImage.setStyleSheet(f"""border-radius: 15px;\nborder-image: url("{self.thumbUrl}") 0 0 0 0 stretch stretch""")

        for m in self.musicsI:
            item = QtWidgets.QListWidgetItem(self.ui.playlistEditList)
            item.setData(QtCore.Qt.UserRole, (m,m.title, m.author, self.il.get(m.thumb)))
            self.ui.playlistEditList.addItem(item)
        self.ui.stackedWidget.setCurrentWidget(self.ui.page_editPlaylist)

    def getNewName(self,text):
        self.nameI = text
        self.plname.setText(text)
        self.save()

    def updateList(self):
        self.musicsI = self.ui.playlistEditList.getNewList()

    def hanndleShuffle(self):
        self.doShuffle = self.ui.checkBox_2.isChecked()
        self.save()

    def handlerNM(self):
        self.ui.playlistEditAdd.setEnabled(False)
        QtCore.QTimer.singleShot(100, lambda: self.ui.playlistEditAdd.setDisabled(False))
        CdialogWin(self.getNewMusic).setTxt("Please give a Youtube url to add to the playlist (This can took a while !) : ")

    def handlerCI(self):
        self.ui.playlistEditChangeImage.setEnabled(False)
        QtCore.QTimer.singleShot(100, lambda: self.ui.playlistEditChangeImage.setDisabled(False))
        CdialogWin(self.getNewImage).setTxt("Please give a image url for the new thumbnail : ")

    def getNewMusic(self,msg):
        ok = msg["user"]
        text = msg["msg"]
        if ok:
            m = api.YtMusic(text)
            self.musicsI.append(m)
            item = QtWidgets.QListWidgetItem(self.ui.playlistEditList)
            item.setData(QtCore.Qt.UserRole, (m,m.title, m.author, self.il.get(m.thumb)))
            self.ui.playlistEditList.addItem(item)

    def getNewImage(self,msg):
        ok = msg["user"]
        text = msg["msg"]
        if ok:
            self.thumbI = text
            self.thumbUrl = self.il.get(self.thumbI)

            self.PlaylistTemplate.setStyleSheet("\n"
            "#PlaylistTemplate {\n"
            "  background-color: rgb(39, 44, 54);\n"
            "  border-radius: 15px;\n"
            '  border-image: url("'+self.thumbUrl+'") 0 0 0 0 stretch stretch;\n'
            "  background-repeat: no-reperat;\n"
            "  border: none;\n"
            "}")
            self.ui.playlistEditImage.setStyleSheet(f"""border-radius: 15px;\nborder-image: url("{self.thumbUrl}") 0 0 0 0 stretch stretch""")
            self.save()

    def delete(self,i):

        i.PlaylistTemplate.setParent(None)
        self.ui.stackedWidget.setCurrentWidget(self.ui.page_playlists)
        self.pere.playlists.remove(i)
        del(i)

    def playAt(self,wo):
        row = self.ui.playlistEditList.row(wo)
        self.mp.addAndPlay(self.musicsI[row:])
            
        

class CustomListWidget(QListWidget):
    def __init__(self, type,wid, parent=None):
        super(CustomListWidget, self).__init__(parent)
        self.setIconSize(QtCore.QSize(124, 124))
        self.setDragDropMode(QtWidgets.QAbstractItemView.DragDrop)
        self.setDefaultDropAction(QtCore.Qt.MoveAction)
        self.setSelectionMode(QtWidgets.QAbstractItemView.ExtendedSelection)
        self.setAcceptDrops(True)
        self.table = {}
        self.wid = wid
        self.type = type
        self.listener = None
        self.listener2 = None
        self.model().rowsInserted.connect(
            self.handleRowsInserted, QtCore.Qt.QueuedConnection)

    def handleRowsInserted(self, parent, first, last):
        #self.logger.info(f"Adding item to CustomQListWidget '{self}' in '{self.type}'")
        for index in range(first, last + 1):
            item = self.item(index)
            data = item.data(QtCore.Qt.UserRole)
            
            if item is not None and self.itemWidget(item) is None and data is not None:
                id, index, name, icon = data
                
                widget = self.wid(self)
                widget.setTexte(index,name)
                widget.setIcone(icon)
                widget.setId(id)
                
                if self.wid == DraggableItem:
                    widget.deletetkt.clicked.connect(lambda: self.deleteMe(item))
                    widget.dplay.clicked.connect(lambda: self.listener2(item))
                
                item.setSizeHint(widget.sizeHint())
                self.table[widget] = item
                
                self.setItemWidget(item, widget)
                continue
    
    def dragMoveEvent(self, event):
        if ((target := self.row(self.itemAt(event.pos()))) ==
            (current := self.currentRow()) + 1 or
            (current == self.count() - 1 and target == -1)):
            event.ignore()
        else:
            super().dragMoveEvent(event)


    def getNewList(self):
        nl = []
        for x in range(self.count()):
            nl.append(self.item(x).data(QtCore.Qt.UserRole)[0])
        return nl
    
    def deleteMe(self,who):
        self.takeItem(self.row(who))
        self.listener()

    def addListener(self,fct,fct2):
        self.listener = fct
        self.listener2 = fct2

    def removeListener(self):
        self.listener = None
        self.listener2 = None

class DraggableItem(QtWidgets.QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.id = None

        self.nwidget = QFrame(self)

        self.nwidget.setMinimumSize(QtCore.QSize(0, 80))
        self.nwidget.setStyleSheet("background-color: rgb(39, 44, 54);\n"
        " border-radius: 15px;")
        self.nwidget.setObjectName("horizontalWidget")
        self.horizontalLayout_16 = QtWidgets.QHBoxLayout(self.nwidget)
        self.horizontalLayout_16.setObjectName("horizontalLayout_16")

        self.spacerItem2 = QtWidgets.QSpacerItem(10, 19, QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_16.addItem(self.spacerItem2)
        
        self.dplay = QtWidgets.QPushButton(self.nwidget)
        self.dplay.setText("")
        icon3 = QtGui.QIcon()
        icon3.addPixmap(QtGui.QPixmap(":/16x16/icons/16x16/cil-media-play.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.dplay.setIcon(icon3)
        self.horizontalLayout_16.addWidget(self.dplay)


        self.spacerItem2u = QtWidgets.QSpacerItem(10, 19, QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_16.addItem(self.spacerItem2u)

        self.label_3 = QtWidgets.QLabel(self.nwidget)
        self.label_3.setMinimumSize(QtCore.QSize(60, 7))
        self.label_3.setMaximumSize(QtCore.QSize(75, 75))
        self.label_3.setText("")
        self.label_3.setScaledContents(True)
        self.label_3.setObjectName("label_3")
        self.horizontalLayout_16.addWidget(self.label_3)
        spacerItem3 = QtWidgets.QSpacerItem(20, 20, QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_16.addItem(spacerItem3)
        self.label = QtWidgets.QLabel(self.nwidget)
        font = QtGui.QFont()
        font.setPointSize(12)
        font.setBold(True)
        font.setWeight(75)
        self.label.setFont(font)
        self.label.setScaledContents(False)
        self.label.setObjectName("label")
        self.horizontalLayout_16.addWidget(self.label)
        self.deletetkt = QtWidgets.QPushButton(self.nwidget)
        self.deletetkt.setText("")
        icon3 = QtGui.QIcon()
        icon3.addPixmap(QtGui.QPixmap(":/20x20/icons/20x20/cil-x-circle.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.deletetkt.setIcon(icon3)
        self.horizontalLayout_16.addWidget(self.deletetkt)

        self.spacerItem2ud = QtWidgets.QSpacerItem(100, 19, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_16.addItem(self.spacerItem2ud)

        self.allQHBoxLayout = QtWidgets.QHBoxLayout()
        self.allQHBoxLayout.addWidget(self.nwidget, 0)
        self.setLayout(self.allQHBoxLayout)
        self.pere = parent
        

    def setTexte(self, text1,text2):
        maxcara = 60
        a = f"    {text1} - By {text2}"
        if len(a)>maxcara:
            tm = maxcara-(len(a)-len(text1))
            text1 = text1[:tm]+"..."

        self.label.setText(f"    {text1} - By {text2}")

    def setIcone(self, imagePath):
        self.label_3.setStyleSheet(f"""border-radius: 2px;\nborder-image: url("{imagePath}") 0 0 0 0 stretch stretch""")

    def setId(self, id):
        self.id = id
        

class DraggableGame(QtWidgets.QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.id = None

        self.nwidget = QFrame(self)

        self.nwidget.setMinimumSize(QtCore.QSize(0, 80))
        self.nwidget.setStyleSheet("background-color: rgb(39, 44, 54);\n"
        " border-radius: 15px;")
        self.nwidget.setObjectName("horizontalWidget")
        self.horizontalLayout_16 = QtWidgets.QHBoxLayout(self.nwidget)
        self.horizontalLayout_16.setObjectName("horizontalLayout_16")
        self.spacerItem2 = QtWidgets.QSpacerItem(10, 19, QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_16.addItem(self.spacerItem2)
        self.label_3 = QtWidgets.QLabel(self.nwidget)
        self.label_3.setMinimumSize(QtCore.QSize(75, 75))
        self.label_3.setMaximumSize(QtCore.QSize(75, 75))
        self.label_3.setText("")
        self.label_3.setScaledContents(True)
        self.label_3.setObjectName("label_3")
        self.horizontalLayout_16.addWidget(self.label_3)
        spacerItem3 = QtWidgets.QSpacerItem(20, 20, QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_16.addItem(spacerItem3)
        self.label = QtWidgets.QLabel(self.nwidget)
        font = QtGui.QFont()
        font.setPointSize(16)
        font.setBold(True)
        font.setWeight(75)
        self.label.setFont(font)
        self.label.setScaledContents(False)
        self.label.setObjectName("label")
        self.horizontalLayout_16.addWidget(self.label)

        self.spacerItem2e = QtWidgets.QSpacerItem(20, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_16.addItem(self.spacerItem2e)


        self.deletetkt = QtWidgets.QPushButton(self.nwidget)
        self.deletetkt.setText("")
        icon3 = QtGui.QIcon()
        icon3.addPixmap(QtGui.QPixmap(":/20x20/icons/20x20/cil-x-circle.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.deletetkt.setIcon(icon3)

        self.pushButton_3 = QtWidgets.QPushButton(self.nwidget)
        self.pushButton_3.setMinimumSize(QtCore.QSize(80, 80))
        self.pushButton_3.setMaximumSize(QtCore.QSize(80, 8015))
        self.pushButton_3.setSizeIncrement(QtCore.QSize(0, 0))
        self.pushButton_3.setText("")
        icon5 = QtGui.QIcon()
        icon5.addPixmap(QtGui.QPixmap(":/16x16/icons/16x16/cil-chevron-circle-right-alt.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.pushButton_3.setIcon(icon5)
        self.pushButton_3.setObjectName("pushButton_3")
        self.horizontalLayout_16.addWidget(self.pushButton_3)

        self.horizontalLayout_16.addWidget(self.deletetkt)

        self.spacerItem2d = QtWidgets.QSpacerItem(10, 19, QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_16.addItem(self.spacerItem2d)

        self.allQHBoxLayout = QtWidgets.QHBoxLayout()
        self.allQHBoxLayout.addWidget(self.nwidget, 0)
        self.setLayout(self.allQHBoxLayout)
        self.pere = parent
        self.name = ""
        

    def setTexte(self, text1,text2):
        a = f"    {text1}"
        self.label.setText(a)
        self.name = text1


    def setIcone(self, imagePath):
        self.label_3.setStyleSheet(f"""border-radius: 15px;\nborder-image: url("{imagePath}") 0 0 0 0 stretch stretch""")

    def setId(self, id):
        self.id = id
        self.pushButton_3.clicked.connect(lambda: self.id(self))

class DraggableAdd(QtWidgets.QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.id = None

        self.nwidget = QFrame(self)

        self.nwidget.setMinimumSize(QtCore.QSize(0, 80))
        self.nwidget.setStyleSheet("background-color: rgb(39, 44, 54);\n"
        " border-radius: 15px;")
        self.nwidget.setObjectName("horizontalWidget")
        self.horizontalLayout_16 = QtWidgets.QHBoxLayout(self.nwidget)
        self.horizontalLayout_16.setObjectName("horizontalLayout_16")
        self.spacerItem2 = QtWidgets.QSpacerItem(10, 19, QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_16.addItem(self.spacerItem2)
        self.label_3 = QtWidgets.QLabel(self.nwidget)
        self.label_3.setMinimumSize(QtCore.QSize(75, 75))
        self.label_3.setMaximumSize(QtCore.QSize(75, 75))
        self.label_3.setText("")
        self.label_3.setScaledContents(True)
        self.label_3.setObjectName("label_3")
        self.horizontalLayout_16.addWidget(self.label_3)
        spacerItem3 = QtWidgets.QSpacerItem(20, 20, QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_16.addItem(spacerItem3)
        self.label = QtWidgets.QLabel(self.nwidget)
        font = QtGui.QFont()
        font.setPointSize(16)
        font.setBold(True)
        font.setWeight(75)
        self.label.setFont(font)
        self.label.setScaledContents(False)
        self.label.setObjectName("label")
        self.horizontalLayout_16.addWidget(self.label)

        self.spacerItem2e = QtWidgets.QSpacerItem(20, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_16.addItem(self.spacerItem2e)


        self.deletetkt = QtWidgets.QPushButton(self.nwidget)
        self.deletetkt.setText("")
        icon3 = QtGui.QIcon()
        icon3.addPixmap(QtGui.QPixmap(":/20x20/icons/20x20/cil-library-add.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.deletetkt.setIcon(icon3)



        self.horizontalLayout_16.addWidget(self.deletetkt)

        self.spacerItem2d = QtWidgets.QSpacerItem(10, 19, QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_16.addItem(self.spacerItem2d)

        self.allQHBoxLayout = QtWidgets.QHBoxLayout()
        self.allQHBoxLayout.addWidget(self.nwidget, 0)
        self.setLayout(self.allQHBoxLayout)
        self.pere = parent
        

    def setTexte(self, text1,text2):
        a = f"    {text1}"
        self.label.setText(a)
        self.name = text1


    def setIcone(self, imagePath):
        self.label_3.setStyleSheet(f"""border-radius: 15px;\nborder-image: url("{imagePath}") 0 0 0 0 stretch stretch""")

    def setId(self, id):
        self.id = id
        self.deletetkt.clicked.connect(lambda: self.id(self))

class DraggablePlaylist(QtWidgets.QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.id = None

        self.nwidget = QFrame(self)

        self.nwidget.setMinimumSize(QtCore.QSize(0, 80))
        self.nwidget.setStyleSheet("background-color: rgb(39, 44, 54);\n"
        " border-radius: 15px;")
        self.nwidget.setObjectName("horizontalWidget")
        self.horizontalLayout_16 = QtWidgets.QHBoxLayout(self.nwidget)
        self.horizontalLayout_16.setObjectName("horizontalLayout_16")
        self.spacerItem2 = QtWidgets.QSpacerItem(10, 19, QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Minimum)
        
        self.label_3 = QtWidgets.QLabel(self.nwidget)
        self.label_3.setMinimumSize(QtCore.QSize(75, 75))
        self.label_3.setMaximumSize(QtCore.QSize(75, 75))
        self.label_3.setText("")
        self.label_3.setScaledContents(True)
        self.label_3.setObjectName("label_3")
        
        self.spacerItem3 = QtWidgets.QSpacerItem(20, 20, QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Minimum)
        
        self.label = QtWidgets.QLabel(self.nwidget)
        font = QtGui.QFont()
        font.setPointSize(16)
        font.setBold(True)
        font.setWeight(75)
        self.label.setFont(font)
        self.label.setScaledContents(False)
        self.label.setObjectName("label")
    
        self.spacerItem2e = QtWidgets.QSpacerItem(20, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)

        self.deletetkt = QtWidgets.QPushButton(self.nwidget)
        self.deletetkt.setText("")

        self.spacerItem2d = QtWidgets.QSpacerItem(10, 19, QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Minimum)

        self.allQHBoxLayout = QtWidgets.QHBoxLayout()
        self.allQHBoxLayout.addWidget(self.nwidget, 0)
        self.setLayout(self.allQHBoxLayout)
        self.pere = parent
        

    def setTexte(self, text1,text2):
        a = f"    {text1}"
        self.label.setText(a)
        self.name = text1
        self.plid = text2


    def setIcone(self, imagePath):
        self.thumb = imagePath
        self.label_3.setStyleSheet(f"""border-radius: 15px;\nborder-image: url("{imagePath}") 0 0 0 0 stretch stretch""")

    def setId(self, id):
        self.id = id[0]
        self.left = id[1]
        self.deletetkt.clicked.connect(lambda: self.id(self,self.left))
        if self.left:
            icon3 = QtGui.QIcon()
            icon3.addPixmap(QtGui.QPixmap(":/16x16/icons/16x16/cil-chevron-circle-right-alt.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
            self.deletetkt.setIcon(icon3)
            self.horizontalLayout_16.addItem(self.spacerItem2)
            self.horizontalLayout_16.addWidget(self.label_3)
            self.horizontalLayout_16.addItem(self.spacerItem3)
            self.horizontalLayout_16.addWidget(self.label)
            self.horizontalLayout_16.addItem(self.spacerItem2e)
            self.horizontalLayout_16.addWidget(self.deletetkt)
            self.horizontalLayout_16.addItem(self.spacerItem2d)
        else:
            icon3 = QtGui.QIcon()
            icon3.addPixmap(QtGui.QPixmap(":/16x16/icons/16x16/cil-chevron-circle-left-alt.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
            self.deletetkt.setIcon(icon3)
            self.horizontalLayout_16.addItem(self.spacerItem2)
            self.horizontalLayout_16.addWidget(self.deletetkt)
            self.horizontalLayout_16.addItem(self.spacerItem2d)
            self.horizontalLayout_16.addWidget(self.label_3)
            self.horizontalLayout_16.addItem(self.spacerItem3)
            self.horizontalLayout_16.addWidget(self.label)
            self.horizontalLayout_16.addItem(self.spacerItem2e)
            
class FileManager():
    def __init__(self) -> None:  
        self.appname = "deltaplayer"
        self.appauthor = "CGS"
        dirs = PlatformDirs(self.appname, self.appauthor)
        self.user_data_dir = dirs.user_data_dir
        self.user_cache_dir = dirs.user_cache_dir
        self.user_log_dir = dirs.user_log_dir
        os.makedirs(self.user_data_dir,exist_ok=True)
        os.makedirs(self.user_cache_dir,exist_ok=True)
        os.makedirs(self.user_log_dir,exist_ok=True)

class ExitProgram():
    def __init__(self,managers) -> None:
        self.managers = managers

    def appExit(self):
        for i in self.managers:
            i.appExit()

class SettingManager():

    def __init__(self,ui,logger) -> None:
        self.ui = ui
        self.BASESETTINGS = {"volume":50}
        self.fm = FileManager()
        self.settings = self.load()
        self.logger = logger

        
        

    def save(self):
        with open(path.join(self.fm.user_data_dir,"settings.json"),"w") as file:
            file.write(json.dumps(self.settings))
    
    def load(self):
        try:
            with open(path.join(self.fm.user_data_dir,"settings.json"),"r") as file:
                h = file.read()
            return json.loads(h)
        except:
            with open(path.join(self.fm.user_data_dir,"settings.json"),"w") as file:
                file.write(json.dumps(self.BASESETTINGS))
            return self.BASESETTINGS
        
    def appExit(self):
        self.logger.info("Exiting Setting Manager")
        self.save()

class GameManager():
    def __init__(self,ui,pm,logger) -> None:
        self.logger = logger
        self.ui = ui
        self.pm = pm
        self.fm = FileManager()
        self.il = ImageLoader(self.fm)
        self.gameDb = self.loadDb()
        self.games = self.load()
        self.currentGame = None

        ####### Game List
        self.ui.gameList = CustomListWidget(self.ui.gameListHolder,wid=DraggableGame)
        self.ui.gameList.setDragDropMode(QListWidget.InternalMove)
        self.ui.gameList.setSelectionMode(QListWidget.ExtendedSelection)
        #self.ui.gameList.setSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)

        self.ui.gameList.setStyleSheet("border: 0px")
        self.ui.gameList.setObjectName("gameList")
        self.ui.verticalLayout_16.addWidget(self.ui.gameList)

        self.scrollArea = QtWidgets.QScrollArea(self.ui.gameListHolder)
        self.scrollArea.setWidgetResizable(True)
        self.scrollArea.setStyleSheet("border-radius: 0px;")
        self.scrollAreaWidgetContents = QtWidgets.QWidget()
        self.gridLayoutPlay = QtWidgets.QGridLayout(self.scrollAreaWidgetContents)
        self.scrollArea.setWidget(self.scrollAreaWidgetContents)
        #self.ui.verticalLayout_16.addWidget(self.scrollArea)
        ########

        ######## Add List
        self.ui.addList = CustomListWidget(self.ui.gameAdd,wid=DraggableAdd)
        self.ui.addList.setDragDropMode(QListWidget.InternalMove)
        self.ui.addList.setSelectionMode(QListWidget.ExtendedSelection)
        self.ui.addList.setSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)


        self.ui.horizontalLayout_16.addWidget(self.ui.addList)
        #self.ui.addList.setStyleSheet("border: 0px")
        self.ui.addList.setObjectName("addList")
        

        self.scrollArea5 = QtWidgets.QScrollArea(self.ui.addList)
        self.scrollArea5.setWidgetResizable(True)
        self.scrollArea5.setStyleSheet("border-radius: 0px;")
        self.scrollAreaWidgetContents5 = QtWidgets.QWidget()
        self.gridLayoutPlay7 = QtWidgets.QGridLayout(self.scrollAreaWidgetContents5)
        self.scrollArea5.setWidget(self.scrollAreaWidgetContents5)
        #self.ui.verticalLayout_16.addWidget(self.scrollArea)
        self.ui.addList.setVisible(False)
        self.ui.gameAddSearch.setVisible(False)
        #######

        #### EDIT1
        self.ui.gameEdit1 = CustomListWidget(self.ui.holder1,wid=DraggablePlaylist)
        self.ui.gameEdit1.setDragDropMode(QListWidget.InternalMove)
        self.ui.gameEdit1.setSelectionMode(QListWidget.ExtendedSelection)
        self.ui.gameEdit1.setSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)


        self.ui.verticalLayout_19.addWidget(self.ui.gameEdit1)
        self.ui.gameEdit1.setStyleSheet("border: 5px; background: transparent")
        self.ui.gameEdit1.setObjectName("gameEdit1")
        

        self.scrollAreagameEdit1 = QtWidgets.QScrollArea(self.ui.gameEdit1)
        self.scrollAreagameEdit1.setWidgetResizable(True)
        self.scrollAreagameEdit1.setStyleSheet("border-radius: 0px;")
        self.scrollAreaWidgetContentsgameEdit1 = QtWidgets.QWidget()
        self.gridLayoutPlayEdit1 = QtWidgets.QGridLayout(self.scrollAreaWidgetContentsgameEdit1)
        self.scrollAreagameEdit1.setWidget(self.scrollAreaWidgetContentsgameEdit1)
        #self.ui.verticalLayout_19.addWidget(self.scrollArea)


        #### EDIT2
        self.ui.gameEdit2 = CustomListWidget(self.ui.holder2,wid=DraggablePlaylist)
        self.ui.gameEdit2.setDragDropMode(QListWidget.InternalMove)
        self.ui.gameEdit2.setSelectionMode(QListWidget.ExtendedSelection)
        self.ui.gameEdit2.setSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)

        self.ui.gameEdit2.setStyleSheet("border: 0px; background: transparent")
        self.ui.gameEdit2.setObjectName("gameEdit2")
        self.ui.verticalLayout_20.addWidget(self.ui.gameEdit2)

        self.scrollAreagameEdit2 = QtWidgets.QScrollArea(self.ui.gameEdit2)
        self.scrollAreagameEdit2.setWidgetResizable(True)
        self.scrollAreagameEdit2.setStyleSheet("border-radius: 0px;")
        self.scrollAreaWidgetContentsgameEdit2 = QtWidgets.QWidget()
        self.gridLayoutPlaygameEdit2 = QtWidgets.QGridLayout(self.scrollAreaWidgetContentsgameEdit2)
        self.scrollAreagameEdit2.setWidget(self.scrollAreaWidgetContentsgameEdit2)
        #self.ui.verticalLayout_16.addWidget(self.scrollArea)
        ###

        self.ui.btn_gameAdd.clicked.connect(lambda: self.handlerAG())
        self.abState = True

        self.ui.gameAddSearch.textChanged.connect(self.search)

        self.genAutocomplete()
        self.drawGames()

    def drawGames(self):
        self.ui.addList.clear()

        gaa = self.gameDb.copy()
        for gh in self.games:
            for i in gaa:
                if i["name"] == gh["name"]:
                    gaa.remove(i)

        for m in gaa:
            item = QtWidgets.QListWidgetItem(self.ui.addList)
            item.setData(QtCore.Qt.UserRole, (self.addGame,m["name"],m["proc"],self.il.get(m["img"])))
            self.ui.addList.addItem(item)

        self.ui.gameList.clear()

        for m in self.games:
            item = QtWidgets.QListWidgetItem(self.ui.gameList)
            item.setData(QtCore.Qt.UserRole, (self.modifyGame,m["name"],m["proc"],self.il.get(m["img"])))
            self.ui.gameList.addItem(item)



    def loadDb(self):
        try:
            with open(path.join(self.fm.user_data_dir,"gameDb.json"),"r") as file:
                h = file.read()
            return json.loads(h)
        except Exception as e:
            with open(path.join(self.fm.user_data_dir,"gameDb.json"), 'wb') as f:
                f.write(requests.get("https://raw.githubusercontent.com/elycraft/musicplayer/main/gameDb.json").content)
            self.logger.error(e)
            self.logger.info("Downloaded gameDb.")
            with open(path.join(self.fm.user_data_dir,"gameDb.json"),"r") as file:
                h = file.read()
            return json.loads(h)
        
    def load(self):
        try:
            with open(path.join(self.fm.user_data_dir,"games.json"),"r") as file:
                h = file.read()
            return json.loads(h)
        except:
            with open(path.join(self.fm.user_data_dir,"games.json"),"w") as file:
                file.write(json.dumps([]))
            return []

    def save(self):
        with open(path.join(self.fm.user_data_dir,"games.json"),"w") as file:
            file.write(json.dumps(self.games))

    def handlerAG(self):
        #self.ui.btn_gameAdd.setEnabled(False)
        #QtCore.QTimer.singleShot(100, lambda: self.ui.btn_gameAdd.setDisabled(False))
        if self.abState:
            self.ui.gameAdd.setMinimumHeight(300)
            self.ui.gameAdd.setMaximumHeight(300)
            self.ui.gameAdd.adjustSize()
            self.ui.addList.setVisible(True)
            self.ui.gameAddSearch.setVisible(True)
            self.ui.btn_gameAdd.setText("Cancel")
            #self.search("")
            self.abState = False
            
        else:
            self.ui.gameAdd.setMinimumHeight(100)
            self.ui.gameAdd.setMaximumHeight(100)
            self.ui.gameAdd.adjustSize()
            self.ui.addList.setVisible(False)
            self.ui.gameAddSearch.setVisible(False)
            self.ui.btn_gameAdd.setText("Add game to librairy")
            self.abState = True

    def search(self,a):

        for x in range(self.ui.addList.count()):
            c = self.ui.addList.item(x)
            c.setHidden(True)

        f = []
        if a == "":
            f = self.gameDb
            for x in range(self.ui.addList.count()):
                c = self.ui.addList.item(x)
                c.setHidden(False)
        else:
            res = self.autocomplete.search(word=a, max_cost=3, size=5)
            for i in res:
                f.append(i[0])
            g = {}
            for x in self.ui.addList.table:
                g[x.name] = self.ui.addList.table[x]
            for i in f:
                c = g[i]
                c.setHidden(False)
 
    def genAutocomplete(self):
        d,words,e = self.genlist()
        self.d = d
        self.e = e
        self.autocomplete = AutoComplete(words=words)

    def genlist(self):
        d = {}
        d2 = {}
        e = []
        for g in self.gameDb:
            e.append(g["name"])
            d2[g["name"]] = {}
            d[g["name"]] = g
        return (d,d2,e)

    def addGame(self,wo):
        m = wo.name
        for i in self.gameDb:
            if i["name"] == m:
                item = QtWidgets.QListWidgetItem(self.ui.gameList)
                item.setData(QtCore.Qt.UserRole, (self.modifyGame,i["name"],i["proc"],self.il.get(i["img"])))
                self.ui.gameList.addItem(item)
                self.ui.addList.takeItem(self.ui.addList.row(self.ui.addList.table[wo]))
                i["playlists"] = []
                self.games.append(i)

    def modifyGame(self,wo):
        self.currentGame = wo.name
        gi = None
        for i in self.games:
            if i["name"] == wo.name:
                gi = i

        self.ui.gameTitle.setText("    "+gi["name"])
        self.ui.gameImage.setStyleSheet(f"""border-radius: 15px;\nborder-image: url("{self.il.get(gi["img"])}") 0 0 0 0 stretch stretch""")
        self.ui.gameEdit1.clear()
        self.ui.gameEdit2.clear()

        for pl in self.pm.raw_playlists:
            if pl["plid"] in gi["playlists"]:
                item = QtWidgets.QListWidgetItem(self.ui.gameEdit2)
                item.setData(QtCore.Qt.UserRole, ([self.handleDlPl,False],pl["name"],pl["plid"],self.il.get(pl["thumb"])))
                self.ui.gameEdit2.addItem(item)
            else:    
                item = QtWidgets.QListWidgetItem(self.ui.gameEdit1)
                item.setData(QtCore.Qt.UserRole, ([self.handleDlPl,True],pl["name"],pl["plid"],self.il.get(pl["thumb"])))
                self.ui.gameEdit1.addItem(item)
                    
        self.ui.stackedWidget.setCurrentWidget(self.ui.page_editGame)

    def handleDlPl(self,wo,left):
        if left:
            self.ui.gameEdit1.takeItem(self.ui.gameEdit1.row(self.ui.gameEdit1.table[wo]))
            item = QtWidgets.QListWidgetItem(self.ui.gameEdit2)
            item.setData(QtCore.Qt.UserRole, ([self.handleDlPl,False],wo.name,"",wo.thumb))
            self.ui.gameEdit2.addItem(item)
            for i in self.games:
                if i["name"] == self.currentGame:
                    i["playlists"].append(wo.plid)
        else:
            self.ui.gameEdit2.takeItem(self.ui.gameEdit2.row(self.ui.gameEdit2.table[wo]))
            item = QtWidgets.QListWidgetItem(self.ui.gameEdit1)
            item.setData(QtCore.Qt.UserRole, ([self.handleDlPl,True],wo.name,"",wo.thumb))
            self.ui.gameEdit1.addItem(item)
            for i in self.games:
                if i["name"] == self.currentGame:
                    i["playlists"].remove(wo.plid)


    def appExit(self):
        self.logger.info("Exiting Game Manager")
        self.save()

class DetectorManager(QObject):

    def __init__(self,gm,pm,bm,logger) -> None:
        QObject.__init__(self)
        self.logger = logger
        self.gm = gm
        self.pm = pm
        self.bm = bm
        self.interrupt = False

        self.start()

    def verifier_jeu(self,jeux): #renvoie le dico des jeu en train de tourner
        save = detector.get_process()
        jt = []
        for jeu in jeux:
            if jeu["proc"] in save:
                jt.append(jeu)
        return jt
    
    def start(self):
        def worker():
            self.interrupt = False
            self.alreadyp = []
            while True:
                for waittime in range(10):
                    if self.interrupt: break
                    time.sleep(1)
                if self.interrupt: break
                r = self.verifier_jeu(self.gm.games)
                if len(r) == 0: continue
                for g in r:
                    k = None
                    for i in self.alreadyp:
                        if g["name"] == i["name"]:
                            k = i
                            break
                    if k == None:
                        k = {"name":g["name"],"time":time.time()}
                        self.alreadyp.append(k)
                        rpl = []
                        for i in g["playlists"]:
                            for a in self.pm.raw_playlists:
                                if a["plid"] == i: rpl.append(a)
                        for i in rpl:
                            self.pm.mapping[i["plid"]].play()
                    elif (time.time()-k["time"])>1800:
                        k["time"] = time.time()
                        rpl = []
                        for i in g["playlists"]:
                            for a in self.pm.raw_playlists:
                                if a["plid"] == i: rpl.append(a)
                        for i in rpl:
                            self.pm.mapping[i["plid"]].autoplay()
                    else:
                        pass
                    break

        t = threading.Thread(target=worker)
        t.start()

    def appExit(self):
        self.interrupt = True
        self.logger.info("Exiting Detector Manager")


class ImageLoader():
    
    def __init__(self,fm=None) -> None:
        if not fm:self.fm = FileManager()
        else: self.fm = fm
        

    def get(self,url):
        ph = path.join(self.fm.user_cache_dir,self.getfn(url))
        if not path.exists(ph):
            with open(ph, 'wb') as f:
                f.write(requests.get(url).content)
        return ph.replace("\\","/")

    def getfn(self,url):
        return hashlib.md5(url.encode('utf-8')).hexdigest()+"."+url.split("/")[-1].split("?")[0].split(".")[-1]

