import socket 
import subprocess
import sys
import os
import signal
port = 6969
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
s.bind(("", port))
chk=0
while 1:
	try:
		print "read :"
        	data,addr = s.recvfrom(1024)
		print data 
		f = open('prog1.py','w')
		f.write(data)
		f.close()
		try:
			os.kill(process, 0)
		except:
			pass
		process = subprocess.Popen([sys.executable,'progrun.py','&'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		chk=1
	except KeyboardInterrupt:
		print 'exit'
		s.close()
		raise
        except Exception,e:
		print e
                pass

