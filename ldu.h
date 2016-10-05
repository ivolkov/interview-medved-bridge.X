#include <stdbool.h>
#include <stdint.h>          /* For uint32_t definition                       */

#define LDUUARTid       UART3
#define LDUResetTimeMs  400
#define LDUMaxDelayMs   25

void LDUInit();
void LDUSetZero();
float LDUGetValue();
void LDUGetDI(bool *DI0, bool *DI1);
void LDUFlush();