import threading
import subprocess
import os
import time
import pafy
from PySide2.QtCore import QObject, Signal

class Player(QObject):

    update = Signal(int)
    updateTime = Signal(int)


    def __init__(self) -> None:
        QObject.__init__(self)
        self.playing = False
        self.paused = False
        self.startTime = None
        self.queue = []
        self.process = None
        self.interrupt = False
        self.current = None
        
        self.time = (0,0)
        self.sound = 99
        self.backQueue = []

    def sendToMpv(self,cmd):
        os.system(f'echo {cmd} >\\\.\pipe\mpvsocket')

    def play(self,music):
        url = music
        if self.playing:
            self.stop()
        self.time=(0,0)
        self.process = subprocess.Popen(["mpv", url,"--input-ipc-server=\\\.\pipe\mpvsocket","--force-window=no"])
        self.playing = True
        self.timeSong()
        self.startTime = time.time()
        time.sleep(0.2)
        self.setSound()
        self.process.wait()
        self.playing = False
    
    def stop(self):
        if not self.playing: return
        self.sendToMpv('''{ "command": ["stop"] }''')
        time.sleep(0.5)
        self.process.kill()
        self.process = None
        self.startTime = None
        self.playing = False

    def pause(self):
        if not self.playing: return
        if self.paused:
            self.sendToMpv('{ "command": ["set_property", "pause", false] }')
            self.paused = False
        else:
            self.sendToMpv('{ "command": ["set_property", "pause", true] }')
            self.paused = True

    def playQueue(self):
        def worker():
            self.interrupt = False
            while True:
                time.sleep(0)
                if len(self.queue)==0:
                    if self.interrupt: break
                    continue
                
                self.current = self.queue[0]
                self.backQueue.append(self.current)
                del self.queue[0]
                self.update.emit(1)
                self.play(self.current.get_url())
                self.current = None
                #self.update()
                if self.interrupt: break
        t = threading.Thread(target=worker)
        t.start()

    def timeSong(self):
        def worker():
            self.interrupt = False
            while True:
                time.sleep(0)
                if self.paused:
                    if self.interrupt or self.playing==False: break
                    continue
                time.sleep(0.5)
                t1 = self.time[0]+0.5
                try:t2 = t1*100/self.current.duration
                except:t2 = 0
                self.time=(t1,t2)
                self.updateTime.emit(1)
                if self.interrupt or self.playing==False: break
            self.time=(0,0)
        t = threading.Thread(target=worker)
        t.start()
    
    def addToQueue(self,element):
        if type(element) == type([]):
            self.queue += element
        else:
            self.queue.append(element)
    
    def addAndPlay(self,element):
        self.clearQueue()
        if type(element) == type([]):
            self.queue += element
        else:
            self.queue = [element] + self.queue
        time.sleep(0.5)   
    
    def stopPlayQueue(self):
        self.interrupt = True
        self.stop()

    def clearQueue(self):
        self.queue = []
        self.stop()
    
    def skip(self):
        self.stop()
        time.sleep(0.5) 

    def setSound(self):
        self.sendToMpv("""{ "command": ["set_property","volume","""+str(self.sound)+"""] }""")
    
    def updateSound(self,value):
        self.sound = value
        self.setSound()

class YtMusic:

    def __init__(self,furl=None,data=None) -> None:
        if data:
            self.id = data["id"]
            self.furl = data["furl"]
            self.title = data["title"]
            self.duration = data["length"]
            self.thumb = data["thumb"]
            self.author = data["author"]
        else:
            video = pafy.new(furl)
            self.id = video.videoid
            self.furl = furl
            self.title = video.title
            self.duration = video.length
            self.thumb = video.thumb
            self.author = video.author

    def __repr__(self):
        return self.title
    
    def get_url(self):
        
        video = pafy.new(self.furl)
        self.url = video.audiostreams[0]
        return self.url.url
    
    def save(self):
        return {"id":self.id,"furl":self.furl,"title":self.title,"length":self.duration,"thumb":self.thumb,"author":self.author}

class Playlist():

    def __init__(self,name,process,musics) -> None:
        self.name = name
        self.process = process
        self.musics = musics
    
    def get_json(self):
        return {"name":self.name,"process":self.process,"musics":self.musics}


if __name__ == "__main__":        
    p = Player("","")
    #p.sendToMpv('''{ "command": ["stop"] }''')

    p.sendToMpv('''{ "command": ["stop"] }''')
