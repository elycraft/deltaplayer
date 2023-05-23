#import pyautogui
#
#for x in pyautogui.getAllTitles():  
#    print(x)

import psutil
# Iterate over all running process
def get_process():
    a = [] 
    for proc in psutil.process_iter():
        try:
            # Get process name & pid from process object.
            processName = proc.name().lower()
            if not processName in a:
                a.append(processName)          
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            pass
    return a
if __name__ == "__main__":  
    print(get_process(),len(get_process()))