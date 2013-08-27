/*
 * API.h - Provides the high-level user functionality intended for use by typical Maple
 * programmers; more advanced programmers might also want "API_ext.h"
 *
 * This file is part of the Purdue Robotics Operating System (PROS).
 *
 * PROS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PROS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PROS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef API_H_
#define API_H_

// System includes
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/types.h>

// Begin C++ extern to C
#ifdef __cplusplus
extern "C" {
#endif

// -------------------- Pin control functions --------------------

// ---- Pin control definitions ----
/**
 * There are 15 available analog I/O on the Maple accessible by the analogRead() function by
 * default.
 */
#define BOARD_NR_ADC_PINS 15
/**
 * There are 42 available I/O on the Maple that can be used for digital communication,
 * including the JTAG, expansion, and analog ports
 *
 * Not all of these pins are 5-volt tolerant.
 */
#define BOARD_NR_GPIO_PINS 42
/**
 * 3 USARTs are accessible. There are five, but UART4 and 5's pins are already used.
 */
#define BOARD_NR_USARTS 3
/**
 * The LeafLabs Maple is clocked at 72 MHz for about 72 cycles per microsecond.
 */
#define CYCLES_PER_MICROSECOND 72
/**
 * In reality, any non-zero argument to digitalWrite() is interpreted as HIGH.
 */
#define HIGH 1
/**
 * In reality, a zero argument to digitalWrite() is interpreted as LOW.
 */
#define LOW 0

// ---- Possible pin states for pinMode() ----
/**
 * Digital input, with pullup. The pullup causes the input to read as "HIGH" when unplugged,
 * but is fairly weak and can safely be driven by most sources.
 */
#define INPUT_PULLUP 0x0A
/**
 * Analog input. This is the default state for the 6 Analog pins. This only works on pins with
 * analog input capabilities; use anywhere else results in undefined behavior.
 */
#define INPUT_ANALOG 0x00
/**
 * Digital input, no pullup. This is the default state of all digital pins. Beware of power
 * consumption, as digital inputs left "floating" may switch back and forth and cause spurious
 * interrupts.
 */
#define INPUT_FLOATING 0x04
#define INPUT INPUT_FLOATING
/**
 * Digital output, push-pull. This is the mode which should be used to output a digital HIGH
 * or LOW value from the Maple. This mode is useful for pneumatic solenoid valves.
 */
#define OUTPUT 0x01
/**
 * Open-drain output. This is useful in a few cases, namely I2C and fault pins.
 */
#define OUTPUT_OD 0x05

// ---- High-level pin-by-pin control ----

/**
 * analogCalibrate - Calibrates the analog sensor on the specified channel, assuming that it is
 * not actively changing. This is intended to be used in initialize(), NOT initializeIO().
 *
 * Approximately 500 samples are taken, 1 ms apart, for a 0.5 s period of calibrate. The
 * average value thus calculated is returned and stored for later calls to the
 * analogReadCalibrated() and analogReadCalibratedHR() functions.
 */
uint16_t analogCalibrate(uint32_t channel);
/**
 * analogRead - Reads an analog input channel 1-8 and returns the 12-bit value from 0 to 4095.
 * A value of 0 reflects an input voltage of nearly 0 V, and a value of 4095 reflects an input
 * voltage of nearly 5 V.
 *
 * The value returned is undefined if the analog pin has been switched to a different mode.
 * This function is Wiring-compatible with the exception of the larger output range.
 */
uint16_t analogRead(uint32_t channel);
/**
 * analogReadCalibrated - Reads the calibrated value of an analog input channel 1-8; the
 * analogCalibrate() function must be run first on that channel.
 *
 * This function is inappropriate for sensor values intended for integration, as round-off
 * error can accumulate causing drift over time. Use analogReadCalibratedHR() instead.
 */
int16_t analogReadCalibrated(uint32_t channel);
/**
 * analogReadCalibratedHR - Reads the calibrated value of an analog input channel 1-8;
 * analogCalibrate() must be run first. This is intended for integrated sensor values to reduce
 * drift due to round-off, and should not be used on a direct-read or derivative sensor such as
 * a line tracker or potentiometer.
 *
 * The value returned actually has 16 bits of "precision", even though the ADC only reads
 * 12 bits, so that errors induced by the average value being between two values come out
 * in the wash when integrated over time. Think of the value as the true value times 16.
 */
