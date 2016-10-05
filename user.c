#include <plib.h>            /* Include to use PIC32 peripheral libraries     */
#include <stdint.h>          /* For uint32_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */
#include "user.h"            /* variables/params used by user.c               */
#include "errValue.h"
#include "ldu.h"
#include "mlx.h"
#include "display.h"
#include "usb.h"

unsigned int ICTmr[ICAvgCnt] ;
unsigned int ICCnt ;

float fD1 = 0.0 ;
float fD2 = 0.0 ;
float fD3 = 0.0 ;

#ifdef DBG
bool DI0 = false ;
bool DI1 = true ;
#else
bool DI0 ;
bool DI1 ;
#endif

bool refreshD1 = false ;
bool refreshD2 = false ;
bool refreshD3 = false ;
bool refreshDisplayLabels = false ;
bool enableAutoZero = true ;
bool autoZeroLDU = false ;
bool manualZeroLDU = false ;
bool enableUSBConnection = false ;

void InitApp(void)
{
            // IO ports configuration
    PORTSetPinsDigitalOut( IOPORT_B, BIT_2 | BIT_3 ) ;
    PORTClearBits( IOPORT_B, BIT_2 | BIT_3 ) ;

    ClearICTmr() ;

            // UART for display
    UARTConfigure( DISPLAYUARTid, UART_ENABLE_PINS_TX_RX_ONLY ) ;
    UARTSetFifoMode( DISPLAYUARTid, UART_INTERRUPT_ON_TX_DONE ) ;
    UARTSetLineControl( DISPLAYUARTid, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1 ) ;
    UARTSetDataRate( DISPLAYUARTid, 80000000, 9600 ) ;
    UARTEnable( DISPLAYUARTid, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_TX) ) ;

            // UART for LDU
    UARTConfigure( LDUUARTid, UART_ENABLE_PINS_TX_RX_ONLY ) ;
    UARTSetFifoMode( LDUUARTid, UART_INTERRUPT_ON_TX_DONE ) ;
    UARTSetLineControl( LDUUARTid, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1 ) ;
    UARTSetDataRate( LDUUARTid, 80000000, 115200 ) ;
    UARTEnable( LDUUARTid, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX) ) ;

            // UART for USB
    UARTConfigure( USBUARTid, UART_ENABLE_PINS_TX_RX_ONLY ) ;
    UARTSetFifoMode( USBUARTid, UART_INTERRUPT_ON_RX_NOT_EMPTY ) ;
    UARTSetLineControl( USBUARTid, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1 ) ;
    UARTSetDataRate( USBUARTid, 80000000, 115200 ) ;
    UARTEnable( USBUARTid, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX) ) ;

            // I2C for MLX
    I2CConfigure( MLXI2Cid, I2C_ENABLE_SMB_SUPPORT | I2C_ENABLE_SLAVE_CLOCK_STRETCHING  ) ;
    I2CSetFrequency( MLXI2Cid, 80000000, 100000 ) ;                             // 100 kHz @ 80 MHz core
    DelayMs( 200 ) ;                                                            // wait after MLX90614 POR (Tvalid = 150ms)
    I2CEnable( MLXI2Cid, true ) ;

            // Timer 2,3 configuration for IC
    T2CONbits.T32 = 1 ;                                                         // 32-bit mode (Timer2 + Timer3)
    T2CONbits.TCKPS = 0b000 ;                                                   // 1:1 prescale value
    T2CONbits.TGATE = 0 ;                                                       // gated time accumulation is disabled
    T2CONbits.SIDL = 1 ;                                                        // discontinue operation when device enters Idle mode
    PR2 = 40000000 ;                                                            // 0.5 sec timeout
    T2CONbits.ON = 0 ;                                                          // timer disabled

            // IC
    IC1CONbits.ICM = 0x02 ;                                                     // simple capture event mode - every rising edge
    IC1CONbits.ICI = 0x00 ;                                                     // interrupt on every capture event
    IC1CONbits.C32 = 0x01 ;                                                     // 32-bit timer resource capture
    IC1CONbits.SIDL = 0x00 ;                                                    // continue to operate in CPU Idle mode
    IC1CONbits.ON = 0x01 ;                                                      // module enabled

            // Display refresh timer
    T1CONbits.TCS = 0 ;                                                         // internal periphial clock
    T1CONbits.TCKPS = 0b11 ;                                                    // 1:256 prescaler (overflow after 0.2097152)
    T1CONbits.TGATE = 0 ;                                                       // gated time accumulation is disabled
    T1CONbits.SIDL = 1 ;                                                        // discontinue operation when device enters Idle mode
    T1CONbits.TON = 1 ;                                                         // timer enabled

            // send USB packet timer
    T4CONbits.SIDL = 1 ;                                                        // discontinue operation in idle mode
    T4CONbits.T32 = 1 ;                                                         // 32bit timer mode
    T4CONbits.TCKPS = 0b000 ;                                                   // 1:1 prescaler
    T4CONbits.TGATE = 0 ;                                                       // gated accumulation is disabled
    PR4 = 0xFFFFFFFF ;                                                          // 51.53960754 sec timeout
    T4CONbits.ON = 0 ;                                                          // timer disabled

            // interrupts
    INTEnableSystemMultiVectoredInt() ;

    INTEnable( INT_T1 ,INT_ENABLED ) ;                                          // enable Timer1 interrupt
    INTSetVectorPriority( INT_T1, INT_PRIORITY_LEVEL_2 ) ;                      // with priority = 2

    INTEnable( INT_SOURCE_UART_RX(USBUARTid), INT_ENABLED ) ;                   // enable USB UARD RX interrupt
    INTSetVectorPriority( INT_VECTOR_UART(USBUARTid), INT_PRIORITY_LEVEL_3 ) ;  //   with priority 3

    ConfigIntTimer23( T23_INT_ON | T23_INT_PRIOR_5 ) ;                          // enable Timer2 interrupt
    ConfigIntCapture1( IC_INT_ON | IC_INT_PRIOR_6 ) ;                           // enable IC1 interrupt
}


