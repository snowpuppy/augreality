// File: packet.h
// Contains the definition of the
// packets exchanged wirelessly.
#ifndef PACKET_H
#define PACKET_H

// broadcastPacket: This is the packet
// that the headset will send repeatedly
// on startup. The ccu will listen for these
// packets to know how many headsets are
// available.
typedef struct broadcastPacket
{
  char header[3];
  char type;
  uint16_t address;
  float latitude;
  float longitude;
  uint16_t crc;
} broadCastPacket_t;

// AcceptPacket: This is the packet
// that the ccu will send to the headset
// when the ccu is ready to accept the
// headset.

// RejectPacket: This is the packet
// that the ccu will send to the headset
// to indicate that it is no longer in
// the simulation and that it should
// exit the simulation.

// LoadPacket: This is the packet used
// to send images/obj data from the ccu to
// the headset.

// updateImagePacket: This is the packet
// sent from the ccu to the headset to
// indicate that an image instance needs to
// be updated.

// startSimulation: This is the packet
// sent from the ccu to headset to indicate
// that the simulation should start.

// endSimulation: This is the packet
// sent from the ccu to the headset to
// indicate that the simulation should
// be ended for this headset.

// heartbeatPacket: This is the packet
// that the headset will send to the ccu
// during a simulation to inform the ccu
// about it's gps location and orientation.

// confirmPacket: This is the packet
// that the headset will send to indicate
// that it received an update from the ccu.



// Shared functions.

// Function: calcCrc
// This function takes a list of characters
// which has probably been cast that way from a
// structure and performs a crc calculation.
uint16_t calcCrc(char *packet, int size)
{
  uint16_t ret = 0;
  // Make sure min size is one byte plus the 16bit crc
  if (packet != NULL && size > 1+sizeof(short))
  {
    ret = ( ((short)packet[0]) << 8) + packet[size-sizeof(short)];
    return ret;
  }
  return 0;
}

#endif
