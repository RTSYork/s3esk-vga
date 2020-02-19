#include "xparameters.h"
#include <stdio.h>
#include "xil_types.h"
#include "xuartlite_l.h"


///////////////////////////////////////////////////
// Set these values to match those chosen in XPS //
///////////////////////////////////////////////////
#define WIDTH          800
#define HEIGHT         600
#define BITS_PER_PIXEL 8
///////////////////////////////////////////////////


// Set frame buffer location to start of DDR
#define FRAME_BUFFER XPAR_DDR_SDRAM_MPMC_BASEADDR


#if (BITS_PER_PIXEL == 4)

// Colour definitions (2 pixels per byte)
#define BLACK   0b00000000
#define WHITE   0b01110111
#define RED     0b01000100
#define GREEN   0b00100010
#define BLUE    0b00010001
#define CYAN    0b00110011
#define YELLOW  0b01100110
#define MAGENTA 0b01010101

// Draws a rectangle of solid colour on the screen
void drawRect(int xLoc, int yLoc, int width, int height, u8 colour) {
	int x, y;

	for (y = yLoc; y < yLoc + height; y++) {
		for (x = xLoc; x < xLoc + width; x+=2) {
			*((volatile u8 *) FRAME_BUFFER + x/2 + (WIDTH/2 * y)) = colour;
		}
	}
}

// Fills the screen with a 1-pixel chequered pattern
void fillChequers(void) {
	int x, y, odd = 0;

	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x+=2) {
			*((volatile u8 *) FRAME_BUFFER + x/2 + (WIDTH/2 * y)) = (odd ? 0b01110000 : 0b00000111);
		}
		odd = !odd;
	}
}

// Fills the screen with 1-pixel horizontal stripes
void fillStripes(void) {
	int x, y, odd = 0;

	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x+=2) {
			*((volatile u8 *) FRAME_BUFFER + x/2 + (WIDTH/2 * y)) = (odd ? BLACK : WHITE);
		}
		odd = !odd;
	}
}

// Fills quarters of the screen with the specified colours
void fillQuarters(u8 colour1, u8 colour2) {
	drawRect(0, 0, WIDTH/2, HEIGHT/2, colour1);
	drawRect(WIDTH/2, 0, WIDTH/2, HEIGHT/2, colour2);
	drawRect(0, HEIGHT/2, WIDTH/2, HEIGHT/2, colour2);
	drawRect(WIDTH/2, HEIGHT/2, WIDTH/2, HEIGHT/2, colour1);
}

// Draws a white 1-pixel dot in each corner of the screen
void drawDots(void) {
	*((volatile u8 *) FRAME_BUFFER) = 0b01110000;
	*((volatile u8 *) FRAME_BUFFER + WIDTH/2 - 1) = 0b00000111;
	*((volatile u8 *) FRAME_BUFFER + ((HEIGHT - 1) * WIDTH/2)) = 0b01110000;
	*((volatile u8 *) FRAME_BUFFER + ((HEIGHT - 1) * WIDTH/2) + (WIDTH/2 - 1)) = 0b00000111;
}

// Draws a white border around the screen
void drawBorder(void) {
	int i;
	for (i = 0; i < HEIGHT; i++) {
		*((volatile u8 *) FRAME_BUFFER + (i * WIDTH/2)) = 0b01110000;
		*((volatile u8 *) FRAME_BUFFER + (i * WIDTH/2) + WIDTH/2 - 1) = 0b00000111;
	}
	for (i = 0; i < WIDTH/2; i++) {
		*((volatile u8 *) FRAME_BUFFER + i) = WHITE;
		*((volatile u8 *) FRAME_BUFFER + i + ((HEIGHT - 1) * WIDTH/2)) = WHITE;
	}
}

#elif (BITS_PER_PIXEL == 8)

// Colour definitions (1 pixel per byte)
#define BLACK   0b00000000
#define WHITE   0b00000111
#define RED     0b00000100
#define GREEN   0b00000010
#define BLUE    0b00000001
#define CYAN    0b00000011
#define YELLOW  0b00000110
#define MAGENTA 0b00000101

// Draws a rectangle of solid colour on the screen
void drawRect(int xLoc, int yLoc, int width, int height, u8 colour) {
	int x, y;

	for (y = yLoc; y < yLoc + height; y++) {
		for (x = xLoc; x < xLoc + width; x++) {
			*((volatile u8 *) FRAME_BUFFER + x + (WIDTH * y)) = colour;
		}
	}
}

