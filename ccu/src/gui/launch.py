#!/usr/bin/python

SPLASHSCRE = 1
SELECTHEAD = 2
SELECTSIMU = 3
RUNSIMULAT = 4

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
    # SPLASHSCRE
    self.WelcomeText = Label(root, wraplength=WRAPLENGTH,text="Welcome to Augmented Reality.\n\n\nPlease Press Enter.")
    # SELECTHEAD
    self.HeadsetInfoStr = HeadsetInfoStr
    self.HeadsetInfo = Label(root, wraplength=WRAPLENGTH, text=self.HeadsetInfoStr)
    self.HeadsetStrList = []
    # Initialize Headset status.
    for i in range(10):
      self.HeadsetStrList.append(HeadsetStr % (i, "-1","-1", "Disconnected"))
    self.Headsets = Label(root,bg="white", wraplength=WRAPLENGTH,text=" ".join(self.HeadsetStrList))
    # SELECTSIMU
    self.SimulationInfoStr = SimulationInfoStr
    self.SimulationInfo = Label(root, wraplength=WRAPLENGTH,text=self.SimulationInfoStr)
    # (Hard coded for now...)
    self.SimulationList = [ "1 Pac-Man" ]
    self.SimulationList.append("2 Demo")
    self.Simulations = Label(root,bg="white", wraplength=WRAPLENGTH, text=" ".join(self.SimulationList))
    # RUNSIMULAT
    self.RunInfoStr = RunInfoStr
    self.RunInfo = Label(root, wraplength=WRAPLENGTH,text=self.RunInfoStr)
    #self.HeadSimulationList =
    # Initialize state.
    self.state = SPLASHSCRE
    self.setupSplashScre()

  # Allow the user to select
  # Their own headset.
  def setupSplashScre(self):
    self.WelcomeText.grid()
  def teardownSplashScre(self):
    self.WelcomeText.grid_forget()
  # Allow the user to select
  # a simulation to run.
  def setupSelectHead(self):
    self.HeadsetInfo.grid()
    self.Headsets.grid()
  def teardownSelectHead(self):
    self.HeadsetInfo.grid_forget()
    self.Headsets.grid_forget()
  # Select Simulation
  def setupSelectSimu(self):
    self.SimulationInfo.grid()
    self.Simulations.grid()
  def teardownSelectSimu(self):
    self.SimulationInfo.grid_forget()
    self.Simulations.grid_forget()
  # Run a simulation.
  def setupRunSimulation(self):
    self.RunInfo.grid()
  def teardownRunSimulat(self):
    self.RunInfo.grid_forget()
  def handleEnter(self,event):
    if (self.state == SPLASHSCRE):
      self.state = SELECTHEAD
      self.teardownSplashScre()
      self.setupSelectHead()
    elif (self.state == SELECTHEAD):
      self.state = SELECTSIMU
      self.teardownSelectHead()
      self.setupSelectSimu()
      sendAccept('40A66DAE\r3A200\r')
    elif (self.state == SELECTSIMU):
      self.state = RUNSIMULAT
      self.teardownSelectSimu()
      self.setupRunSimulation()
      sendStart()
    elif (self.state == RUNSIMULAT):
      pass
  def handleBackspace(self,event):
    if (self.state == SPLASHSCRE):
      pass
    elif (self.state == SELECTHEAD):
      #self.state = SPLASHSCRE
      pass
    elif (self.state == SELECTSIMU):
      self.teardownSelectSimu()
      self.setupSelectHead()
      self.state = SELECTHEAD
      sendGoBack('40A66DAE\r3A200\r')
    elif (self.state == RUNSIMULAT):
      self.teardownRunSimulat()
      self.setupSelectHead()
      self.state = SELECTHEAD
      sendGoBack('40A66DAE\r3A200\r')
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
    #print "Key %d was pressed." % (num, )
    if (self.state == SPLASHSCRE):
      pass
    elif (self.state == SELECTHEAD):
      pass
    elif (self.state == SELECTSIMU):
      # Hardcoded simulations.
      if (num == 0):
        sendFile("../sim/pacman.tar")
      if (num == 1):
        sendFile("../sim/demo.tar")
      pass
    elif (self.state == RUNSIMULAT):
      pass
  def refreshList(self):
    myList = getBroadCastIDs()
    for i in range(len(myList)):
      listItems = myList[i].split()
      self.HeadsetStrList[i] = HeadsetStr % (i, listItems[0], listItems[1], "Available")
    self.Headsets.configure(text=" ".join(self.HeadsetStrList))
    self.root.after(1000, self.refreshList)

root = Tk()

root.title("Augmented Reality Simulator - Central Control Unit.")
augReal = AugRealObj(root)

# force the focus on me!
root.focus_force()
root.mainloop()