int32_t analogReadCalibratedHR(uint32_t channel);
/**
 * digitalRead - Gets the digital value (1 or 0) of a pin configured as a digital input.
 *
 * If the pin is configured as some other mode, the digital value which reflects the current
 * state of the pin is returned, which may or may not differ from the currently set value. The
 * return value is undefined for pins configured as Analog inputs, or for ports in use by a
 * Communications interface. This function is Wiring-compatible.
 */
bool digitalRead(uint32_t pin);
/**
 * digitalWrite - Sets the digital value (1 or 0) of a pin configured as a digital output.
 *
 * If the pin is configured as some other mode, behavior is undefined. This function is
 * Wiring-compatible.
 */
void digitalWrite(uint32_t pin, bool value);
/**
 * pinMode - Configures the pin as an input or output with a variety of settings. Possible
 * values include INPUT_PULLUP, INPUT_ANALOG, INPUT_FLOATING, OUTPUT, and OUTPUT_OD.
 *
 * This function is Wiring-compatible.
 */
void pinMode(uint32_t pin, uint32_t mode);

// -------------------- Custom sensor control functions --------------------

// ---- I2C port control ----
/**
 * i2cRead - Reads the specified number of data bytes from the specified 7-bit I2C address. The
 * bytes will be stored at the specified location. Returns true if successful or false if
 * failed. If only some bytes could be read, false is still returned.
 *
 * The I2C address should be right-aligned; the R/W bit is automatically supplied.
 *
 * Since most I2C devices use an 8-bit register architecture, this method has limited
 * usefulness. Consider i2cReadRegister instead for the vast majority of applications.
 */
bool i2cRead(uint8_t addr, uint8_t *data, uint16_t count);
/**
 * i2cReadRegister - Reads the specified amount of data from the given register address on
 * the specified 7-bit I2C address. Returns true if successful or false if failed. If only some
 * bytes could be read, false is still returned.
 *
 * The I2C address should be right-aligned; the R/W bit is automatically supplied.
 *
 * Most I2C devices support an auto-increment address feature, so using this method to read
 * more than one byte will usually read a block of sequential registers. Try to merge reads to
 * separate registers into a larger read using this function whenever possible to improve code
 * reliability, even if a few intermediate values need to be thrown away.
 */
bool i2cReadRegister(uint8_t addr, uint8_t reg, uint8_t *value, uint16_t count);
/**
 * i2cWrite - Writes the specified number of data bytes to the specified 7-bit I2C address.
 * Returns true if successful or false if failed. If only smoe bytes could be written, false
 * is still returned.
 *
 * The I2C address should be right-aligned; the R/W bit is automatically supplied.
 *
 * Since most I2C devices use an 8-bit register architecture, this method is mostly useful for
 * setting the register position (most devices remember the last-used address) or writing a
 * sequence of bytes to one register address using an auto-increment feature. In these cases,
 * the first byte written from the data buffer should have the register address to use.
 */
bool i2cWrite(uint8_t addr, uint8_t *data, uint16_t count);
/**
 * i2cWriteRegister - Writes the specified data byte to a register address on the specified
 * 7-bit I2C address. Returns true if successful or false if failed.
 *
 * The I2C address should be right-aligned; the R/W bit is automatically supplied.
 *
 * Only one byte can be written to each register address using this method. While useful for
 * the vast majority of I2C operations, writing multiple bytes requires the i2cWrite method.
 */
bool i2cWriteRegister(uint8_t addr, uint8_t reg, uint16_t value);

// ---- Serial port control ----
/**
 * FILE is an integer referring to a stream. FILE * is the standard library method of referring
 * to a file pointer, even though there is nothing there.
 */
typedef int FILE;
/**
 * 8 data bits (typical)
 */
#define SERIAL_DATABITS_8 0x0000
/**
 * 9 data bits
 */
#define SERIAL_DATABITS_9 0x1000
/**
 * 1 stop bit (typical)
 */
#define SERIAL_STOPBITS_1 0x0000
/**
 * 2 stop bits
 */
#define SERIAL_STOPBITS_2 0x2000
/**
 * No parity (typical)
 */
#define SERIAL_PARITY_NONE 0x0000
/**
 * Even parity
 */
#define SERIAL_PARITY_EVEN 0x0400
/**
 * Odd parity
 */
#define SERIAL_PARITY_ODD 0x0600
/**
 * Default serial settings
 */
#define SERIAL_8N1 0x0000