// Fills the screen with a 1-pixel chequered pattern
void fillChequers(void) {
	int x, y, odd = 0;

	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			*((volatile u8 *) FRAME_BUFFER + x + (WIDTH * y)) = (odd ? BLACK : WHITE);
			odd = !odd;
		}
		odd = !odd;
	}
}

// Fills the screen with 1-pixel horizontal stripes
void fillStripes(void) {
	int x, y, odd = 0;

	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			*((volatile u8 *) FRAME_BUFFER + x + (WIDTH * y)) = (odd ? BLACK : WHITE);
		}
		odd = !odd;
	}
}

// Fills quarters of the screen with the specified colours
void fillQuarters(u8 colour1, u8 colour2) {
	drawRect(0, 0, WIDTH/2, HEIGHT/2, colour1);
	drawRect(WIDTH/2, 0, WIDTH/2, HEIGHT/2, colour2);
	drawRect(0, HEIGHT/2, WIDTH/2, HEIGHT/2, colour2);
	drawRect(WIDTH/2, HEIGHT/2, WIDTH/2, HEIGHT/2, colour1);
}

// Draws a white 1-pixel dot in each corner of the screen
void drawDots(void) {
	*((volatile u8 *) FRAME_BUFFER) = WHITE;
	*((volatile u8 *) FRAME_BUFFER + WIDTH - 1) = WHITE;
	*((volatile u8 *) FRAME_BUFFER + ((HEIGHT - 1) * WIDTH)) = WHITE;
	*((volatile u8 *) FRAME_BUFFER + ((HEIGHT - 1) * WIDTH) + (WIDTH - 1)) = WHITE;
}

// Draws a white border around the screen
void drawBorder(void) {
	int i;
	for (i = 0; i < HEIGHT; i++) {
		*((volatile u8 *) FRAME_BUFFER + (i * WIDTH)) = WHITE;
		*((volatile u8 *) FRAME_BUFFER + (i * WIDTH) + WIDTH - 1) = WHITE;
	}
	for (i = 0; i < WIDTH; i++) {
		*((volatile u8 *) FRAME_BUFFER + i) = WHITE;
		*((volatile u8 *) FRAME_BUFFER + i + ((HEIGHT - 1) * WIDTH)) = WHITE;
	}
}

#endif


// Waits for any byte to be received over UART
void waitForKey(void) {
	XUartLite_RecvByte(XPAR_RS232_DTE_BASEADDR);
}



int main(void) {
	print("\r\n----\r\nVGA test running. Press any key to enable output.\r\n");

	// Set VGA core frame buffer location
	*((volatile unsigned int *) XPAR_EMBS_VGA_0_BASEADDR) = FRAME_BUFFER;

	waitForKey();

	while (1) {
		print("Enabling output\r\n");
		*((volatile unsigned int *) XPAR_EMBS_VGA_0_BASEADDR + 1) = 1;
		waitForKey();

		print("Chequers\r\n");
		fillChequers();
		waitForKey();

		print("Stripes\r\n");
		fillStripes();
		waitForKey();

		print("Squares\r\n");
		fillQuarters(BLACK, WHITE);
		waitForKey();

		print("Coloured squares\r\n");
		fillQuarters(BLUE, GREEN);
		waitForKey();

		print("Red screen\r\n");
		drawRect(0, 0, WIDTH, HEIGHT, RED);
		waitForKey();

		print("Green screen\r\n");
		drawRect(0, 0, WIDTH, HEIGHT, GREEN);
		waitForKey();

		print("Blue screen\r\n");
		drawRect(0, 0, WIDTH, HEIGHT, BLUE);
		waitForKey();

		print("White screen\r\n");
		drawRect(0, 0, WIDTH, HEIGHT, WHITE);
		waitForKey();

		print("Black screen\r\n");
		drawRect(0, 0, WIDTH, HEIGHT, BLACK);
		waitForKey();

		print("Corner dots\r\n");
		drawDots();
		waitForKey();

		print("Screen border\r\n");
		drawBorder();
		waitForKey();

		print("Changing frame buffer location\r\n");
		*((volatile unsigned int *) XPAR_EMBS_VGA_0_BASEADDR) = FRAME_BUFFER + 0x00500000;
		waitForKey();

		print("Restoring frame buffer location\r\n");
		*((volatile unsigned int *) XPAR_EMBS_VGA_0_BASEADDR) = FRAME_BUFFER;
		waitForKey();

		print("Disabling output\r\n");
		*((volatile unsigned int *) XPAR_EMBS_VGA_0_BASEADDR + 1) = 0;
		waitForKey();
	}

	return 0;
}