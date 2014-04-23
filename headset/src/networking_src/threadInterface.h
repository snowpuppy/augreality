#ifndef THREADINTERFACE_H
#define THREADINTERFACE_H

// Api requests made from gui
// and from the simulation to
// get updated information
// about the headsets.
enum apiRequest
{
	// Sensor requests
	GETUSERPOS = 1,
	GETWIFISTATUS,
	GETBATTERYSTATUS,
	// Network requests
	GETBROADCASTIDS,
	GETNUMBROADCAST,
	GETBROADCASTLOC, // deprecated (not used)
	GETPOSITION,
	GETNUMALIVE,
	GETALIVE,
	SENDUPDATEOBJS,
	SENDFILE,
	SENDEND,
	SENDSTART,
	SENDACCEPT,
	RESETGPSORIGIN,
  SETGPSORIGIN,
  GETGPSORIGIN,
  GETACCEPTEDIDS,
  GETRECEIVEDFILE,
  GETEND,
  GETSTART,
  GETDROP,
  SENDDROP,
  GETACCEPT
};

// Function: initServer()
// Purpose: starts the server that will
// communicate with the gui and simulation.
int initServer(void);
// Function: stopServer()
// Purpose: stops the server that will
// communicate with the gui and simulation.
void stopServer(void);

#endif
