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

class AugRealObj:
  def __init__(self, root):
    self.root = root
    #self.root.minsize(800,600)
    self.root.bind("<KP_Enter>",self.handleEnter)
    self.root.bind("<BackSpace>",self.handleBackspace)
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
    self.Headsets = Label(root,bg="white", wraplength=WRAPLENGTH,text=" ".join(self.HeadsetStrList))
    # Create Forwards and backwards info labels.
    self.NextText = Label(root, wraplength=WRAPLENGTH,text=NextStr)
    self.GoBackText = Label(root, wraplength=WRAPLENGTH,text=GoBackStr)
    # SELECTSIM
    self.SimulationInfoStr = SimulationInfoStr
    self.SimulationInfo = Label(root, wraplength=WRAPLENGTH,text=self.SimulationInfoStr)
    # (Hard coded for now...)
    self.SimulationList = [ "1 Pac-Man" ]
    self.SimulationList.append("2 Trees")
    self.Simulations = Label(root,bg="white", wraplength=WRAPLENGTH, text=" ".join(self.SimulationList))
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

    #self.HeadSimulationList =
    # Initialize state.
    self.state = SELECTMODE
    self.setupSelectMode()

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
      self.state = RUNSIMHOST
      self.teardownSelectSim()
      self.setupStartSimHost()
    elif (self.state == STARTSIMHOST):
      self.state = RUNSIMHOST
      self.teardownStartSimHost()
      self.setupRunSimHost()
      #sendStart()
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
    elif (self.state == RUNSIMJOIN):
      pass
  def handleBackspace(self,event):
    if (self.state == SELECTMODE):
      pass
    elif (self.state == ADDHEADSETS):
      self.teardownAddHead()
      self.setupSelectMode()
      self.state = SELECTMODE
      pass
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
    elif (self.state == WAITRECEIVE):
      self.teardownWaitReceive()
      self.setupSelectMode()
      self.state = SELECTMODE
    elif (self.state == WAITSTART):
      self.teardownWaitStart()
      self.setupSelectMode()
      self.state = SELECTMODE
    elif (self.state == RUNSIMJOIN):
      self.teardownRunSimJoin()
      self.setupSelectMode()
      self.state = SELECTMODE
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
      elif (num == 0):
        self.state = WAITACCEPT
        self.teardownSelectMode()
        self.setupWaitAccept()
      pass
    elif (self.state == ADDHEADSETS):
      pass
    elif (self.state == SELECTSIM):
      # Hardcoded simulations.
      if (num == 1):
        sendFile("../sim/pacman.tar")
      if (num == 2):
        sendFile("../sim/demo.tar")
      pass
    elif (self.state == RUNSIMHOST):
      pass

  def refreshList(self):
    myList = [] #getBroadCastIDs()
    for i in range(len(myList)):
      listItems = myList[i].split()
      self.HeadsetStrList[i] = HeadsetStr % (i, listItems[0], listItems[1], "Available")
    self.Headsets.configure(text=" ".join(self.HeadsetStrList))
    self.root.after(1000, self.refreshList)

root = Tk()

root.title("Augmented Reality Simulator - User Interface.")
augReal = AugRealObj(root)

# force the focus on me!
root.focus_force()
root.mainloop()
