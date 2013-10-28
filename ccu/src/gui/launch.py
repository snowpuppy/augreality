#!/usr/local/bin/python

SPLASHSCRE = 1
SELECTHEAD = 2
SELECTSIMU = 3
RUNSIMULAT = 4

from Tkinter import *

class AugRealObj:
  def __init__(self, root):
    self.root = root
    self.root.minsize(800,600)
    self.root.bind("<Return>",self.handleEnter)
    self.root.bind("<BackSpace>",self.handleBackspace)
    self.root.bind("1",self.handle1)
    self.root.bind("2",self.handle2)
    self.root.bind("3",self.handle3)
    self.root.bind("4",self.handle4)
    self.root.bind("5",self.handle5)
    self.root.bind("6",self.handle6)
    self.root.bind("7",self.handle7)
    self.root.bind("8",self.handle8)
    self.root.bind("9",self.handle9)
    self.root.bind("0",self.handle0)

    self.Label1 = Label(root,text="Welcome to Augmented Reality.\n\n\nPlease Press Enter.")

    self.Label1.grid(row=5,column=4)
    self.state = SPLASHSCRE

  # Allow the user to select
  # Their own headset.
  def setupSplashScre(self):
    pass
  def teardownSplashScre(self):
    pass
  # Allow the user to select
  # a simulation to run.
  def setupSelectHead(self):
      self.Label1.grid_forget()
  def teardownSelectHead(self):
    pass
  # Select Simulation
  def setupSelectSimu(self):
    pass
  def teardownSelectSimu(self):
    pass
  # Run a simulation.
  def setupRunSimu(self):
    pass
  def teardownRunSimulat(self):
    pass
  def handleEnter(self,event):
    if (self.state == SPLASHSCRE):
      self.state = SELECTHEAD
      self.teardownSplashScre()
      self.setupSelectHead()
    elif (self.state == SELECTHEAD):
      self.state = SELECTSIMU
      self.teardownSelectHead()
      self.setupSelectSimu()
    elif (self.state == SELECTSIMU):
      self.state = RUNSIMULAT
      self.teardownSelectSimu()
      self.setupRunSimu()
    elif (self.state == RUNSIMULAT):
      pass
  def handleBackspace(self,event):
    if (self.state == SPLASHSCRE):
      pass
    elif (self.state == SELECTHEAD):
      self.state = SPLASHSCRE
      self.Label1.grid()
    elif (self.state == SELECTSIMU):
      self.state = SELECTHEAD
    elif (self.state == RUNSIMULAT):
      self.state = SELECTSIMU
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
    if (self.state == SPLASHSCRE):
      pass
    elif (self.state == SELECTHEAD):
      pass
    elif (self.state == SELECTSIMU):
      pass
    elif (self.state == RUNSIMULAT):
      pass

root = Tk()

root.title("Augmented Reality Simulator - Central Control Unit.")
augReal = AugRealObj(root)

root.mainloop()