/**
 * usartInit - Initialize the specified serial interface with the given connection parameters.
 * The baud range may be from 2400 baud to 2250000 baud, and the flag constants are specified
 * in this document. The serial port should be either "uart1" or "uart2".
 *
 * I/O to the port is accomplished using the "standard" I/O functions such as fputs(),
 * fprintf(), and fputc().
 *
 * Re-initializing an open port may cause loss of data in the buffers. This routine may be
 * safely called from initializeIO() or when the scheduler is paused.
 *
 * If I/O is attempted on a serial port which has never been opened, the behavior will be the
 * same as if the port had been disabled. See usartShutdown() for details.
 */
void usartInit(FILE *usart, uint32_t baud, uint16_t flags);
/**
 * usartShutdown - Disables the specified serial interface. Any data in the transmit and receive
 * buffers will be lost. The serial port should be either "uart1" or "uart2".
 *
 * Attempts to read from the port when it is disabled will deadlock, and attempts to write to
 * it may deadlock depending on the state of the buffer.
 */
void usartShutdown(FILE *usart);

// -------------------- Character input and output --------------------

// ---- "Standard" I/O definitions ----
/**
 * Standard input and output streams use the PC debug terminal. Note that dereferencing these
 * pointers will cause a segmentation fault.
 *
 * See the note on getchar() and putchar() about use of this stream over wireless.
 */
#define stdout ((FILE *)4)
#define stdin ((FILE *)4)
/**
 * UART 1 on the Maple; must be opened first using serialOpen().
 */
#define uart1 ((FILE *)1)
/**
 * UART 2 on the Maple; must be opened first using serialOpen(). This matches the location of
 * the Arduino UART.
 */
#define uart2 ((FILE *)2)
/**
 * UART 3 on the Maple; must be opened first using serialOpen().
 */
#define uart3 ((FILE *)3)

// ---- Simple I/O routines ----
/**
 * fcount - Returns the number of characters that can be read without blocking (the number of
 * characters available) from the specified stream. This function may underestimate, but will
 * not overestimate, the number of characters which meet this criteria.
 *
 * If this number cannot be determined, returns 0.
 */
int fcount(FILE *stream);
/**
 * fgetc - Reads and returns one character from the specified stream, blocking until complete.
 *
 * If no character can be read for some reason, returns -1. Do not use fgetc() on a VEX LCD
 * port; deadlock may occur.
 */
int fgetc(FILE *stream);
/**
 * fprint - Prints the simple string to the specified stream, where the stream is one of the
 * constants defined above. This method is much, much faster than fprintf() and does not add a
 * new line like fputs().
 *
 * The string must have a null terminator; if it does not, undefined behavior results. Do not
 * use fprint() on a VEX LCD port. Use lcdSetText() instead.
 */
void fprint(const char *string, FILE *stream);
/**
 * fputc - Writes one character to the specified stream and returns the input value.
 *
 * Do not use fputc() on a VEX LCD port. Use lcdSetText() instead.
 */
int fputc(int value, FILE *stream);
/**
 * fputs - Behaves the same as the "fprint" function, and appends a trailing newline ("\n").
 * Returns the number of characters written, excluding the newline, for standards compliance.
 *
 * Do not use fputs() on a VEX LCD port. Use lcdSetText() instead.
 */
int fputs(const char *string, FILE *stream);
/**
 * getchar - Reads and returns one character from "stdin", which is the PC debug terminal.
 * Input from this source may be unreliable using wireless connections.
 */
int getchar();
/**
 * print - Prints the simple string to the debug terminal without formatting. Once again, this
 * method is much, much faster than printf().
 *
 * The string must have a null terminator; if it does not, undefined behavior results.
 */
void print(const char *string);
/**
 * putchar - Writes one character to "stdout", which is the PC debug terminal, and returns the
 * input value. When using a wireless connection, one may need to press the spacebar before
 * the input is visible on the terminal.
 */
int putchar(int value);
/**
 * puts - Behaves the same as the "print" function, and appends a trailing newline ("\n").
 * Returns the number of characters written, excluding the newline, for standards compliance.
 */
int puts(const char *string);

// ---- Formatted I/O routines ----

/*
 * WARNING: Use of the this family of functions requires at least 48 variables
 *(NOT 64 bytes!) of space on the stack. Tasks using these probably should have a stack size of
 * at least 128 variables, if not 256 or more. The default stack size is probably acceptable.
 */

