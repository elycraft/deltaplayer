import os

os.chdir("c:/Users/Elycraft/Documents/MusicPlayer")
os.system("""pyuic5  -x ./UI/GUI_BASE.ui -o ./ui_main.py""")
os.system("""pyrcc5 ./UI/files.qrc -o ./files_rc.py""")


with open('ui_main.py', 'r', encoding='utf-8') as file:
    data = file.readlines()

data[10] = "from PySide2 import QtCore, QtGui, QtWidgets\n"
  
with open('ui_main.py', 'w', encoding='utf-8') as file:
    file.writelines(data)


with open('files_rc.py', 'r', encoding='utf-8') as file:
    data = file.readlines()

data[8] = "from PySide2 import QtCore\n"
  
with open('files_rc.py', 'w', encoding='utf-8') as file:
    file.writelines(data)