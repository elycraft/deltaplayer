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
import requests
import json
from appdata import AppDataPaths



class Functions(MainWindow):
    pass

class PlayBarManager():

    def __init__(self,ui) -> None:
        self.ui = ui

        self.mp = api.Player(self.updateUi,self.updateTime)
        self.mp.playQueue()

        self.ui.btn_rewind.clicked.connect(lambda: self.btnRewind())
        self.ui.btn_pause.clicked.connect(lambda: self.btnPause())
        self.ui.btn_skip.clicked.connect(lambda: self.btnSkip())

        self.soundSlider = self.ui.soundSlider
        self.soundSlider.valueChanged.connect(self.sliderSound)
        self.soundSlider.setValue(50)

        self.updateTime()
        self.updateUi()
        
    
    def appExit(self):
        print("Exiting API Player")
        self.mp.stopPlayQueue()

    def btnPause(self):
        print("Play")
        self.mp.pause()

    def btnRewind(self):
        self.mp.addToQueue(api.YtMusic("https://www.youtube.com/watch?v=ItBOHxAOgYc"))

    def btnSkip(self):
        print("Skip")
        self.mp.skip()
    
    def sliderSound(self):
        self.mp.updateSound(self.soundSlider.value())
    
    def updateTime(self):
        try: 
            a = strftime("%H:%M:%S", gmtime(self.mp.time[0]))
            b = strftime("%H:%M:%S", gmtime(self.mp.current.duration))
            self.ui.Bar.setFormat(f"{a}/{b}")
        except: self.ui.Bar.setFormat(f":/:")
        self.ui.Bar.setValue(self.mp.time[1])
    
    def updateUi(self):
        try: self.ui.MusicName.setText(str(self.mp.current.title)) 
        except: self.ui.MusicName.setText(str(""))
        try:
            image = QtGui.QImage()
            image.loadFromData(requests.get(self.mp.current.thumb).content)
            self.ui.MusicPoster.setPixmap(QPixmap(image))
            print(self.mp.current.thumb)
        except Exception as e:
            print(e)
            self.ui.MusicPoster.setPixmap(QtGui.QPixmap())




class PlaylistManager():

    def __init__(self,ui,mp) -> None:
        self.ui = ui
        self.raw_playlists = self.load()
        self.playlists = []
        self.mp = mp

        self.drawPlaylists()
        
    
    def save(self):
        self.raw_playlists = []
        for pl in self.playlists:
            self.raw_playlists.append(pl.save())
        with open("playlists.json","w") as file:
            file.write(json.dumps(self.raw_playlists))
    
    def load(self):
        try:
            with open("playlists.json","r") as file:
                h = file.read()
            return json.loads(h)
        except:
            with open("playlists.json","w") as file:
                file.write(json.dumps([]))
            return []
        
    def drawPlaylists(self):
        for pl in self.raw_playlists:
            n = PlaylistItem(pl["name"],pl["musics"],pl["thumb"],self.ui.gridLayout_3,self.ui.gridFrame,self.mp)
            self.playlists.append(n)
            self.playlists[-1].create()

    def appExit(self):
        print("Exiting Playlist Manager")
        self.save()
            
        
    def newPlaylist(self,name,musics):
        pl = ""

class PlaylistItem():

    def __init__(self,name,musics,thumb, layout,gridFrame,mp) -> None:
        self.nameI = name
        self.thumbI = thumb
        self.musicsI = self.getRealPlaylist(musics)
        self.layout = layout
        self.gridFrame = gridFrame
        self.process = []
        self.mp = mp

    def create(self):
        self.PlaylistTemplate = QtWidgets.QFrame(self.gridFrame)
        self.PlaylistTemplate.setMaximumSize(QtCore.QSize(150, 150))
        self.PlaylistTemplate.setStyleSheet("\n"
        "#PlaylistTemplate {\n"
        "  background-color: rgb(39, 44, 54);\n"
        "  border-radius: 15px;\n"
        "  border-image: url(https://www.adobe.com/fr/express/create/cover/media_1a06c14de66bef800d4fb40eaa2caeadc66eed471.jpeg) 0 0 0 0 stretch stretch;\n"
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
        "background-color: rgba(0, 0, 0, 2);")
        self.options.setObjectName("options")
        self.optionLayout = QtWidgets.QHBoxLayout(self.options)
        self.optionLayout.setObjectName("optionLayout")
        self.plname = QtWidgets.QLabel(self.options)
        font = QtGui.QFont()
        font.setPointSize(7)
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
        "    background-image: url(:/16x16/icons/16x16/cil-transfer.png);\n"
        "}\n"
        "QPushButton:hover {\n"
        "    background-color: rgb(33, 37, 43);\n"
        "}\n"
        "QPushButton:pressed {    \n"
        "    background-color: rgb(85, 170, 255);\n"
        "}")
        self.modifyPlaylist.setText("")
        self.modifyPlaylist.setObjectName("modifyPlaylist")
        self.optionLayout.addWidget(self.modifyPlaylist)
        self.verticalLayout_13.addWidget(self.options)
        self.layout.addWidget(self.PlaylistTemplate, 0, 0, 1, 1)

        self.thumb.clicked.connect(lambda: self.play())
        self.plname.setText(self.nameI)

    def getRealPlaylist(self,fromjson):
        return [api.YtMusic(data=x) for x in fromjson]
    
    def save(self):
        return {"name":self.nameI,"musics":[x.save() for x in self.musicsI],"thumb":self.thumbI}

    def play(self):
        self.mp.addToQueue(self.musicsI)

        
class FileManager():
    def __init__(self) -> None:  
        self.app_paths = AppDataPaths('deltaplay')
        self.app_paths.setup()
        print(self.app_paths.app_data_path)

class ExitProgram():
    def __init__(self,managers) -> None:
        self.managers = managers

    def appExit(self):
        for i in self.managers:
            i.appExit()