/**
 * fprintf - Prints the formatted string to the specified output stream. The stream argument is
 * the same value that would be passed to fputc() to output a character to the stream.
 *
 * The specifiers supported by this minimalistic printf() function are:
 * %d: Signed integer in base 10 (int, int32_t)
 * %u: Unsigned integer in base 10 (unsigned int, uint32_t)
 * %x, %X: Unsigned integer in base 16 (unsigned int, uint32_t), works for int if argument >= 0
 * %p: Pointer (void *, int *, ...)
 * %c: Character (char)
 * %s: Null-terminated string (char *)
 * %%: Single literal percent sign
 *
 * %f: Floating-point number, ONLY supported on some builds (in other words, avoid it)
 *
 * Specifiers can be modified with:
 * 0: Zero-pad, instead of space-pad
 * a.b: Make the field at least "a" characters wide. If "b" is specified for "%f", changes the
 *      number of digits after the decimal point
 * -: Left-align, instead of right-align
 * +: Always display the sign character (displays a leading "+" for positive numbers)
 * l: Ignored for compatibility
 *
 * Invalid format specifiers, or mismatched parameters to specifiers, cause undefined behavior.
 * Do not use fprintf() on a VEX LCD port. Use lcdPrint() instead.
 */
int fprintf(FILE *stream, const char *formatString, ...);
/**
 * printf - Prints the formatted string to the debug stream (the PC terminal).
 *
 * For detailed documentation, see fprintf().
 */
int printf(const char *formatString, ...);
/**
 * snprintf - Prints the formatted string to the string buffer with the specified length limit.
 * The length limit, as per the C standard, includes the trailing null character, so an
 * argument of 256 will cause a maximum of 255 non-null characters to be printed, and one null
 * terminator in all cases.
 *
 * For detailed documentation, see fprintf().
 */
int snprintf(char *buffer, size_t limit, const char *formatString, ...);
/**
 * sprintf - Prints the formatted string to the string buffer, which must be big enough to
 * contain the complete formatted output. If it is not, undefined behavior occurs.
 *
 * See snprintf() for a safer version of this function.
 *
 * For detailed documentation, see fprintf().
 */
int sprintf(char *buffer, const char *formatString, ...);

// -------------------- Real-time scheduler functions --------------------
/**
 * The longest possible delay time for a task. This is measured in milliseconds and is over
 * three weeks of continuous uptime, not an issue for VEX applications.
 *
 * Use of this value as a time-out in a call to an API function may cause the calling task to
 * be placed into the Suspended state, rather than Blocked or Delayed.
 */
#define MAX_DELAY ((clock_t)0xFFFFFFFF)
/**
 * Only this many tasks can exist at once; attempts to create further tasks will not succeed
 * until tasks end or are destroyed, AND the idle task cleans them up.
 *
 * Changing this value will not change the limit without a kernel recompile. The idle task
 * and VEX daemon task count against the limit. The user autonomous() or teleop() also counts
 * against the limit, so 9 tasks usually remain for other uses.
 */
#define TASK_MAX 12
/**
 * The maximum number of available task priorities, which run from 0 to 3.
 *
 * Changing this value will not change the priority count without a kernel recompile.
 */
#define TASK_MAX_PRIORITIES 4
/**
 * The lowest priority that can be assigned to a task, which puts it on a level with the idle
 * task. This may cause severe performance problems and is generally not recommended.
 */
#define TASK_PRIORITY_LOWEST 0
/**
 * The default task priority, which should be used for most tasks.
 *
 * Default tasks such as autonomous() inherit this priority.
 */
#define TASK_PRIORITY_DEFAULT 1
/**
 * The highest priority that can be assigned to a task. Unlike the lowest priority, this
 * priority can be safely used without hampering interrupts. Beware of deadlock.
 */
#define TASK_PRIORITY_HIGHEST (TASK_MAX_PRIORITIES - 1)
/**
 * The recommended stack size for a new task that does an average amount of work. This stack
 * size is used for default tasks such as autonomous().
 *
 * This is probably OK for 4-5 levels of function calls and occasional use of printf() with
 * a few arguments. Tasks requiring local buffers or other space will need a bigger stack.
 */
#define TASK_DEFAULT_STACK_SIZE 256
/**
 * The minimum stack depth for a task. Scheduler state is stored on the stack, so even if the
 * task never uses the stack, at least this much space must be allocated.
 *
 * Function calls and other seemingly innocent constructs may place information on the stack.
 * Err on the side of a larger stack when possible.
 */
