import time
import threading
import os

os.system("make")

def peer_one():
    os.system("alacritty -e ./main 3000 3232 3333")

def peer_two():
    os.system("alacritty -e ./main 3001 3535 3737")

def peer_three():
    os.system("alacritty -e ./main 3002 3000 3001")

t1 = threading.Thread(target=peer_one) 
t2 = threading.Thread(target=peer_two) 
t3 = threading.Thread(target=peer_three)
t1.start()
time.sleep(1)
t2.start()
time.sleep(1)
t3.start()
