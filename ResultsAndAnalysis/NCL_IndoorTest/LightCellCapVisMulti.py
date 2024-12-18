import numpy as np
import matplotlib.pyplot as plt
import time
import sys
import shutil
import os

skiprows = 1
animatePlot = False
showPlot = False
relayedData = False

# NCL measurements - Uncomment any of the measurements an run the script to reproduce the results in the output folder.
measurement = "2023-05-11_L1_PidSleep_Dye_vs_Powerfilm"
# measurement = "2023-05-12_L3_Mnist_Dye_vs_Powerfilm"
# measurement = "2023-05-15_L3_Mnist_Dye_vs_Powerfilm_300lx"

# measurement = "_DEV"

print("{:<18} {:<10}".format("Measurement: ", measurement))
outDir = "./output/"
inDir = "./input/"

for selectDeviceId in range(22):

    file1in = inDir + measurement + "_LightCellCap.csv"
    file2in = inDir + measurement + ".csv"
    shutil.copy(inDir + measurement + ".txt", outDir + measurement + "_description.txt")

    # filein = "_DEV_LightCellCap.csv"
    file1out = outDir + measurement + "_LightCellCap_raw.csv"
    file2out = outDir + measurement + "_Messages_raw.csv"
    file3out = outDir + measurement + "_RelayedMessages_raw.csv"
    metafile = outDir + measurement + "_" + str(selectDeviceId) + "_meta.txt"
    metainfo = outDir + ""

    def convertData():
        #input file1
        fin = open(file1in, "rt")
        #output file to write the result to
        fout = open(file1out, "wt")
        #for each line in the input file
        for line in fin:
            #read replace the string and write to output file
            line = line.replace('\"', '')
            line = line.replace(' lx', '')
            line = line.replace(' V', '')
            line = line.replace(', ', ',')
            fout.write(line)
        #close input and output files
        fin.close()
        fout.close()

        #input file1
        fin = open(file2in, "rt")
        #output file to write the result to
        fout2 = open(file2out, "wt")
        fout3 = open(file3out, "wt")
        #for each line in the input file
        for line in fin:
            #read replace the string and write to output file
            line = line.replace('\"', '')
            splt = line.split(',')

            if len(splt) > 2:
                # Direct messages
                if (splt[2] == '206' or splt[2] == '205' or splt[2] == '107'):
                    if (splt[2] == '206' or splt[2] == '107'): # Workaround for np.loadtxt that number of entries is the same for every row 
                        line = line.replace('\n','')
                        line = line + ',-1\n'
                    fout2.write(line)
                # Relayed messages
                if (splt[2] == '225' or splt[2] == '226'):
                    fout3.write(line)
        #close input and output files
        fin.close()
        fout.close()

    convertData()

    lightCellCapArray = np.loadtxt(file1out, delimiter=",", skiprows=skiprows)
    # print(len(lightCellCapArray))
    seconds = lightCellCapArray[:,0] - skiprows
    lux = lightCellCapArray[:,1]
    # cellVcc = lightCellCapArray[:,2]
    # capVcc = lightCellCapArray[:,3]
    processDirectMessages = True

    if (os.path.exists(file3out) and (os.stat(file3out).st_size == 0)):
        # print("No direct messages in measurement ", str(selectDeviceId))
        processDirectMessages = False

    if (os.path.exists(file2out) and (os.stat(file2out).st_size > 0)):
        # print("No direct messages in measurement ", str(selectDeviceId))
        processDirectMessages = True


    secondsp = np.empty( shape=(0, 0) )

    if (processDirectMessages):
        messageArray = np.loadtxt(file2out, delimiter=",", skiprows=skiprows)
        # deviceArray = messageArray[ messageArray[:,5] == selectDeviceId ] # Select only rows for device ID
        deviceArray = messageArray[np.logical_and(messageArray[:,5] == selectDeviceId, messageArray[:,0] > skiprows)] # Select only rows for device ID

        mnistResultTypeArray = np.empty(shape=(0, 0))
        if (len(deviceArray) > 0):
            mnistResultTypeArray = messageArray[np.logical_and(messageArray[:,5] == selectDeviceId, messageArray[:,2] == 205, messageArray[:,0] > skiprows)] # Select only rows for device ID


        # print("HERE")
        # print(len(deviceArray))
        secondsp = deviceArray[:,0]
        messageTypes = deviceArray[:,2]
        internalVcc = deviceArray[:,3]
        pidCyclesp = deviceArray[:,4]
        nump = deviceArray[:,6]

    if (len(secondsp) == 0):
        processDirectMessages = False
        print("No direct messages from device with ID ", str(selectDeviceId))

    if processDirectMessages:
        # fig, (ax1, ax2, ax4, ax5, ax6) = plt.subplots(5, 1, sharex=False)
        fig, (ax1, ax4, ax5, ax6) = plt.subplots(4, 1, sharex=False)
        fig.tight_layout()
        # fig, (ax1, ax2, ax3, ax4) = plt.subplots(4, 1, sharex=False)
        # ax1.set_title('Capacitor Voltage')
        # capacitorVoltagePlot, = ax1.plot(seconds, capVcc, animated=animatePlot, color = 'blue')

        ax1.set_title('Internal Voltage')
        internalVoltagePlot, = ax1.plot(secondsp, internalVcc, animated=animatePlot, color = 'blue')
        # ax2.set_title('Light intensity')
        # lightIntensityPlot, = ax2.plot(seconds, lux, animated=animatePlot, color = 'green')
        # ax3.set_title('Cell Voltage')
        # cellVoltagePlot, = ax3.plot(seconds, cellVcc, animated=animatePlot, color = 'black')
        ax4.set_title('Package number')
        # ax4.set_xlim(xmin=0)
        packageNumberPlot, = ax4.plot(secondsp, nump, animated=animatePlot, color = 'purple')
        ax5.set_title('Package Histogram')
        ax5.hist(secondsp, np.arange(0, max(secondsp),10), animated=animatePlot, color='violet' )
        # ax5.set_ylim(ymax=50)
        ax6.set_title('PID Adjusted Sleep Cycles')
        # pidSleepPlot, = ax6.plot(secondsp, pidCyclesp, animated=animatePlot, color = 'red')
        ax6.scatter(secondsp, pidCyclesp, animated=animatePlot, color = 'red', s=1)

        # print("{:<18} {:<10.2f} {:<12} {:<10.2f} {:<12} {:<10.2f}".format("Seconds:", seconds[-1], "Minutes:", seconds[-1]/60 , "Hours:", seconds[-1]/3600))
        metainfo += str("{:<18} {:<10.2f} {:<12} {:<10.2f} {:<12} {:<10.2f}\n".format("Seconds:", seconds[-1], "Minutes:", seconds[-1]/60 , "Hours:", seconds[-1]/3600))

        startMessageIndices = np.where(nump == 1)
        totalMessages = 0
        totalSeconds = secondsp[-1] - secondsp[0]

        for index in startMessageIndices[0]:
            # print("Index: ", index)
            # print(nump[index-1])
            totalMessages = totalMessages + nump[index-1]
            # print("Total messages: ", totalMessages)

        if skiprows > 0:
            # totalMessages = nump[-1] - nump[0]
            totalMessages = len(nump)
        else:
            totalMessages = totalMessages - nump[0]

        totalRuntime = secondsp[-1] - secondsp[0]
        # Do not need the datatype for the start message indices
        startMessageIndices = startMessageIndices[0]

        if skiprows == 0 and len(startMessageIndices) > 0:
            metainfo += str("{:<18} {:<10} {:<12} {:<10.2f}\n".format("Duty cycles: ", len(startMessageIndices), "s/DC", seconds[-1]/len(startMessageIndices)))

        numMessages = len(nump)
        psec = numMessages/totalRuntime

        metainfo += str("{:<18} {:<10.0f}\n".format("Number of Messages: ", numMessages))
        metainfo += str("{:<18} {:<10.0f}\n".format("Max message number: ", totalMessages))
        metainfo += str("{:<18} {:<10.2f}\n".format("Message %: ", numMessages/totalMessages))
        metainfo += str("{:<18} {:<10.0f}\n".format("Total runtime: ", totalRuntime))
        metainfo += str("{:<18} {:<10.4f}\n".format("Per second: ", psec))
        metainfo += str("{:<18} {:<10.1f}\n".format("Per hour: ", psec*3600))
        metainfo += str("{:<18} {:<10.2f}\n".format("Av. lx: ", np.average(lux)))
        metainfo += str("{:<18} {:<10.2f}\n".format("Min. lx: ", np.min(lux)))
        metainfo += str("{:<18} {:<10.2f}\n".format("Max. lx: ", np.max(lux)))


        # Get individual runtimes per run
        # Startmessages are preceeded by an end message, so we transform the start message array accordingly
        endMessageIndices = np.roll(startMessageIndices - 1, -1)

        # Get endseconds, startseconds and subtract
        startSeconds = np.empty(0)
        endSeconds = np.empty(0)
        runtimes = np.empty(0)
        endMessageNumbers = np.empty(0)

        for i in range(0, len(startMessageIndices)):
            startMessageindex = startMessageIndices[i]
            endMessageIndex = endMessageIndices[i]
            startSeconds = secondsp[startMessageindex]
            endSeconds = secondsp[endMessageIndex]
            runtime = endSeconds - startSeconds
            runtimes = np.append(runtimes, runtime)
            endMessageNumbers = np.append(endMessageNumbers, nump[endMessageIndex])

        if skiprows == 0:
            totalRuntime = np.sum(runtimes)
            averageRuntime = np.average(runtimes)
            activePercentage = (totalRuntime/totalSeconds)*100
            averageMessages = np.average(endMessageNumbers)

            metainfo += str("{:<18} {:<10.2f}\n".format("Av. runtime: ", averageRuntime))
            metainfo += str("{:<18} {:<10.2f}\n".format("Av. messages: ", averageMessages))
            metainfo += str("{:<18} {:<10.2f}\n".format("% active: ", activePercentage))

        minVcc = np.amin(internalVcc)
        maxVcc = np.amax(internalVcc)

        metainfo += str("{:<18} {:<10.0f}\n".format("Minimum VCC: ", minVcc))
        metainfo += str("{:<18} {:<10.0f}\n".format("Maximum VCC: ", maxVcc))
        averageSleep = np.average(pidCyclesp)
        metainfo += str("{:<18} {:<10.1f}\n".format("Av. sleep ", averageSleep))
        metainfo += str("{:<18} {:<10.0f}\n".format("Mnist pred.: ", len(mnistResultTypeArray)))
        metainfo += str("{:<18} {:<10.1f}\n".format("Mnist pred./h: ", (3600/seconds[-1])*len(mnistResultTypeArray)))
        metainfo += str("{:<18} {:<10.0f}\n".format("Device ID: ", selectDeviceId))
        print(metainfo)

        metaout = open(metafile, "wt+")
        metaout.write(metainfo)
        metaout.close()

        # print("{:<18} {:<10.2f}".format("Av. Runtime", averageRuntime))
        # activePercentage = (totalRuntime/totalSeconds)*100

        # print("{:<18} {:.2f} {}".format("Active:", averageRuntime, "%"))

        fig = plt.gcf()
        fig.set_size_inches((22, 11), forward=False)

        ax1.set_xlim(xmin=skiprows)
        # ax2.set_xlim(xmin=skiprows)
        # ax3.set_xlim(xmin=skiprows)
        ax4.set_xlim(xmin=skiprows)
        ax5.set_xlim(xmin=skiprows)
        ax6.set_xlim(xmin=skiprows)

        ax6.set_ylim(ymin=0)

        maxx = max(seconds)*1.02

        ax1.set_xlim(xmin=0, xmax=maxx)
        # ax2.set_xlim(xmin=0, xmax=maxx)
        # ax3.set_xlim(xmin=0, xmax=maxx)
        ax4.set_xlim(xmin=0, xmax=maxx)
        ax5.set_xlim(xmin=0, xmax=maxx)
        ax6.set_xlim(xmin=0, xmax=maxx)

        plt.savefig(outDir + measurement + "_" + str(selectDeviceId) + ".png")
        # plt.pause(3)

        fig.set_figwidth(23)
        fig.set_figheight(10)

        if showPlot:
            # ax1.set_ylim(ymin=0)
            # ax3.set_ylim(ymin=0)

            plt.show(block=(not animatePlot))
        else:
            if animatePlot:
                print("Please set showPlot to true to enable animation!")
            plt.close()
            # break
            # sys.exit()      # Exit if plot not shown

    if (os.path.exists(file3out) and (os.stat(file3out).st_size > 0)):
        metainfo = str('')

        relayedMessageArray = np.loadtxt(file3out, delimiter=",", skiprows=skiprows)
        # We filter by relay device ID, not source ID
        relayedDeviceArray = relayedMessageArray[np.logical_and(relayedMessageArray[:,9] == selectDeviceId, relayedMessageArray[:,0] > skiprows)] # Select only rows for device ID
        
        if (processDirectMessages):
            resyncMessageArray = messageArray[np.logical_and(messageArray[:,5] == selectDeviceId, messageArray[:,0] > skiprows)] # Select only rows for device ID

        relaySecondsp = relayedDeviceArray[:,0]
        internalVcc = relayedDeviceArray[:,3]
        pidCyclesp = relayedDeviceArray[:,4]
        sourceId = relayedDeviceArray[:,5]
        nump = relayedDeviceArray[:,6]

        relayInternalVcc = relayedDeviceArray[:,7]
        relayPidCyclesp = relayedDeviceArray[:,8]

        if (processDirectMessages):
            # Information about resync messages
            relayResyncSeconds = resyncMessageArray[:,0] 
            relayResyncPid = resyncMessageArray[:,4]

        if (len(relaySecondsp) == 0):
            print("No relayed data found for device with ID ", str(selectDeviceId))
            continue    

        totalRuntime = seconds[-1] - seconds[0]
        metainfo += str("{:<18} {:<10.0f}\n".format("Runtime (s): ", totalRuntime))
        metainfo += str("{:<18} {:<10.0f}\n".format("Relayed messages: ", len(nump)))
        if (processDirectMessages):
            metainfo += str("{:<18} {:<10.0f}\n".format("Resync messages: ", len(relayResyncSeconds)))
        if (len(nump) > 0 and processDirectMessages):
            metainfo += str("{:<18} {:<10.1f}\n".format("Resync %: ", (len(relayResyncSeconds)/len(nump))*100 ))
        
        psec = len(nump)/totalRuntime

        metainfo += str("{:<18} {:<10.4f}\n".format("Per second: ", psec))
        metainfo += str("{:<18} {:<10.1f}\n".format("Per hour: ", psec*3600))
        metainfo += str("{:<18} {:<10.1f}\n".format("Av. relay sleep: ", np.average(relayPidCyclesp) ))

        for id in np.unique(sourceId):
            idArray = sourceId[sourceId == id]
            psec = len(idArray)/totalRuntime
            
            metainfo += str("\n{} {:<13} {}\n").format("From", int(id), len(idArray))
            metainfo += str("{:<18} {:<10.4f}\n".format("Per second: ", psec))
            metainfo += str("{:<18} {:<10.1f}\n".format("Per hour: ", psec*3600))

            # print(idArray)

        # Print metainfo and append it to existing file
        print(metainfo)
        if (processDirectMessages):
            metaout = open(metafile, "at+")
        else:
            metaout = open(metafile, "wt+")
        metaout.write(metainfo)
        metaout.close()

    
        fig2, (ax1, ax2, ax3, ax4, ax5, ax6, ax7, ax8, ax9) = plt.subplots(9, 1, sharex=False)
        fig2.tight_layout()
        # fig, (ax1, ax2, ax3, ax4) = plt.subplots(4, 1, sharex=False)
        # ax1.set_title('Capacitor Voltage')
        # capacitorVoltagePlot, = ax1.plot(seconds, capVcc, animated=animatePlot, color = 'blue')

        ax1.set_title('Light intensity')
        lightIntensityPlot, = ax1.plot(seconds, lux, animated=animatePlot, color = 'green')

        ax2.set_title('Sender Voltage')
        internalVoltagePlot, = ax2.plot(relaySecondsp, internalVcc, animated=animatePlot, color = 'blue')
        ax3.set_title('Relay Voltage')
        relayInternalVoltagePlot, = ax3.plot(relaySecondsp, relayInternalVcc, animated=animatePlot, color = 'blue')

        ax4.set_title('Sender PID Adjusted Sleep Cycles')
        # pidSleepPlot, = ax4.plot(relaySecondsp, pidCyclesp, animated=animatePlot, color = 'red')
        ax4.scatter(relaySecondsp, pidCyclesp, animated=animatePlot, color = 'red')
        ax5.set_title('Relay PID Adjusted Sleep Cycles')
        # relayPidSleepPlot, = ax5.plot(relaySecondsp, relayPidCyclesp, animated=animatePlot, color = 'red')
        ax5.scatter(relaySecondsp, relayPidCyclesp, animated=animatePlot, color = 'red')

        ax6.set_title('Resync PID')
        if (processDirectMessages):
            ax6.scatter(relayResyncSeconds, relayResyncPid, color = 'red')

        # ax3.set_title('Cell Voltage')
        # cellVoltagePlot, = ax3.plot(seconds, cellVcc, animated=animatePlot, color = 'black')
        ax7.set_title('Sender Package number')
        # ax4.set_xlim(xmin=0)
        packageNumberPlot, = ax7.plot(relaySecondsp, nump, animated=animatePlot, color = 'purple')
        
        if (len(nump) > 0):
            ax8.set_title('Package Histogram')
            ax8.hist(relaySecondsp, np.arange(0, max(relaySecondsp),10), animated=animatePlot, color='violet' )

        ax9.set_title('Source Device ID')
        # relayPidSleepPlot, = ax5.plot(relaySecondsp, relayPidCyclesp, animated=animatePlot, color = 'red')
        ax9.scatter(relaySecondsp, sourceId, animated=animatePlot, color = 'red')

        fig2.set_size_inches((22, 11), forward=False)


        fig2 = plt.gcf()
        fig2.set_size_inches((22, 11), forward=False)

        # ax1.set_xlim(xmin=skiprows)
        # ax2.set_xlim(xmin=skiprows)
        # ax3.set_xlim(xmin=skiprows)
        # ax4.set_xlim(xmin=skiprows)
        # ax5.set_xlim(xmin=skiprows)
        # ax6.set_xlim(xmin=skiprows)
        # ax7.set_xlim(xmin=skiprows)

        ax4.set_ylim(ymin=0)
        ax5.set_ylim(ymin=0)

        maxx = max(seconds)*1.02

        ax1.set_xlim(xmin=skiprows, xmax=maxx)
        ax2.set_xlim(xmin=skiprows, xmax=maxx)
        ax3.set_xlim(xmin=skiprows, xmax=maxx)
        ax4.set_xlim(xmin=skiprows, xmax=maxx)
        ax5.set_xlim(xmin=skiprows, xmax=maxx)
        ax6.set_xlim(xmin=skiprows, xmax=maxx)
        ax7.set_xlim(xmin=skiprows, xmax=maxx)
        ax8.set_xlim(xmin=skiprows, xmax=maxx)
        ax9.set_xlim(xmin=skiprows, xmax=maxx)

        plt.savefig(outDir + measurement + "_Relayed_" + str(selectDeviceId) + ".png")

    # if animatePlot:
    #     fig.canvas.toolbar.pack_forget()
    # bg = fig.canvas.copy_from_bbox(fig.bbox)

    # ax1.draw_artist(capacitorVoltagePlot)
    # ax2.draw_artist(lightIntensityPlot)
    # fig.canvas.blit(fig.bbox)

    # while animatePlot:
    #     convertData()
    #     # fig.canvas.restore_region(bg)

    #     arr = np.loadtxt(file1out, delimiter=",", skiprows=skiprows)
    #     seconds = arr[:,0] - skiprows
    #     lux = arr[:,1]
    #     # capVcc = arr[:,3]
    #     # cellVcc = arr[:,2]

    #     messageArray = np.loadtxt(file2out, delimiter=",")
    #     deviceArray = messageArray[ messageArray[:,5] == selectDeviceId  ] # Select only rows for device ID
    #     secondsp = deviceArray[:,0]
    #     internalVcc = deviceArray[:,3]
    #     nump = deviceArray[:,5]

    #     # print("Seconds: ", len(seconds), ", Vcc Values: ", len(capVcc))

    #     # capacitorVoltagePlot.set_xdata(seconds)
    #     # capacitorVoltagePlot.set_ydata(capVcc)
    #     internalVoltagePlot.set_xdata(secondsp)
    #     internalVoltagePlot.set_ydata(internalVcc)
    #     lightIntensityPlot.set_xdata(seconds)
    #     lightIntensityPlot.set_ydata(lux)
    #     # cellVoltagePlot.set_xdata(seconds)
    #     # cellVoltagePlot.set_ydata(cellVcc)
    #     packageNumberPlot.set_xdata(secondsp)
    #     packageNumberPlot.set_ydata(nump)

    #     maxx = max(seconds)*1.02

    #     ax1.set_xlim(xmin=0, xmax=maxx)
    #     ax2.set_xlim(xmin=0, xmax=maxx)
    #     # ax3.set_xlim(xmin=0, xmax=maxx)
    #     ax4.set_xlim(xmin=0, xmax=maxx)
    #     ax5.set_xlim(xmin=0, xmax=maxx)

    #     ax1.plot(seconds, capVcc, color = 'blue')
    #     ax2.plot(seconds, lux, color = 'green')
    #     # ax3.plot(seconds, cellVcc, color = 'black')
    #     ax4.plot(secondsp, nump, color = 'purple')
    #     ax5.hist(secondsp, np.arange(0, max(secondsp),10), color='violet')

    #     fig.canvas.draw()
    #     fig.canvas.blit(fig.bbox)
    #     fig.canvas.flush_events()

    #     time.sleep(1)

#print(seconds)
#print(capVcc)

# plot.plot(*np.loadtxt(fileout,unpack=True,delimiter=",", skiprows=1), linewidth=2.0)
#plot.scatter(x,y, s=1)

#plot.show()