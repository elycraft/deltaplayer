import requests
import os
import tempfile
import sys
from zipfile import ZipFile
import shutil
from subprocess import Popen, PIPE

def launch():
    CREATE_NEW_PROCESS_GROUP = 0x00000200
    DETACHED_PROCESS = 0x00000008

    p = Popen(["deltaplayer.exe"], stdin=PIPE, stdout=PIPE, stderr=PIPE,
            creationflags=DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP)

response = requests.get("https://api.github.com/repos/elycraft/musicplayer/releases/latest")
re = response.json()
tag = re["tag_name"]
for i in re["assets"]:
    if i["name"].startswith("deltaplayer"):
        dl = i["browser_download_url"]
try:
    with open("ver.txt",'r') as file:
        ver = file.read()
except:
    ver = "???"

if ver == tag:
    print("launch")
    launch()
else:
    print("Updating : ")
    temp = tempfile.gettempdir()
    
    if os.path.exists(os.path.join(temp,"deltatemp")):
        shutil.rmtree(os.path.join(temp,"deltatemp"))
    
    os.makedirs(os.path.join(temp,"deltatemp"))
    print(os.path.join(temp,"deltatemp"))
    file_name = "deltaplayer_update.zip"
    with open(os.path.join(temp,"deltatemp",file_name), "wb") as f:
        print(f"Downloading {file_name} for update {tag}")
        response = requests.get(dl, stream=True)
        total_length = response.headers.get('content-length')

        if total_length is None: # no content length header
            f.write(response.content)
        else:
            dl = 0
            total_length = int(total_length)
            for data in response.iter_content(chunk_size=4096):
                dl += len(data)
                f.write(data)
                done = int(50 * dl / total_length)
                sys.stdout.write("\r[%s%s]" % ('=' * done, ' ' * (50-done)) )    
                sys.stdout.flush()
    
    print("Download complete : unzipping...")

    with ZipFile(os.path.join(temp,"deltatemp",file_name), 'r') as zObject:
  
        zObject.extractall(
            os.path.join(temp,"deltatemp"))
        zObject.close()

    os.remove(os.path.join(temp,"deltatemp",file_name))
    

    if getattr(sys, 'frozen', False):
        # If the application is run as a bundle, the PyInstaller bootloader
        # extends the sys module by a flag frozen=True and sets the app 
        # path into variable _MEIPASS'.
        application_path = os.getcwd()
    else:
        application_path = os.path.dirname(os.path.abspath(__file__))
    print(f"Unziping done. Replacing files in {application_path}...")

    for filename in os.listdir(application_path):
        if filename ==  os.path.basename(__file__): continue
        file_path = os.path.join(application_path, filename)
        try:
            if os.path.isfile(file_path) or os.path.islink(file_path):
                os.unlink(file_path)
            elif os.path.isdir(file_path):
                shutil.rmtree(file_path)
        except Exception as e:
            print('Failed to delete %s. Reason: %s' % (file_path, e))

    shutil.copytree(os.path.join(temp,"deltatemp"),application_path,dirs_exist_ok=True)

    with open("ver.txt",'w') as file:
        file.write(tag)
    
    launch()

sys.exit()
