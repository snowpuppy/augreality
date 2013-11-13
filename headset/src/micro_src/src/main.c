/* Includes */

#include "main.h"
#include "imu.h"
#include "printf.h"
#include "usb.h"
#include "core_cmInstr.h"


// Spi shared data.
// Data ready indicator.
// Buffer, length of data,
// and synchronization primitive.
/*
static int spiDataReady = 0;
static char spiBuf[256] = {0};
static char spiNumBytesRdy = 0;
static char spiMutex = 0;
*/

/**
 * USB initialization
 */
static void usbInit(void) {
    VCP_Init();
    VCP_SetUSBTxBlocking(0);
}

/**
 * System initialization
 */
static void init(void) {
    // Tick every 1ms
    SysTick_Config(168000);
    // Set up status LED
    gpioInit();
    // Set up the peripherals
    i2cInit();
    serialInit();
    usbInit();
    spiInit();
    // Enable interrupts for all peripherals
    __enable_fault_irq();
    __enable_irq();
}

// Function: lock()
// This function creates a locking mechanism
// that can be used to access sensative data
// through the use of a mutex.
/*
static void lock(char *mutex)
{
    char ret = 0;
    // Attempt to claim the mutex.
    do
    {
        // Keep trying while the mutex is
        // being held.
        do
        {
            // the mutex value is 1 if in use.
            // the mutex value is 0 if available.
            ret = __ldrex(mutex);
        } while (ret == 1);
        // strex returns 0 on success.
        // strex returns 1 on failure.
        ret = __strex(1, mutex);
    } while (ret == 1);
}
*/

// Function unlock()
// This function allows one to unlock
// a mutex. Note that this could be done by
// anybody... (but they shouldn't)
static void unlock(char *mutex)
{
    *mutex = 0;
}

/* Function: getBytes()
 *
 * Reads specified number of bytes into given buffer.
 *
 */
static void getBytes(char *data, int numBytes)
{
    int i = 0;
    if (numBytes > 0)
    {
        for (i = 0; i < numBytes; i++)
        {
            data[i] = fgetc(xbee);
        }
    }
}

/**
 * XBee test function, echoes characters with transformation (ASCII value + 1)
 */
static void testXBee(void) {
    char buf[256];
    unsigned char numBytes = 0;
    int i = 0;
    getBytes(buf,3);
    if (buf[0] == 'P' && buf[1] == 'A' && buf[2] == 'C')
    {
        //fputc('a', xbee);
        /*
        getBytes(buf,1);
        lock(spiMutex);
        spiDataReady = 1;
        unlock(spiMutex);
        */
        //while (spiDataReady == 1);
        getBytes(buf,1);
        getBytes(buf,1);
        spiWriteByte(buf[0]);
        numBytes = (unsigned char)buf[0];

        // get remaining bytes from wireless.
        getBytes(buf,(int)numBytes);
        // write all bytes to spi!
        for (i = 0; i < numBytes; i++)
        {
            spiWriteByte(buf[i]);
            //fputc(buf[i], xbee);
        }
        spiWriteByte(0);
        // Toggle LED for status, output character + 1
        ledToggle();
        fputc('b', xbee);
        //fputc(numBytes, xbee);
    }
}

/**
 * SPI test function, sends "Hello World" in a loop to the Raspberry PI on request
 */
/*
static void testSPI(void) {
    returnIndex = 1;
    // Write 1st byte
    spiWriteByte(returnString[0]);
    spiInit();
    // Wait FOREVER
    while (1) __WFI();
}*/

void spiReceivedByte(uint8_t data) {
    //fputc('a', xbee);
    /*
    char bytesAvailable = 0;
    char numBytesRdy = 0;
    int i = 0;

    // check to see if bytes are available to read.
    lock(spiMutex);
    bytesAvailable = spiDataReady;
    numBytesRdy = spiNumBytesRdy;
    unlock(spiMutex);
    spiWriteByte(numBytesRdy);
    if (numBytesRdy > 0)
    {
        lock(spiMutex);
        for (i = 0; i < numBytesRdy; i++)
        {
            spiWriteByte(i);
        }
        unlock(spiMutex);
    }
    */
}

int main(void) {
    uint8_t data[64];
    // Sys init
    init();
    ledOff();
    // write first byte of spi as zero.
    spiWriteByte(0);
    // initialize spi.
    //spiInit();
    // main loop.
    while (1) {
        // Loop bytes back
        uint32_t count = VCP_BytesAvailable();
        testXBee();
        if (count > 0) {
            // Pull up to 64 bytes
            if (count > 64) count = 64;
            for (uint32_t i = 0; i < count; i++)
                data[i] = VCP_GetByte();
            // Send them back
            VCP_DataTx(data, count);
        }
        __WFI();
    }
    return 0;
}
