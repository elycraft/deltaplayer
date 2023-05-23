import threading
import subprocess
import os
import time
import pafy

class Player():

    def __init__(self,caller,caller2) -> None:
        self.playing = False
        self.paused = False
        self.startTime = None
        self.queue = []
        self.process = None
        self.interrupt = False
        self.current = None
        self.update = caller
        self.updateTime = caller2
        self.time = (0,0)
        self.sound = 99

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
                if len(self.queue)==0:
                    if self.interrupt: break
                    continue
                time.sleep(0.5)
                self.current = self.queue[0]
                del self.queue[0]
                self.update()
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
                if self.paused:
                    if self.interrupt or self.playing==False: break
                    continue
                time.sleep(0.5)
                t1 = self.time[0]+0.5
                try:t2 = t1*100/self.current.duration
                except:t2 = 0
                self.time=(t1,t2)
                self.updateTime()
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
        self.stopPlayQueue()
        if type(element) == type([]):
            self.queue += element
        else:
            self.queue = [element] + self.queue
        time.sleep(0.5)
        self.playQueue()      
    
    def stopPlayQueue(self):
        self.interrupt = True
        self.stop()
    
    def skip(self):
        self.stopPlayQueue()
        time.sleep(0.5)
        self.playQueue()  

    def setSound(self):
        self.sendToMpv("""{ "command": ["set_property","volume","""+str(self.sound)+"""] }""")
    
    def updateSound(self,value):
        self.sound = value
        self.setSound()

class YtMusic:

    def __init__(self,furl=None,data=None) -> None:
        if data:
            self.furl = data["furl"]
            self.title = data["title"]
            self.duration = data["length"]
            self.thumb = data["thumb"]
        else:
            video = pafy.new(furl)
            self.furl = furl
            self.title = video.title
            self.duration = video.length
            self.thumb = video.thumb

    def __repr__(self):
        return self.title
    
    def get_url(self):
        
        video = pafy.new(self.furl)
        self.url = video.audiostreams[0]
        return self.url.url
    
    def save(self):
        return {"furl":self.furl,"title":self.title,"length":self.duration,"thumb":self.thumb}

class Playlist():

    def __init__(self,name,process,musics) -> None:
        self.name = name
        self.process = process
        self.musics = musics
    
    def get_json(self):
        return {"name":self.name,"process":self.process,"musics":self.musics}


if __name__ == "__main__":        
    p = Player()
    #p.sendToMpv('''{ "command": ["stop"] }''')

    p.addToQueue(YtMusic("https://www.youtube.com/watch?v=sgKK0YRQyMQ"))
    p.addToQueue(YtMusic("https://www.youtube.com/watch?v=GDoiNIg28jg"))

    while True:
        print("A - Add to queue\nS - Stop playing queue\nP - Start playing queue\nD - Direct play")
        j = input("> ")
        j = j.lower()
        if j == "a":
            j = input("> ")
            p.addToQueue(YtMusic(j))
        if j == "s":
            p.stopPlayQueue()
        if j == "p":
            p.pause()
        if j == "z":
            p.playQueue()
        if j == "d":
            j = input("> ")
            p.addAndPlay(YtMusic(j))
        if j == "q":
            p.stopPlayQueue()
            break
        if j == "n":
            p.skip()
        print(len(p.queue),p.queue,len(p.queue))
    
