#define DISPLAYUARTid UART6

void DisplayInit();
void DisplayRefreshLabels(bool PrintTemp);

void DisplayPrintD1(float Value);
void DisplayPrintD2(float Value);
void DisplayPrintD3(float Value);

void DisplayPrintD1Error();
void DisplayPrintD3Error();

void DisplayClearD3();