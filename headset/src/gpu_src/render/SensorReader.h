#include <irrlicht.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>

#ifndef SENSORREADER_H
#define SENSORREADER_H

class SensorReader {
private:
    float pitch;
    float yaw;
    float roll;
    float pos_x;
    float pos_y;
    key_t shm_key;
    key_t sem_key;
    int shmid;
    int semid;
    char *data;
    struct sembuf sb;

public:
    SensorReader();
    void poll();
    irr::core::vector3df getLocation();
    irr::core::vector3df getOrientation();
};

#endif
