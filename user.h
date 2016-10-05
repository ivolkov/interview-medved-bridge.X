#define MaxTemperature 120
#define MinTemperature 50
#define MaxTorque 3000

#define ICAvgCnt 60

// DEBUG DEFINE HERE
//#define DBG
// DEBUG DEFINE HERE

extern float fD1;
extern float fD2;
extern float fD3;
extern unsigned int ICTmr[ICAvgCnt];
extern unsigned int ICCnt;
extern bool DI0;
extern bool DI1;

extern bool refreshD1;
extern bool refreshD2;
extern bool refreshD3;
extern bool refreshDisplayLabels;
extern bool enableUSBConnection;
extern bool enableAutoZero;
extern bool autoZeroLDU;
extern bool manualZeroLDU;

void InitApp(void);                                                            // I/O and Peripheral Initialization
void UARTSendString(UART_MODULE id, unsigned char *data, unsigned char len);
unsigned int UARTGetString(UART_MODULE ModuleID, unsigned int length, unsigned char *buffer, unsigned int MaxDelayMs);
void UARTFlush(UART_MODULE ModuleID, unsigned int DelayMs);
void ClearICTmr();                                                             // clear ICTmr values array
void DelayMs(unsigned short value);                                            // delay routine in milliseconds
void refreshScreen();                                                         // refresh screen labels
void GetD1();                                                                  // refresh D1 value
void GetD2();                                                                  // refresh D2 value
void GetD3();                                                                  // refresh D3 value
void PrintD1();                                                                // print D1 value
void PrintD2();                                                                // print D2 value
void PrintD3();                                                                // print D3 value


