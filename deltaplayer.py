import threading
import api
import detector
import json
import time
import sys

INSTRUCTION = """
P  - Met en pause
S  - Skip la musique
NP - Nouvelle playist
SP - Monter les playlists existantes

STOP    - Arrete l'application
PROCESS - Montre les processus actifs sur le pc 
"""

class Playlist():

    def __init__(self,name,process,musics) -> None:
        self.name = name
        self.process = process
        self.musics = musics
    
    def get_json(self):
        return {"name":self.name,"process":self.process,"musics":self.musics}
    
    def getRealPlaylist(self,fromjson):
        return [api.YtMusic(x) for x in fromjson["musics"]]

def savep():
    with open("playlists.json","w") as file:
        file.write(json.dumps(myplaylists))
    
def loadp():
    try:
        with open("playlists.json","r") as file:
            h = file.read()
        return json.loads(h)
    except:
        with open("playlists.json","w") as file:
            file.write(json.dumps([]))
        return []

def getPlaylistProcess():
    return {x["process"]:x for x in myplaylists}



def interface():
    global mustExit, myplaylists
    print(INSTRUCTION)
    while True:
        cmd = input("Menu > ")
        if cmd == "np":
            name = input("Name of playlist > ")
            process = input("Process name > ")
            print("New playlist. S(s) to stop add music")
            npl = []
            while True:
                o = input("New music to add > ")
                if o == 's':
                    break
                npl.append(o)
            myplaylists.append(Playlist(name,process,npl).get_json())
            savep()
        if cmd == "sp":
            print(myplaylists)
        if cmd == 'stop':
            mustExit = True
            break
        if cmd == "debug":
            p.sendToMpv('''{ "command": ["stop"] }''')
        if cmd == "p":
            p.pause()
        if cmd == 's':
            p.skip()
        if cmd == "process":
           print( [x for x in detector.get_process()].join(" | "))
        print(p.queue)

myplaylists = loadp()
mustExit = False
playing = None
t = threading.Thread(target=interface)
t.setDaemon(True)
t.start()

p = api.Player()
p.playQueue()
while True:
    if mustExit:
        p.stopPlayQueue()
        break
    procs = detector.get_process()
    props = getPlaylistProcess() 
    for pr in props:
        if pr in procs:
            if not playing == props[pr]:
                playing = props[pr]
                p.addToQueue(getRealPlaylist(playing))
                #p.playQueue()
    time.sleep(0.5)

sys.exit()