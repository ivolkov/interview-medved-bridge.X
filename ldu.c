#include <plib.h>
#include <stdbool.h>
#include "ldu.h"
#include "errValue.h"

unsigned char cmdBR[] = { 'B', 'R', 13, 10 } ;                                  // get baudrate
unsigned char cmdBR115200[] = { 'B', 'R', ' ', '1', '1', '5', '2', '0', '0', 13, 10 } ; // set baudrate 115200
unsigned char cmdWP[] = { 'W', 'P', 13, 10 } ;                                  // save parameters
unsigned char cmdSR[] = { 'S', 'R', 13, 10 } ;                                  // firmware reset
unsigned char cmdFL0[] = { 'F', 'L', ' ', '0', 13, 10 } ;                       // filter value = 0
unsigned char cmdFF0[] = { 'F', 'F', ' ', '0', 13, 10 } ;                       // filter factor = 0
unsigned char cmdGG[] = { 'G', 'G', 13, 10 } ;                                  // get gross
unsigned char cmdGN[] = { 'G', 'N', 13, 10 } ;                                  // get net
unsigned char cmdUR[] = { 'U', 'R', ' ', '0', 13, 10 } ;                        // set 90 samples/sec
unsigned char cmdSZ[] = { 'S', 'Z', 13, 10 } ;                                  // set zero
unsigned char cmdST[] = { 'S', 'T', 13, 10 } ;                                  // set tare
unsigned char cmdIN[] = { 'I', 'N', 13, 10 } ;                                  // digital input status

unsigned char lduErr = 0 ;                                                      // LDU error counter
unsigned char LDURXBuf[32] ;
char sGG[8] ;                                                                   // LDU GG command response

void LDUInit()
{
        // get LDU baudrate
    lduErr = 0 ;

    UARTSendString( LDUUARTid, &cmdBR, sizeof(cmdBR) ) ;
    unsigned int res = UARTGetString( LDUUARTid, 32, &LDURXBuf, LDUMaxDelayMs ) ;
    if ( (LDURXBuf[0] != 'B') || (res == 32) ) {                                // if command not recognized

        UARTEnable( LDUUARTid, UART_DISABLE_FLAGS(UART_DISABLE) ) ;             // disable LDUUARTid
        UARTSetDataRate( LDUUARTid, 80000000, 9600 ) ;                          // set baudrate 9600
        UARTEnable( LDUUARTid, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX) ) ;   // enable uart
        UARTSendString( LDUUARTid, &cmdBR115200, sizeof(cmdBR115200) ) ;        // send command "set baudrate 115200" to LDU
        LDUFlush() ;
        UARTSendString( LDUUARTid, &cmdWP, sizeof(cmdWP) ) ;                    // save baudrate in LDU EEPROM
        UARTGetString( LDUUARTid, 32, &LDURXBuf, LDUMaxDelayMs ) ;
        UARTSendString( LDUUARTid , &cmdSR, sizeof(cmdSR) ) ;                   // LDU firmware reset
        DelayMs( LDUResetTimeMs ) ;

        UARTEnable( LDUUARTid, UART_DISABLE_FLAGS(UART_DISABLE) ) ;             // disable LDUUART
        UARTSetDataRate( LDUUARTid, 80000000, 115200 ) ;                        // set baudrate 115200
        UARTEnable( LDUUARTid, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX) ) ;   // enable uart
    }

    UARTSendString( LDUUARTid, &cmdFL0, sizeof(cmdFL0) ) ;
    LDUFlush() ;

    UARTSendString( LDUUARTid, &cmdFF0, sizeof(cmdFF0) ) ;
    LDUFlush() ;

    UARTSendString( LDUUARTid, &cmdUR, sizeof(cmdUR) ) ;
    LDUFlush() ;

    LDUSetZero() ;

    LDUFlush() ;
}

void LDUSetZero()
{
            // set tare
    UARTSendString( LDUUARTid, &cmdST, sizeof(cmdST) ) ;
    LDUFlush() ;
}

float LDUGetValue()
{
    unsigned int rxBytesLeft ;
    unsigned char rxBuf[32] ;
    float res ;

    rxBuf[0] = '\0' ;
    UARTSendString( LDUUARTid, &cmdGN, sizeof(cmdGN) ) ;
    rxBytesLeft = UARTGetString( LDUUARTid, 9, &rxBuf, LDUMaxDelayMs ) ;

    if ( (rxBytesLeft != 0) || (rxBuf[0] != 'N') ) {
        lduErr++ ;
        if ( lduErr > 5 )
            LDUInit() ;
        return errValue ;
    } else {
        lduErr = 0 ;

        sGG[0] = rxBuf[1] ;
        sGG[1] = rxBuf[2] ;
        sGG[2] = '.' ;
        sGG[3] = rxBuf[3] ;
        sGG[4] = rxBuf[4] ;
        sGG[5] = rxBuf[5] ;
        sGG[6] = rxBuf[6] ;
        sGG[7] = '\0' ;

        res = atof( (char *)sGG ) ;
        res *= 1262 ;
        return res ;
    }
}


void LDUGetDI(bool *DI0, bool *DI1)
{
    unsigned int rxBytesLeft ;
    unsigned char rxBuf[32] ;

    rxBuf[0] = '\0' ;
    UARTSendString( LDUUARTid, &cmdIN, sizeof(cmdIN) ) ;
    rxBytesLeft = UARTGetString( LDUUARTid, 8, &rxBuf, LDUMaxDelayMs ) ;

    if ( (rxBytesLeft != 0) || (rxBuf[0] != 'I') || (rxBuf[1] != 'N') ) {
        lduErr++ ;
        if ( lduErr > 5 )
            LDUInit() ;
    } else {
        lduErr = 0 ;

        if ( rxBuf[5] == '1' )
            *DI0 = true ;
        else
            *DI0 = false ;

        if ( rxBuf[6] == '1' )
            *DI1 = true ;
        else
            *DI1 = false ;
    }
}


void LDUFlush()
{
    UARTFlush( LDUUARTid, LDUMaxDelayMs ) ;
}