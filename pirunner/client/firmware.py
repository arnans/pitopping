import Tkinter
from tkFileDialog import *
import tkMessageBox
import sys
import socket
import time
import thread
class App:
    def __init__(self,master):
        self.text = ""
        self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        

        self.srecv = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.srecv.bind(("",6970))
        self.srecv.settimeout(0.1)
        
        self.lbf1 = Tkinter.LabelFrame(master,text="Firmware Source (Python file)")
        self.lbf1.pack(side="top",anchor = "nw",padx=7,pady=7)
        self.en1 = Tkinter.Entry(self.lbf1,width = 70,bd=3)
        self.en1.pack(side="top",padx=7,pady=7)
        self.button1 = Tkinter.Button(self.lbf1,text="Browse",width=15,height=2,command=self.openfile)
        self.button1.pack(side="top",anchor="ne",padx=7,pady=7)

        self.lbf2 = Tkinter.LabelFrame(master,text="IP Address",width=10)
        self.lbf2.pack(side="top",anchor="nw",padx=7,pady=7)
        self.lb = Tkinter.Label(self.lbf2,text="IP")
        self.lb.pack(side="left",padx=7,pady=7)
        self.en2 = Tkinter.Entry(self.lbf2,width = 25,bd=3)
        self.en2.pack(side = "left",padx=7,pady=7)

        self.dlbt = Tkinter.Button(master,text="Download Firmware",width=20,height=3,command=self.sendfile)
        self.dlbt.place(x=300,y=130)
        #self.dlbt.pack(side = "right",anchor="ne",padx=7,pady=7)

        self.lbf3 = Tkinter.LabelFrame(master)
        self.lbf3.pack(side="top",anchor="w")
        self.scrollbar = Tkinter.Scrollbar(self.lbf3)
        self.scrollbar.pack(side="right",fill="y")
        self.tx = Tkinter.Text(self.lbf3,width=55,yscrollcommand = self.scrollbar.set )
        self.tx.pack(side="left")
        self.scrollbar.config(command=self.tx.yview)
        
        self.btclear = Tkinter.Button(master,text="Clear",width=10,height=1,command=self.clear)
        self.btclear.pack(side="top",anchor="w",padx=3,pady=5)
        
    def sendfile(self):
        host = self.en2.get()
        port = 6969
        f = open(self.en1.get())
        text = f.read()
        self.s.sendto(text,(host,port))
        
    def openfile(self):
        initial_dir = "C:\ "
        # the filetype mask (default is all files)
        mask = \
        [("Text and Python files","*.txt *.py *.pyw"), 
        ("HTML files","*.htm"), 
        ("All files","*.*")]        
        fin = askopenfile(initialdir=initial_dir, filetypes=mask, mode='r')
        self.en1.delete(0,len(self.en1.get()))
        self.en1.insert(0,fin.name)
        #self.text = fin.read()

    def test(self):
        while(1):
            try:
                data,addr = self.srecv.recvfrom(1024)
                self.tx.insert("end",data)
                self.tx.insert("end","\n")
            except:
                pass

    def clear(self):
        self.tx.delete("0.0","end")
    '''def recieve(self):
        while(1):
            #self.srecv.bind(("",6970))
            try:
                data,addr = self.srecv.recvfrom(1024)
                self.tx.insert("end",data)
            except:
                pass
        self.srecv.close()'''

root = Tkinter.Tk()
root.title("Firmware Loader v 1.00")
root.resizable(width=False, height=False)
app=App(root)
#func=getattr(app,"test")
try:
    thread.start_new_thread(app.test,())
except Exception,e:
    print e
root.mainloop()
