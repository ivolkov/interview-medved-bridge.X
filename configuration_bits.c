/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <plib.h>            /* Include to use PIC32 peripheral libraries     */

/******************************************************************************/
/* Configuration Bits                                                         */
/*                                                                            */
/* Refer to 'C32 Configuration Settings' under the Help > Contents            */
/* > C32 Toolchain in MPLAB X IDE for available PIC32 Configurations.  For    */
/* additional information about what the hardware configurations mean in      */
/* terms of device operation, refer to the device datasheet 'Special Features'*/
/* chapter.                                                                   */
/*                                                                            */
/******************************************************************************/

/* Fill in your configuration bits here.  The general style is shown below.
The Debug Configuration bit is handline by MPLAB and should not be embedded
in the configuration macro.*/

// SRS Select:
#pragma config FSRSSEL = PRIORITY_0         // SRS Priority 0

// Ethernet RMII/MII Enable:
#pragma config FETHIO = ON                  // Default Ethernet I/O

// USB VBUS ON Selection:
#pragma config FVBUSONIO = ON               // Controlled by USB Module

// PLL Input Divider:
#pragma config FPLLIDIV = DIV_2             // 2x Divider

// PLL Multiplier:
#pragma config FPLLMUL = MUL_20             // 20x Multiplier

// USB PLL Input Divider:
#pragma config UPLLIDIV = DIV_1             // 1x Divider

// USB PLL Enable:
#pragma config UPLLEN = OFF                 // Disabled and Bypassed

// System PLL Output Clock Divider:
#pragma config FPLLODIV = DIV_1             // PLL Divide by 1

// Oscillator Selection Bits:
#pragma config FNOSC = FRCPLL               // Fast RC with PLL Osc (FRCPLL)

// Secondary Oscillator Enable:
#pragma config FSOSCEN = OFF                // Disabled

// Internal/External Switch Over:
#pragma config IESO = OFF                   // Disabled

// Primary Oscillator Configuration:
#pragma config POSCMOD = HS                 // HS osc mode

// CLKO Output Signal Active on the OSCO Pin:
#pragma config OSCIOFNC = OFF                // Disabled

// Peripheral Clock Divisor:
#pragma config FPBDIV = DIV_1               // Pb_Clk is Sys_Clk/1

// Clock Switching and Monitor Selection:
#pragma config FCKSM = CSDCMD               // Clock Switch Disable, FSCM Disabled

// Watchdog Timer Postscaler:
#pragma config WDTPS = PS1                  // 1:1

// Watchdog Timer Enable:
#pragma config FWDTEN = OFF                 // WDT Disabled (SWDTEN Bit Controls)

// Background Debugger Enable:
#pragma config DEBUG = ON                 // Debugger is disabled

// ICE/ICD Comm Channel Select:
#pragma config ICESEL = ICS_PGx1            // ICE EMUC1/EMUD1 pins shared with PGC1/PGD1

// Program Flash Write Protect:
#pragma config PWP = OFF                    // Disable

// Boot Flash Write Protect bit:
#pragma config BWP = OFF                    // Protection Disabled

// Code Protect:
#pragma config CP = OFF                     // Protection Disabled
