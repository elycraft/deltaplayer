from ui_register import Ui_MainWindow
from PyQt5 import QtCore, QtGui, QtWidgets
from detector import get_process
import json

class machin():

    def __init__(self,ui) -> None:
        self.ui = ui

        self.ui.pushButton.clicked.connect(lambda : self.actualiser())
        self.ui.btn_ok.clicked.connect(lambda : self.ok())

        self.lib = self.load()

        self.actualiser()
        
    
    def actualiser(self):
        a = get_process()
        a.sort()
        self.ui.comboBox.clear()
        for i in a:
            self.ui.comboBox.addItem(i)

    def ok(self):
        name = self.ui.lineEdit.text()
        img = self.ui.lineEdit_2.text()
        proc = self.ui.comboBox.currentText() 
        self.lib.append({"name":name,"img":img,"proc":proc})
        self.save()

    def save(self):
        with open("lib.json","w") as file:
            file.write(json.dumps(self.lib))
    
    def load(self):
        try:
            with open("lib.json","r") as file:
                h = file.read()
            return json.loads(h)
        except:
            with open("lib.json","w") as file:
                file.write(json.dumps([]))
            return []

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    m = machin(ui)
    MainWindow.show()
    sys.exit(app.exec_())