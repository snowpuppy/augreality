/*
 * itm.h - Integrated Trace Macrocell (ITM) driver for reporting data back to the PC over the
 * SWO two-wire debug protocol using the standard printf(), puts() functions
 */

#ifndef ITM_H_
#define ITM_H_

#include <stdint.h>

// Initialize ITM macrocell
void itmInit();

#endif
