/******************************************************************************/
/*  Files to Include                                                          */
/******************************************************************************/

#include <plib.h>           /* Include to use PIC32 peripheral libraries      */
#include <stdint.h>         /* For uint32_t definition                        */
#include <stdbool.h>        /* For true/false definition                      */
#include "system.h"         /* System funct/params, like osc/periph config    */
#include "user.h"           /* User funct/params, such as InitApp             */
#include "errValue.h"
#include "ldu.h"
#include "display.h"

int32_t main(void)
{

    /*Refer to the C32 peripheral library compiled help file for more
    information on the SYTEMConfig function.
    
    This function sets the PB divider, the Flash Wait States, and the DRM
    /wait states to the optimum value.  It also enables the cacheability for
    the K0 segment.  It could has side effects of possibly alter the pre-fetch
    buffer and cache.  It sets the RAM wait states to 0.  Other than
    the SYS_FREQ, this takes these parameters.  The top 3 may be '|'ed
    together:
    
    SYS_CFG_WAIT_STATES (configures flash wait states from system clock)
    SYS_CFG_PB_BUS (configures the PB bus from the system clock)
    SYS_CFG_PCACHE (configures the pCache if used)
    SYS_CFG_ALL (configures the flash wait states, PB bus, and pCache)*/

    SYSTEMConfig(SYS_FREQ, SYS_CFG_ALL); 

    /* Initialize I/O and Peripherals for application */
    InitApp();

    /*Configure Multivector Interrupt Mode.  Using Single Vector Mode
    is expensive from a timing perspective, so most applications
    should probably not use a Single Vector Mode*/
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);

    DisplayInit() ;
    DelayMs( LDUResetTimeMs ) ;
    LDUInit() ;

    while (1) {
        GetD1() ;
        GetD2() ;
        DelayMs( 1 ) ;
        GetDI() ;
        GetD3() ;

        if ( enableUSBConnection )
            SendUSBPacket() ;

#ifndef DBG
        if ( refreshD1 ) {
            PrintD1() ;
            refreshD1 = false ;
        }

        if ( refreshD2 ) {
            PrintD2() ;
            refreshD2 = false ;
        }

        if ( refreshD3 ) {
            if ( fD3 > MinTemperature )
                PrintD3() ;
            refreshD3 = false ;
        }

        if ( refreshDisplayLabels ) {
            DisplayRefreshLabels(fD3 > MinTemperature) ;
            refreshDisplayLabels = false ;
        }

        if ( (manualZeroLDU) || (enableAutoZero && autoZeroLDU) ) {
            LDUSetZero() ;
            autoZeroLDU = false ;
            manualZeroLDU = false ;
        }
#endif
    } 
}
