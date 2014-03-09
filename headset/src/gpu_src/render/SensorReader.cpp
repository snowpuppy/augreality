#include "SensorReader.h"
#include <irrlicht.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>

#define SHM_SIZE 1024

SensorReader::SensorReader() {
    //set up shared memory
    shm_key = ftok("sensorData", 'R');
    shmid = shmget(shm_key, SHM_SIZE, O_RDONLY);
    data = (char *)shmat(shmid, (void *)0, 0);

    //set up semaphore
    sem_key = ftok("sensorDataSem", 'R');
    semid = semget(sem_key, 1, O_RDWR);
    sb.sem_num = 0;
    sb.sem_op = 0;
    sb.sem_flg = 0;
}

void SensorReader::poll() {
    //wait for semaphore to be freed
    semop(semid, &sb, 1);
    //lock semaphore
    sb.sem_op = 1;
    semop(semid, &sb, 1);
    //read data from shared memory
    sscanf(data, "%f %f %f %f %f", &pitch, &roll, &yaw, &pos_x, &pos_y);
    //unlock semaphore
    sb.sem_op = -1;
    semop(semid, &sb, 1);
}

irr::core::vector3df SensorReader::getLocation() {
    return irr::core::vector3df(pos_x, pos_y, 0);
}

irr::core::vector3df SensorReader::getOrientation() {
    return irr::core::vector3df(roll, pitch, yaw);
}