void UARTSendString(UART_MODULE id, unsigned char *data, unsigned char len)
{
    unsigned char i ;

    while ( !UARTTransmitterIsReady ) ;

    for ( i = 0 ; i < len ; i++ ) {
        while ( !UARTTransmissionHasCompleted(id) ) ;
        UARTSendDataByte( id, data[i] ) ;
    }
}


unsigned int UARTGetString(UART_MODULE ModuleID, unsigned int length, unsigned char *buffer, unsigned int MaxDelayMs)
{
    const unsigned int uart_data_wait = MaxDelayMs * 40000 ;                   // max delay in cycles

    while( length ) {
        WriteCoreTimer( 0 ) ;
        while ( !UARTReceivedDataIsAvailable(ModuleID) ) {
            if ( ReadCoreTimer() > uart_data_wait )
                return length ;
        }
        *buffer++ = UARTGetData(ModuleID).data8bit ;                               /* data word from HW buffer to SW buffer */
        length-- ;
    }

    return (length ) ;                                                          /* number of data yet to be received i.e.,0 */
}


void UARTFlush(UART_MODULE ModuleID, unsigned int DelayMs)
{
    unsigned char buff[32] ;

    UARTGetString( ModuleID, 32, (unsigned char *)&buff, DelayMs ) ;
}


void DelayMs(unsigned short value)
{
    unsigned int period = value * 40000 ;
    WriteCoreTimer( 0 ) ;
    while ( ReadCoreTimer() < period ) { }
}


void ClearICTmr()
{
    unsigned int i ;
    for ( i = 0 ; i < ICAvgCnt ; i++ )
        ICTmr[i] = 0 ;
    ICCnt = 0 ;
}


void GetD1()
{
#ifndef DBG
    fD1 = LDUGetValue() ;

    if ( abs(fD1) > MaxTorque )
        PORTSetBits( IOPORT_B, BIT_2 ) ;
    else
        PORTClearBits( IOPORT_B, BIT_2 ) ;
#endif
}


void GetD2()
{
#ifndef DBG
    unsigned int i ;
    unsigned int sum = 0 ;
    float avgIC ;

    for ( i = 0 ; i < ICCnt ; i++ )
        sum += ICTmr[i] ;

    if ( sum == 0 )
        fD2 = 0.0 ;
    else {
        avgIC = (float)sum / (float)ICCnt ;
        fD2 = 13937500.0 / avgIC ;

        //fD2 = 1.0 / ( avgIC * 0.000000012 ) ;
    }
#endif
}


void GetD3()
{
#ifndef DBG
    fD3 = MLXGetValue() ;
    if ( fD3 > 20.0 )
        fD3 = (fD3 * 1.65) - 15.46 ;

    if ( fD3 > MaxTemperature )
        PORTSetBits( IOPORT_B, BIT_3 ) ;
    else {
        PORTClearBits( IOPORT_B, BIT_3 ) ;
        if ( fD3 < MinTemperature )
            DisplayClearD3() ;
    }
#endif
}


void GetDI()
{
    LDUGetDI( &DI0, &DI1 ) ;
}


void PrintD1()
{
    if ( fD1 == errValue )
        DisplayPrintD1Error() ;
    else
        DisplayPrintD1( fD1 ) ;
}


void PrintD2()
{
    DisplayPrintD2( fD2 ) ;
}


void PrintD3()
{
    if ( fD3 == errValue )
        DisplayPrintD3Error() ;
    else
        DisplayPrintD3( fD3 ) ;
}