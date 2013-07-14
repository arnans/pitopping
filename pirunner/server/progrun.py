#!/usr/bin/env python
import subprocess
import sys
import socket
import time 
import tempfile
port = 6970 

host = "192.168.1.1"
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 

#s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
#s2.bind(("",6970))
f = open('output.out','w')
f.close
#f = open('output.out','a')

w = tempfile.NamedTemporaryFile()
process = subprocess.Popen(['python','-u','prog1.py'],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,bufsize=0)


while True:
    line = process.stdout.readline()
    if line != '':
	print line.rstrip()
	#sys.stdout.flush()
	#f.write(line)
	s.sendto(line.rstrip(), (host, port))	
	#f.write(out)    	
    else:
	break
s.close()
