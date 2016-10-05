#include <plib.h>
#include <stdbool.h>
#include "display.h"
#include "errValue.h"

#define ESC 27

const unsigned char cfg1[] = { ESC, 'R' } ;                                     // Display: reset
const unsigned char cfg2[] = { ESC, 'C', '0' } ;                                // Display: cursor off
const unsigned char cfg3[] = { ESC, 'M', '6' } ;                                // Display: clear-mode; autoflow off
const unsigned char selectT1[] = { ESC, 'O', 10, 1 } ;                          // Display: select c10 r1
const unsigned char selectT2[] = { ESC, 'O', 6, 2 } ;                           // Display: select c6 r2
const unsigned char selectT3[] = { ESC, 'O', 15, 2 } ;                          // Display: select c15 r2
const unsigned char selectD1[] = { ESC, 'O', 1, 1 } ;                           // Display: select c4 r1
const unsigned char selectD2[] = { ESC, 'O', 1, 2 } ;                           // Display: select c1 r2
const unsigned char selectD3[] = { ESC, 'O', 12, 2 } ;                          // Display: select c12 r2

const unsigned char T1[] = { 'H', 'm', ' ', ' ', ' ', ' ', ' ' } ;              // Label: Hm
const unsigned char T2[] = { ' ', 'o', '6', '/', 'm' } ;                        // Label: O6/m
const unsigned char T3[] = { 0xDF, 'C' } ;                                      // Label: C
const unsigned char err1[] = "     er1" ;                                       // Error 1 label
const unsigned char err2[] = "er2" ;                                            // Error 2 label
const unsigned char T3clear[] = "      " ;

unsigned char printBuf[10] ;

void DisplaySelectD1Pos();
void DisplaySelectD2Pos();
void DisplaySelectD3Pos();


void DisplayInit()
{
    UARTSendString( DISPLAYUARTid, &cfg1[0], sizeof(cfg1) ) ;
    DelayMs( 500 ) ;
    UARTSendString( DISPLAYUARTid, &cfg2[0], sizeof(cfg2) ) ;
    UARTSendString( DISPLAYUARTid, &cfg3[0], sizeof(cfg3) ) ;
}

void DisplayRefreshLabels(bool PrintTemp)
{
    UARTSendString( DISPLAYUARTid, &selectT1[0], sizeof(selectT1) ) ;
    UARTSendString( DISPLAYUARTid, &T1[0], sizeof(T1) ) ;
    UARTSendString( DISPLAYUARTid, &selectT2[0], sizeof(selectT2) ) ;
    UARTSendString( DISPLAYUARTid, &T2[0], sizeof(T2) ) ;
    if ( PrintTemp ) {
        UARTSendString( DISPLAYUARTid, &selectT3[0], sizeof(selectT3) ) ;
        UARTSendString( DISPLAYUARTid, &T3[0], sizeof(T3) ) ;
    }
}


void DisplaySelectD1Pos()
{
    UARTSendString( DISPLAYUARTid, &selectD1, sizeof(selectD1) ) ;
}


void DisplaySelectD2Pos()
{
    UARTSendString( DISPLAYUARTid, &selectD2, sizeof(selectD2) ) ;
}


void DisplaySelectD3Pos()
{
    UARTSendString( DISPLAYUARTid, &selectD3, sizeof(selectD3) ) ;
}


void DisplayPrintD1(float Value)
{
    DisplaySelectD1Pos() ;
    unsigned char printSize ;

    if ( abs(Value) < 10 )
        sprintf( printBuf, "%8.2f", Value ) ;
    else if ( abs(Value) < 100 )
        sprintf( printBuf, "%8.1f", Value ) ;
    else
        sprintf( printBuf, "%8.0f ", Value ) ;

    UARTSendString( DISPLAYUARTid, &printBuf, 8 ) ;
}


void DisplayPrintD2(float Value)
{
    DisplaySelectD2Pos() ;
    sprintf( printBuf, "%5.0f", Value ) ;
    UARTSendString( DISPLAYUARTid, &printBuf, 5 ) ;
}


void DisplayPrintD3(float Value)
{
    DisplaySelectD3Pos() ;
    sprintf( printBuf, "%3.0f", Value ) ;
    UARTSendString( DISPLAYUARTid, &printBuf, 3 ) ;
}


void DisplayPrintD1Error()
{
    DisplaySelectD1Pos() ;
    UARTSendString( DISPLAYUARTid, &err1, sizeof(err1) ) ;
}


void DisplayPrintD3Error()
{
    DisplaySelectD3Pos() ;
    UARTSendString( DISPLAYUARTid, &err2, sizeof(err2) ) ;
}


void DisplayClearD3()
{
    DisplaySelectD3Pos() ;
    UARTSendString( DISPLAYUARTid, &T3clear, sizeof(T3clear) ) ;
}