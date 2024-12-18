import csv
import datetime as dt
import sys
import threading
import time
import socket

import serial

useUdp = True

# nRFComPort = 'COM4'
# megaComPort = 'COM14'
UDP_IP = "192.168.75.75" # Replace with local IP of choice
UDP_LX_PORT = 2810     # lux metre port
UDP_NRF_PORT = 17583      # nRF message port
nRFComPort = '/dev/ttyUSB0'
megaComPort = '/dev/ttyUSB1'

if useUdp:
    nrfsock = socket.socket(socket.AF_INET, # Internet
                        socket.SOCK_DGRAM) # UDP
    nrfsock.bind((UDP_IP, UDP_NRF_PORT))
else:
    ser = serial.Serial(nRFComPort, 1000000)    # nRF receiver
    ser.flushInput()

printLightCellCap = True
printNrf = True

csvDelimiter = ','
info = ''
measurement = "2023-05-11_L1_PidSleep_Dye_vs_Powerfilm"
# measurement = "2023-05-12_L3_Mnist_Dye_vs_Powerfilm"
# measurement = "2023-05-15_L3_Mnist_Dye_vs_Powerfilm_300lx"
# measurement = "_DEV"

description = "Add description here"
# description = "3 channel package relay with new device layout:\n\
#         Layer 1 - Relay     D0,  D1,  D2\n\
#         Layer 2 - MNIST     D6,  D7,  D8\n\
#         Layer 3 - Source    D12, D13, D14\n\
#         Communication is D12 to D6 to D0 to Fog etc\n\
#         3000lx after run at 500lx"

# exittime = 3600; # seconds for quick testing
exittime = 6*3600 # exit time in seconds
# exittime = 12*3600 # exit time in seconds
# exittime = 3600 # exit time in seconds

# logfile = open(".csv","a",newline='')
logfile = open(measurement + ".csv","w",newline='')
writer = csv.writer(logfile, delimiter=csvDelimiter)
starttime = time.time()

descriptionfile = open(measurement + ".txt", "w", newline='')
descriptionfile.write(description)
descriptionfile.close()

class ProcessLuxSerial(threading.Thread):
    def __init__(self, name):
        threading.Thread.__init__(self)
        self.name = name

    def run(self):
        global info
        global stimeout
        global csvDelimiter
        global starttime
        tickInterval = 0.1
        tick1 = 0.0
        tick2 = 0.0

        logfile = open(measurement + "_LightCellCap.csv","w",newline='')
        writer = csv.writer(logfile, delimiter=csvDelimiter, escapechar='\\')

        if useUdp:
            lxsock = socket.socket(socket.AF_INET, # Internet
                                socket.SOCK_DGRAM) # UDP
            lxsock.bind((UDP_IP, UDP_LX_PORT))
        else:
            arduino = serial.Serial(megaComPort, 115200)
            arduino.flush()
        # print(arduino.name)

        # global flag
        # global val     #made global here
        while True:
            if useUdp:
                lxdata, addr = lxsock.recvfrom(1024) # buffer size is 1024 bytes
                info = lxdata.decode('utf-8')
                numlines = 1 # info.count('\n')
                info = info.replace('\r\n','')
                # print("received message: %s" % lxdata)
            else:
                try:
                    line = arduino.readline()
                    info = line.decode('utf-8')
                    numlines = info.count('\n')
                    info = info.replace('\r\n','')
                    stimeout = False
                except:
                    print("Socket timeout")
                    stimeout = True
                # 60.0 - ((time.time() - starttime) % 60.0)

            # without ticks, just print what is coming
            if (numlines == 1):
                writer.writerow([time.time() - starttime,info])
                logfile.flush()
            if (printLightCellCap == True):
                print("{:<75} {:<5} {:<10}".format("", "Info:", info))

            # exit script condition
            runtime = time.time() - starttime
            if exittime > 0 and runtime > exittime:
                print("Exit lux thread after {}s runtime", runtime)
                sys.exit()


readLuxThread = ProcessLuxSerial("ReadLuxThread")
readLuxThread.start()


while True:
    if useUdp:
        nrfdata, addr = nrfsock.recvfrom(1024) # buffer size is 1024 bytes
        val = nrfdata.decode('utf-8').replace('\r\n','')
        # print("received message: %s" % nrfdata)
    else:
        ser_bytes = ser.readline()
        val = ser_bytes.decode('utf-8').replace('\r\n','')

    runtime = time.time() - starttime
    if exittime > 0 and runtime > exittime:
        print("Exit main script after {}s runtime", runtime)
        sys.exit()
    if (printNrf == True):
        print("{:<10.3f} {:<3} {:<10}".format(time.time() - starttime, ": ", val, ' : ', info))

        # print(time.time() - starttime, ": ", val, ' : ', info)
    #print(dt.datetime.now(), ": ", val)
    writer.writerow([time.time() - starttime,val])
    logfile.flush()
