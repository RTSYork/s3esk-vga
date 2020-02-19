#include "xparameters.h"
#include "xil_types.h"
#include "xuartlite_l.h"

int main (void) {
	// Put frame buffer at start of DDR
	u32 buffer = XPAR_DDR_SDRAM_MPMC_BASEADDR;

	// Set VGA core base address to buffer
	*((volatile u32 *)XPAR_EMBS_VGA_0_BASEADDR) = buffer;

	// Enable VGA core output
	*((volatile u32 *)XPAR_EMBS_VGA_0_BASEADDR + 1) = 1;

	while (1) {
		// Copy UART received byte to next location in frame buffer
		*((volatile u8 *)buffer++) = XUartLite_RecvByte(XPAR_RS232_DTE_BASEADDR);
	}

	return 0;
}