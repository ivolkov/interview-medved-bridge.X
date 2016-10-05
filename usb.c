#include <plib.h>
#include <stdbool.h>
#include "usb.h"
#include "user.h"
#include "system.h"

#define rxBufSize 6

unsigned char rxBuf[rxBufSize] ;
unsigned char rxCnt ;

unsigned char FloatCharBuf[4] ;

/* request packet structure:
 *
 *          byte0                 bytes 2..5       last byte
 * {command_number_uint_8} [arguments (1..4 bytes)] {'\r'}
 *
 *
 * response packet structure:
 *
 *       last byte
 * [data] {'\r'}
 *
 *
 * commands list:
 *
 * 1: find command (check for communication)
 *   request 2 bytes:   |1|\r|
 *   response 11 bytes: |mdv_device|\r|
 *
 * 2: enable USB transmission
 *   request 2 bytes:  |2|\r|
 *   response: none
 *
 * 3: disable USB transmission
 *   request 2 bytes:  |3|\r|
 *   response: none
 *
 * 4: enable LDU Auto-Zero
 *   request 2 bytes: |4|\r|
 *   response: none
 *
 * 5: disable LDU Auto-Zero
 *   request 2 bytes: |5|\r|
 *   response: none
 *
 * 6: manual zero LDU
 *   request 2 bytes: |6|\r|
 *   response: none
 *
 */

void ClearRxBuf()
{
    rxBuf[0] = 0 ;
    rxCnt = 0 ;
}

void ProcessUSBData(unsigned char Data)
{
    unsigned char initResponse[] = { 'm', 'd', 'v', '_', 'd', 'v', 'c', '\r' } ;
    const unsigned char initResponseSize = 8 ;

    rxBuf[rxCnt] = Data ;
    rxCnt++ ;

    if ( rxCnt > rxBufSize ) {
        ClearRxBuf() ;
        return ;
    }

    if ( Data == '\r' ) {
        if ( rxCnt < 2 ) {
            ClearRxBuf() ;
            return ;
        }

        switch ( rxBuf[0] ) {
            case 1:
                ClearRxBuf() ;
                UARTSendString( USBUARTid, initResponse, initResponseSize ) ;
                break ;
            case 2:
                TMR4 = 0 ;
                T4CONbits.ON = 1 ;
                enableUSBConnection = true ;
                break ;
            case 3:
                enableUSBConnection = false ;
                T4CONbits.ON = 0 ;
                break ;
            case 4:
                enableAutoZero = true ;
                break ;
            case 5:
                enableAutoZero = false ;
                break ;
            case 6:
                manualZeroLDU = true ;
                break ;
        }

        ClearRxBuf() ;
    }
}

void BreakFloatValue(float *Value, unsigned char *buf)
{
    union {
        unsigned char charPart[4] ;
        float floatPart ;
    } breakBuf ;

    breakBuf.floatPart = *Value ;

    buf[0] = breakBuf.charPart[0] ;
    buf[1] = breakBuf.charPart[1] ;
    buf[2] = breakBuf.charPart[2] ;
    buf[3] = breakBuf.charPart[3] ;
}

void SendUSBPacket()
{
    unsigned char txBuf[18] ;
    float timerValue = (float)TMR4 / (float)SYS_FREQ ;

    TMR4 = 0 ;

#ifdef DBG
    fD1 = 2999.9 + timerValue ;
    fD2 = 40.0 + timerValue ;
    fD3 = 119.9 + timerValue ;
    DI0 = !DI0 ;
    DI1 = !DI1 ;
#endif 

    BreakFloatValue( &timerValue, &txBuf[0] ) ;
    BreakFloatValue( &fD1, &txBuf[4] ) ;
    BreakFloatValue( &fD2, &txBuf[8] ) ;
    BreakFloatValue( &fD3, &txBuf[12] ) ;

    txBuf[16] = DI0 ;
    txBuf[17] = DI1 ;

    UARTSendString( USBUARTid, txBuf, 18 ) ;
}