#define TASK_MINIMAL_STACK_SIZE	64

// ---- Real-time scheduler type definitions ----
/**
 * Type by which tasks are referenced; as this is a pointer type, it can be safely passed or
 * stored by value.
 */
typedef void * TaskHandle;
/**
 * Currently, both mutexes and semaphores use the default FreeRTOS implementation of queues
 * to manage concurrency. It works, but it is somewhat slow. FMX and FSX, two Maple-specific
 * fast methods of handling mutex and semaphore activity, respectively, are for a later update.
 *
 * The API will remain the same if this change is made in the future.
 */
typedef void * MessageQueue;
/**
 * Type by which mutexes are referenced; this is a pointer type.
 */
typedef MessageQueue Mutex;
/**
 * Type by which semaphores are referenced; this is a pointer type.
 */
typedef MessageQueue Semaphore;
/**
 * Task functions must accept one parameter of type "void *"; they need not use it.
 *
 * For example:
 *
 * void MyTask(void *ignore) {
 *     while (1);
 * }
 */
typedef void (*TaskCode)(void *);

// ---- Task control ----

/**
 * taskCreate - Creates a new task and add it to the list of tasks that are ready to run.
 *
 * stackDepth refers to the number of variables available on the stack. The actual stack size is
 * (sizeof(int) * stackDepth), so for the Maple, 4 * stackDepth bytes will be allocated.
 * parameters will be received as the argument to the taskCode function. priority can be a value
 * from TASK_PRIORITY_LOWEST to TASK_PRIORITY_HIGHEST.
 *
 * Returns 0 if the task was successfully created, or nonzero if an error occurred.
 */
uint32_t taskCreate(TaskCode taskCode, const uint32_t stackDepth, void *parameters,
	const uint32_t priority, TaskHandle *taskHandle);
/**
 * taskDelay - Delays the current task for a given number of milliseconds. Delaying for a
 * negative or zero interval forces a reschedule, which may or may not actually switch out
 * the current task.
 *
 * Delaying for a period of MAX_DELAY will suspend the task instead, as if taskSuspend() was
 * called with an argument of NULL.
 *
 * This is not the best method to have a task execute code at predefined intervals, as the
 * delay time is measured from when the delay is requested. To delay cyclically, use
 * taskDelayUntil().
 */
void taskDelay(const clock_t ticksToDelay);
/**
 * taskDelayUntil - Delays the current task until a specified time. The task will be unblocked
 * at the time *previousWakeTime + cycleTime, and *previousWakeTime will be changed to reflect
 * the time at which the task will unblock.
 *
 * If the target time is in the past, no delay occurs, but a reschedule is forced, as if
 * taskDelay() was called with an argument of zero. If the sum of cycleTime and
 * *previousWakeTime overflows or underflows, undefined behavior occurs.
 *
 * This function should be used by cyclical tasks to ensure a constant execution frequency.
 * While taskDelay() specifies a wake time relative to the time at which the function is
 * called, taskDelayUntil() specifies the absolute future time at which it wishes to unblock.
 * Calling taskDelayUntil with the same cycleTime parameter value in a loop, with 
 * previousWakeTime referring to a local variable initialized to millis(), will cause the
 * loop to execute with a fixed period.
 *
 * waitUntil() is an alias of taskDelayUntil(). taskDelayUntil() is clearer, but waitUntil()
 * is easier to type.
 */
void taskDelayUntil(clock_t* previousWakeTime, const clock_t cycleTime);
/**
 * taskDelete - Kills and removes the specified task from the kernel task list. Passing NULL
 * will delete the calling task.
 *
 * Deleting the last task will permanently end the program, possibly leading to undesirable
 * states, as outputs remain in their last set configuration.
 *
 * NOTE: The idle task is responsible for freeing the kernel allocated memory from tasks that
 * have been deleted. It is therefore important that the idle task is not starved of
 * processing time. Memory allocated by the task code is not automatically freed, and should be
 * freed before the task is deleted.
 */
void taskDelete(TaskHandle taskToDelete);
/**
 * taskGetCount - Determines the number of tasks that are currently being managed. This
 * includes all ready, blocked and suspended tasks.
 *
 * A task that has been deleted but not yet freed by the idle task will also be included in
 * the count. Tasks recently created may take one context switch to be counted.
 */
