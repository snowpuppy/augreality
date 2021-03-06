#!/usr/bin/python

SELECTMODE = 1
ADDHEADSETS = 2
SELECTSIM = 3
STARTSIMHOST = 4
RUNSIMHOST = 5
WAITACCEPT = 6
WAITRECEIVE = 7
WAITSTART = 8
RUNSIMJOIN = 9

WRAPLENGTH = 480

from Tkinter import *
from textInfo import *
from guiNetInterface import *
import subprocess
import os

class AugRealObj:
  def __init__(self, root):
    self.root = root
    #self.root.minsize(800,600)
    self.root.bind("<KP_Enter>",self.handleEnter)
    self.root.bind("<BackSpace>",self.handleBackspace)
    self.root.bind("<KP_Subtract>",self.handleBackspace)
    self.root.bind("<KP_1>",self.handle1)
    self.root.bind("<KP_2>",self.handle2)
    self.root.bind("<KP_3>",self.handle3)
    self.root.bind("<KP_4>",self.handle4)
    self.root.bind("<KP_5>",self.handle5)
    self.root.bind("<KP_6>",self.handle6)
    self.root.bind("<KP_7>",self.handle7)
    self.root.bind("<KP_8>",self.handle8)
    self.root.bind("<KP_9>",self.handle9)
    self.root.bind("<KP_0>",self.handle0)

    # set refresh event handler
    self.root.after(1000, self.refreshList)

    # Create All objects for each state.
    # New set of states for each menu.
    #
    # SELECTMODE
    # ADDHEADSETS
    # SELECTSIM
    # STARTSIMHOST
    # RUNSIMHOST
    # WAITACCEPT
    # WAITRECEIVE
    # WAITSTART
    # RUNSIMJOIN

    # SELECTMODE
    self.WifiText = Label(root, wraplength=WRAPLENGTH, text="WiFi: -----")
    self.BattText = Label(root, wraplength=WRAPLENGTH, text="Battery: -----")
    self.GPSText = Label(root, wraplength=WRAPLENGTH, text="GPS: Down") 
    self.SelectModeText = Label(root, wraplength=WRAPLENGTH,text=SelectModeStr)
    # ADDHEADSETS
    self.HeadsetInfoStr = HeadsetInfoStr
    self.HeadsetInfo = Label(root, wraplength=WRAPLENGTH, text=self.HeadsetInfoStr)
    self.HeadsetStrList = []
    # Initialize Headset status.
    for i in range(10):
      self.HeadsetStrList.append(HeadsetStr % (i, -1, "Disconnected"))
    self.Headsets = Label(root,bg="white", wraplength=WRAPLENGTH,justify=LEFT,text=" ".join(self.HeadsetStrList))
    # Create Forwards and backwards info labels.
    self.NextText = Label(root, wraplength=WRAPLENGTH,text=NextStr)
    self.GoBackText = Label(root, wraplength=WRAPLENGTH,text=GoBackStr)
    # create a list of accepted headsets.
    self.acceptList = []
    # SELECTSIM
    self.SimulationInfoStr = SimulationInfoStr
    self.SimulationInfo = Label(root, wraplength=WRAPLENGTH,text=self.SimulationInfoStr)
    # Dynamically update the list of available simulations
    self.selectedSim = 0
    self.SimList = [""]
    self.SimDispList = ["",""]
    self.Simulations = Label(root,bg="white", wraplength=WRAPLENGTH,justify=LEFT, text=" ".join(self.SimDispList))
    self.UpdateSimList()
    # STARTSIMHOST
    self.StartSimText = Label(root,wraplength=WRAPLENGTH,text=StartSimStr)
    # RUNSIMHOST
    self.RunInfoStr = RunInfoStr
    self.RunInfo = Label(root, wraplength=WRAPLENGTH,text=self.RunInfoStr)
    # WAITACCEPT
    self.MyIdText = Label(root, wraplength=WRAPLENGTH,text=MyIdStr)
    self.WaitAcceptText = Label(root, wraplength=WRAPLENGTH,text=WaitAcceptStr)
    self.GoMainText =  Label(root, wraplength=WRAPLENGTH,text=GoMainStr)
    # WAITRECEIVE
    self.WaitReceiveText = Label(root, wraplength=WRAPLENGTH,text=WaitReceiveStr)
    # WAITSTART
    self.WaitStartText =  Label(root, wraplength=WRAPLENGTH,text=WaitStartStr)
    # RUNSIMJOIN

    #self.HeadSimList =
    # Initialize state.
    self.state = SELECTMODE
    self.setupSelectMode()
    resetToInit()

  # Allow the user to select
  # Their own headset.
  def setupSelectMode(self):
    self.WifiText.grid()
    self.BattText.grid()
    self.GPSText.grid()
    self.SelectModeText.grid()
  def teardownSelectMode(self):
    self.WifiText.grid_forget()
    self.BattText.grid_forget()
    self.GPSText.grid_forget()
    self.SelectModeText.grid_forget()
  # Allow the user to select
  # a simulation to run.
  def setupAddHead(self):
    self.HeadsetInfo.grid()
    self.Headsets.grid()
    self.NextText.grid()
    self.GoBackText.grid()
  def teardownAddHead(self):
    self.HeadsetInfo.grid_forget()
    self.Headsets.grid_forget()
    self.NextText.grid_forget()
    self.GoBackText.grid_forget()
  # Select Simulation
  def setupSelectSim(self):
    self.UpdateSimList()
    self.SimulationInfo.grid()
    self.Simulations.grid()
    self.NextText.grid()
    self.GoBackText.grid()
  def teardownSelectSim(self):
    self.SimulationInfo.grid_forget()
    self.Simulations.grid_forget()
    self.Headsets.grid_forget()
    self.NextText.grid_forget()
    self.GoBackText.grid_forget()
  # Notify User of sending start condition
  def setupStartSimHost(self):
    self.StartSimText.grid()
    self.NextText.grid()
    self.GoBackText.grid()
  def teardownStartSimHost(self):
    self.StartSimText.grid_forget()
    self.NextText.grid_forget()
    self.GoBackText.grid_forget()
  # Run a simulation.
  def setupRunSimHost(self):
    self.RunInfo.grid()
  def teardownRunSimHost(self):
    self.RunInfo.grid_forget()
  # Wait for headset to be accepted (poll)
  def setupWaitAccept(self):
    self.WaitAcceptText.grid()
    self.GoMainText.grid()
  def teardownWaitAccept(self):
    self.WaitAcceptText.grid_forget()
    self.GoMainText.grid_forget()
  # Wait for files to be received (poll)
  def setupWaitReceive(self):
    self.WaitReceiveText.grid()
    self.GoMainText.grid()
  def teardownWaitReceive(self):
    self.WaitReceiveText.grid_forget()
    self.GoMainText.grid_forget()
  # Wait for start condition (poll)
  def setupWaitStart(self):
    self.WaitStartText.grid()
    self.GoMainText.grid()
  def teardownWaitStart(self):
    self.WaitStartText.grid_forget()
    self.GoMainText.grid_forget()
  # Run a join simulation
  def setupRunSimJoin(self):
    self.RunInfo.grid()
  def teardownRunSimJoin(self):
    self.RunInfo.grid_forget()
    # SELECTMODE
    # ADDHEADSETS
    # SELECTSIM
    # STARTSIMHOST
    # RUNSIMHOST
    # WAITACCEPT
    # WAITRECEIVE
    # WAITSTART
    # RUNSIMJOIN
  def handleEnter(self,event):
    print "Pressed Enter."
    if (self.state == SELECTMODE):
      # Enter does nothing in this state.
      # 0 or 1 should be selected instead.
      #self.state = ADDHEADSETS
      #self.teardownSelectMode()
      #self.setupAddHead()
      pass
    elif (self.state == ADDHEADSETS):
      self.state = SELECTSIM
      self.teardownAddHead()
      self.setupSelectSim()
    elif (self.state == SELECTSIM):
      self.state = STARTSIMHOST
      self.teardownSelectSim()
      self.setupStartSimHost()
    elif (self.state == STARTSIMHOST):
      self.state = RUNSIMHOST
      self.teardownStartSimHost()
      self.setupRunSimHost()
      sendStart()
      subprocess.call(["augreality/headset/src/simulation_src/" + self.SimList[self.selectedSim]])
      self.root.focus_force()
      # transition back to select mode.
      self.teardownRunSimHost()
      self.setupSelectMode()
      self.state = SELECTMODE
      # synchronize networking code to INIT state
      resetToInit()
    elif (self.state == RUNSIMHOST):
      pass
    elif (self.state == WAITACCEPT):
      self.state = WAITRECEIVE
      self.teardownWaitAccept()
      self.setupWaitReceive()
    elif (self.state == WAITRECEIVE):
      self.state = WAITSTART
      self.teardownWaitReceive()
      self.setupWaitStart()
    elif (self.state == WAITSTART):
      self.state = RUNSIMJOIN
      self.teardownWaitStart()
      self.setupRunSimJoin()
      # If all else fails, run the client
      # application manually!
      resetGPSOrigin()
			# TODO: needs to be changed to reflect the file that it needs to open.
      subprocess.call(["augreality/headset/src/simulation_src/" + self.SimList[self.selectedSim]])
    elif (self.state == RUNSIMJOIN):
      pass
  def handleBackspace(self,event):
    if (self.state == SELECTMODE):
      pass
    elif (self.state == ADDHEADSETS):
      self.teardownAddHead()
      self.setupSelectMode()
      self.state = SELECTMODE
      # synchronize networking code to INIT state
      resetToInit()
    elif (self.state == SELECTSIM):
      self.teardownSelectSim()
      self.setupAddHead()
      self.state = ADDHEADSETS
    elif (self.state == STARTSIMHOST):
      self.teardownStartSimHost()
      self.setupSelectSim()
      self.state = SELECTSIM
    elif (self.state == RUNSIMHOST):
      self.teardownRunSimHost()
      self.setupStartSimHost()
      self.state = STARTSIMHOST
    elif (self.state == WAITACCEPT):
      self.teardownWaitAccept()
      self.setupSelectMode()
      self.state = SELECTMODE
      resetToInit()
    elif (self.state == WAITRECEIVE):
      self.teardownWaitReceive()
      self.setupSelectMode()
      self.state = SELECTMODE
      resetToInit()
    elif (self.state == WAITSTART):
      self.teardownWaitStart()
      self.setupSelectMode()
      self.state = SELECTMODE
      resetToInit()
    elif (self.state == RUNSIMJOIN):
      self.teardownRunSimJoin()
      self.setupSelectMode()
      self.state = SELECTMODE
      resetToInit()
  def handle1(self,event):
    self.handleNumEvent(1)
  def handle2(self,event):
    self.handleNumEvent(2)
  def handle3(self,event):
    self.handleNumEvent(3)
  def handle4(self,event):
    self.handleNumEvent(4)
  def handle5(self,event):
    self.handleNumEvent(5)
  def handle6(self,event):
    self.handleNumEvent(6)
  def handle7(self,event):
    self.handleNumEvent(7)
  def handle8(self,event):
    self.handleNumEvent(8)
  def handle9(self,event):
    self.handleNumEvent(9)
  def handle0(self,event):
    self.handleNumEvent(0)
  def handleNumEvent(self,num):
    print "Key %d was pressed." % (num, )
    if (self.state == SELECTMODE):
      if (num == 1):
        self.state = ADDHEADSETS
        self.teardownSelectMode() 
        self.setupAddHead()
        # Set as host
        setHostHeadset(1)
        resetGPSOrigin()
      elif (num == 0):
        self.state = WAITACCEPT
        self.teardownSelectMode()
        self.setupWaitAccept()
        # Set as joining
        setHostHeadset(0)
        resetGPSOrigin()
        # Set up automatic transitioning
        self.root.after(1000, self.WaitAccept)
      elif (num == 9):
        exit()
    elif (self.state == ADDHEADSETS):
      myList = getBroadCastIDs()
      # Go through all broadcasting ids.
      for i in range(len(myList)):
        # If the user selected one.
        if (num == i):
          # Check if it was already accepted.
          if (myList[i] in self.acceptList):
            # If so, Drop it.
            sendDrop(myList[i])
            self.acceptList.remove(myList[i])
          else:
            # If not, attempt to accept it.
            ret = sendAccept(myList[i])
            if (ret > 0):
              # Add it to the list.
              self.acceptList.append(myList[i])
    elif (self.state == SELECTSIM):
      # Dynamically select simulation based on the users choice.
      if (num < len(self.SimList)):
        self.selectedSim = num
      print "selectedSim:",self.selectedSim
      if (len(self.SimList) > 0):
        for i in self.acceptList:
          sendFile("augreality/headset/src/simulation_src/simulations/" + self.SimList[self.selectedSim][:-3] + ".tar",i)
    elif (self.state == RUNSIMHOST):
      pass

  def refreshList(self):
    myList = getBroadCastIDs()
    # update each headset's status.
    for i in range(len(myList)):
      if (myList[i] in self.acceptList):
        status = "Accepted"
      else:
        status = "Available"
      self.HeadsetStrList[i] = HeadsetStr % (i, myList[i], status)

    # handle wireless ang gps.
    connected = "Disconnected"
    if (getWiFiStatus() == 1):
      connected = "Connected"
    numSat = getUserPosition()[0]
    GPSStr = "unlocked"
    if (numSat >= 1):
      GPSStr = "locked"
    battery = getBatteryStatus()
    batteryStr = "+"*battery + (10-battery)*"-"

    # Update status of headsets connected.
    # As well as wireless connection, battery, and GPS
    self.Headsets.configure(text=" ".join(self.HeadsetStrList))
    self.WifiText.configure(text="WiFi: %s" % (connected))
    self.BattText.configure(text="Battery: %s" % (batteryStr))
    self.GPSText.configure(text="GPS: %s" % (GPSStr))
    self.root.after(1000, self.refreshList)
  
  def WaitAccept(self):
    accepted = getAccept()
    if (self.state == WAITACCEPT and accepted == 1):
      self.state = WAITRECEIVE
      self.teardownWaitAccept()
      self.setupWaitReceive()
      self.root.after(1000, self.WaitReceive)
    if (self.state == WAITACCEPT):
      self.root.after(1000, self.WaitAccept)
  def WaitReceive(self):
    receivedFile = getReceivedFile()
    if (self.state == WAITRECEIVE and len(receivedFile) > 0):
      self.state = WAITSTART
      self.teardownWaitReceive()
      self.setupWaitStart()
      self.root.after(1000, self.WaitStart())
    if (self.state == WAITRECEIVE):
      self.root.after(1000, self.WaitReceive)
  def WaitStart(self):
    start = getStart()
    if (self.state == WAITSTART and start == 1):
      self.state = RUNSIMJOIN
      self.teardownWaitStart()
      self.setupRunSimJoin()
      # TODO: Launch client simulation as a separate process.
      subprocess.call(["/home/cornell/augreality/headset/src/simulation_src/clientSim.py"])
      # synchronize networking code to INIT state
      self.root.focus_force()
      resetToInit()
      # Pass in the name of the simulation run.
    if (self.state == WAITSTART):
      self.root.after(1000, self.WaitStart)
  def UpdateSimList(self):
    files = os.listdir("augreality/headset/src/simulation_src/")
    # Collect all available simulations from the directory indicated by "Sim" in the name of
    # the file and by ".py" in the name to indicate that it is a python file.
    self.SimList = [x for x in files if x.find("Sim") != -1 and x.find(".py") != -1 and x != "clientSim.py"]
    # format the simulation entries for presentation to the user.
    self.SimDispList = [str(x) + " " + self.SimList[x] + "\n" for x in range(len(self.SimList)) if x < 10]
    # Update text in the label.
    self.Simulations.configure(text=" ".join(self.SimDispList))
    # reset default simulation selection to zero
    self.selectedSim = 0

root = Tk()

root.title("Augmented Reality Simulator - User Interface.")
augReal = AugRealObj(root)

# force the focus on me!
root.focus_force()
root.mainloop()
