#include <plib.h>
#include <stdbool.h>
#include "mlx.h"
#include "errValue.h"

unsigned short cntr = 0 ;                                                       // I2C max delay counter

bool MLXStartTransfer(bool Restart);
void MLXStopTransfer();
bool MLXTransmitOneByte(unsigned char data);

float MLXGetValue()
{
    const unsigned char mlxSlaveAddress = ( 0x5A << 1 ) ;
    float res = 0.0 ;
    unsigned char dataL = 0 ;
    unsigned char dataH = 0 ;
    unsigned int mlxData = 0 ;
    unsigned char PEC = 0 ;

    if ( !MLXStartTransfer( false ) )                                           // start transfer
        goto I2CError ;
    if ( !MLXTransmitOneByte(mlxSlaveAddress) )                                 // send MLX address
        goto I2CError ;
    if ( !MLXTransmitOneByte(0x07) )                                            // send 0x07 command (Read To from RAM)
        goto I2CError ;
    if ( !MLXStartTransfer(true) )                                              // restart transfer
        goto I2CError ;
    if ( !MLXTransmitOneByte(0xB5) )                                            // send MLX Slave Address and set master for transmission, 0xB5 == ( 0x5A<<1 ) | 0x01
        goto I2CError ;

    if( I2CReceiverEnable(MLXI2Cid, TRUE) == I2C_RECEIVE_OVERFLOW )
        goto    I2CError ;
    cntr = 0 ;
    while ( !I2CReceivedDataIsAvailable(MLXI2Cid) ) {                             // wait for 1st byte (DataL)
        cntr++ ;
        if ( cntr == 0 )
            goto I2CError ;
    }
    dataL = I2CGetByte(MLXI2Cid);

    if( I2CReceiverEnable(MLXI2Cid, TRUE) == I2C_RECEIVE_OVERFLOW )
        goto    I2CError ;
    cntr = 0 ;
    while ( !I2CReceivedDataIsAvailable(MLXI2Cid) ) {                             // wait for 2nd byte (DataH)
        cntr++ ;
        if ( cntr == 0 )
            goto I2CError ;
    }
    dataH = I2CGetByte(MLXI2Cid);

    if( I2CReceiverEnable(MLXI2Cid, TRUE) == I2C_RECEIVE_OVERFLOW )
        goto    I2CError ;
    cntr = 0 ;
    while ( !I2CReceivedDataIsAvailable(MLXI2Cid) ) {                             // wait for 3rd byte (PEC)
        cntr++ ;
        if ( cntr == 0 )
            goto I2CError ;
    }
    PEC = I2CGetByte(MLXI2Cid);

    MLXStopTransfer() ;

        // calculate Data (DataL+DataH)
    *((unsigned char *)(&mlxData)) = dataL ;
    *((unsigned char *)(&mlxData)+1) = dataH ;

    res = ( mlxData * 0.02 ) - 273.15 ;
    return res ;

I2CError:
    MLXStopTransfer() ;
    return errValue ;
}

bool MLXStartTransfer(bool Restart)
{
    if( Restart ) {
        I2CRepeatStart( MLXI2Cid );
    } else {
        cntr = 0 ;
        while( !I2CBusIsIdle( MLXI2Cid ) ) {
            cntr++ ;
            if ( cntr == 0)
                return false ;
        }
        if( I2CStart(MLXI2Cid) != I2C_SUCCESS )
            return false ;
    }

    cntr = 0 ;
    while ( !(I2CGetStatus(MLXI2Cid) & I2C_START) ) {
        cntr++ ;
        if ( cntr == 0 )
            return false ;
    }
    return true ;
}


void MLXStopTransfer()
{
    I2CStop(MLXI2Cid);
    cntr = 0 ;
    while ( !(I2CGetStatus(MLXI2Cid) & I2C_STOP) ) {
        cntr++ ;
        if ( cntr == 0 )
            return ;
    }
}


bool MLXTransmitOneByte(unsigned char data)
{
    cntr = 0 ;
    while( !I2CTransmitterIsReady(MLXI2Cid) ) {
        cntr++ ;
        if ( cntr == 0 )
            return false ;
    }
    if( I2CSendByte(MLXI2Cid, data) == I2C_MASTER_BUS_COLLISION )
        return false ;
    cntr = 0 ;
    while( !I2CTransmissionHasCompleted(MLXI2Cid) ) {
        cntr++ ;
        if ( cntr == 0 )
            return false ;
    }
    bool res = I2CByteWasAcknowledged( MLXI2Cid ) ;
    return res ;
}