uint32_t taskGetCount();
/**
 * taskPriorityGet - Obtains the priority of the specified task. Passing NULL results in the
 * priority of the calling task being returned.
 */
uint32_t taskPriorityGet(const TaskHandle task);
/**
 * taskPrioritySet - Sets the priority of the specified task. Passing NULL results in the
 * priority of the calling task being changed. The priority must be between 0 and
 * (TASK_MAX_PRIORITIES - 1).
 *
 * A context switch may occur before the function returns if the priority being set is higher
 * than the currently executing task and the task being mutated is available to be scheduled.
 */
void taskPrioritySet(TaskHandle task, const uint32_t newPriority);
/**
 * taskResume - Resumes the specified task. Passing NULL is not allowed, as the current task
 * by definition cannot be in a suspended state.
 *
 * A task that has been suspended by one or more calls to taskSuspend() will be made available
 * for scheduling again by a call to taskResume(). If the task was not suspended at the time
 * of the call to taskResume(), undefined behavior occurs.
 */
void taskResume(TaskHandle taskToResume);
/**
 * taskResumeAll - Resumes real time kernel activity following a call to taskSuspendAll().
 *
 * If resuming the scheduler caused a context switch then true is returned, otherwise false
 * is returned. This value is only really important in an ISR, as when called in a task the
 * context is automatically switched.
 */
bool taskResumeAll();
/**
 * taskResumeISR - An implementation of taskResume() that can be called from within an ISR.
 */
bool taskResumeISR(TaskHandle taskToResume);
/**
 * taskRunLoop - Starts a task which will periodically call the specified function using
 * taskDelayUntil() with a period of increment milliseconds. Intended for use as a quick-start
 * skeleton for cyclic tasks with higher priority than the "main" tasks.
 *
 * The created task will have priority TASK_PRIORITY_DEFAULT + 1 with the default stack size.
 * To customize behavior, create a task manually with the specified function.
 *
 * This task will automatically terminate after one further function invocation when the robot
 * is disabled or when the robot mode is switched.
 */
TaskHandle taskRunLoop(void (*fn)(void), clock_t increment);
/**
 * taskSuspend - Suspends the specified task. Passing NULL results in the calling task being
 * suspended. When suspended a task will not be scheduled, regardless of whether it might be
 * otherwise available to run.
 *
 * This function should be avoided with non-NULL arguments, as any delayed or blocked tasks
 * might not be restored to the correct previous state on exit.
 */
void taskSuspend(TaskHandle taskToSuspend);
/**
 * taskSuspendAll - Suspends all real time kernel activity while keeping interrupts enabled.
 * After calling taskSuspendAll(), the calling task will continue to execute without risk of
 * being swapped out for another process, even one of higher priority, until a call to
 * taskResumeAll() has been made. Interrupts may still run and mutate some aspect of global or
 * scheduler state.
 *
 * Scheduler API functions such as taskDelay() must not be called while the scheduler is
 * suspended, or deadlock will occur. Other API functions may call scheduler functions
 * implicitly, so be careful.
 */
void taskSuspendAll();

// ---- Semaphore functions ----

/**
 * semaphoreCreate - Creates a semaphore. Semaphores created using this function can be
 * accessed using the semaphoreTake() and semaphoreGive() functions. The mutex or queue
 * functions must not be used on objects of this type.
 *
 * This type of object does not need to have balanced take and give calls, so priority
 * inheritance is not used. Semaphores can be signalled by an interrupt routine.
 *
 * Semaphores are intended to synchronize tasks. To prevent some critical code from
 * simultaneously modifying a shared resource, use mutexes instead.
 */
Semaphore semaphoreCreate();
/**
 * semaphoreGive - Signals a semaphore. Tasks waiting for a signal using semaphoreTake() will
 * be unblocked by this call and can continue execution. Slow processes can give semaphores
 * when ready, and fast processes waiting to take the semaphore will continue at that point.
 *
 * Returns true if the semaphore was successfully given, or false if the semaphore was not
 * taken since the last give. This is normally not an issue.
 *
 * Use when the semaphore is uninitialized results in undefined behavior. A version of this
 * function which works in interrupt routines is available in semaphoreGiveISR().
 */
bool semaphoreGive(Semaphore semaphore);
/**
 * semaphoreGiveISR - Signals a semaphore in a manner safe for use in an ISR or interrupt
 * routine. Returns true if a context switch is required (a task with a higher priority than
 * the current task was woken by the signal).
 *
 * The contract of this method otherwise exactly follows that of semaphoreGive().
 */
bool semaphoreGiveISR(Semaphore semaphore, bool *contextSwitch);
/**
 * semaphoreTake - Waits on a semaphore. If the semaphore is already in the "taken" state, the
 * current task will wait up to blockTime (where MAX_DELAY is forever) milliseconds for the
 * semaphore to be signaled.
 *
 * Returns whether the semaphore was successfully taken. If blockTime is MAX_DELAY, this
 * function will always return true.
 *
 * Use when the semaphore is uninitialized results in undefined behavior. This method is
 * obviously unsafe for use in an interrupt routine.
 */
bool semaphoreTake(Semaphore semaphore, clock_t blockTime);
/**
 * semaphoreDelete - Deletes the specified semaphore. This function can be dangerous; deleting
 * semaphores being waited on by a task is undefined behavior.
 *
 * Do not use a NULL argument.
 */
void semaphoreDelete(Semaphore semaphore);

// ---- Mutex functions ----

/**
 * mutexCreate - Creates a mutex. Mutexes created using this function can be accessed using
 * the mutexTake() and mutexGive() functions. The queue or semaphore functions must not be used
 * on objects of this type.
 *
 * This type of object uses a priority inheritance mechanism so a task 'taking' a mutex MUST
 * ALWAYS 'give' the mutex back once the mutex is no longer required. Mutexes cannot be used
 * from interrupt routines.
 *
 * Mutexes are intended to prevent tasks from simultaneously performing some action or using
 * some shared resource. For signalling and synchronization, try using semaphores.
 */
Mutex mutexCreate();
/**
 * mutexGive - Relinquishes a mutex so that other tasks can use the resource it guards. The
 * mutex must be held by the current task using a corresponding call to mutexTake.
 *
 * Returns true if the mutex was released, or false if the mutex was not already held.
 *
 * Use when the mutex is uninitialized results in undefined behavior. Mutexes are inherently
 * unsafe for use in an interrupt routine.
 */
bool mutexGive(Mutex mutex);
/**
 * mutexTake - Requests a mutex so that other tasks cannot simultaneously use the resource
 * it guards. The mutex must not already be held by the current task. If another task already
 * holds the mutex, the function will wait up to blockTime (where MAX_DELAY is forever)
 * milliseconds for the mutex to be released.
 *
 * Returns whether the mutex was successfully taken. If blockTime is MAX_DELAY, this function
 * will always return true.
 *
 * Use when the mutex is uninitialized results in undefined behavior. Mutexes are inherently
 * unsafe for use in an interrupt routine.
 */
bool mutexTake(Mutex mutex, clock_t blockTime);
/**
 * mutexDelete - Deletes the specified mutex. This function can be dangerous; deleting mutexes
 * being waited on by a task is undefined behavior.
 *
 * Do not use a NULL argument.
 */
void mutexDelete(Mutex mutex);

// ---- Aliases ----

/**
 * delay - Wiring-compatible alias of taskDelay().
 */
void delay(unsigned long time);
/**
 * delayMicroseconds - Wait for approximately the given number of microseconds. The method
 * used for delaying this length of time may vary depending on the argument. The current task
 * will always be delayed by at least the specified period, but possibly much more depending
 * on CPU load. In general, this function is less reliable than delay().
 */
void delayMicroseconds(unsigned long us);
/**
 * micros - Returns the number of microseconds since Maple power-up. There are 10^6
 * microseconds in a second, so as a 32-bit integer, this will overflow every two hours or so.
 * The OS will handle overflows properly, but user code must be aware.
 *
 * This function is Wiring-compatible.
 */
unsigned long micros();
/**
 * millis - Returns the number of milliseconds since Maple power-up. There are 1000
 * milliseconds in a second, so as a 32-bit integer, this will not overflow until 50 days or so.
 *
 * This function is Wiring-compatible.
 */
unsigned long millis();
/**
 * wait - Alias of taskDelay() intended to help EasyC users.
 */
void wait(unsigned long time);
/**
 * waitUntil - Alias of taskDelayUntil() intended to help EasyC users.
 */
void waitUntil(unsigned long *previousWakeTime, const unsigned long time);

// End C++ extern to C
#ifdef __cplusplus
}
#endif

#endif
