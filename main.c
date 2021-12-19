/**
 * @file main.c
 * @author Tereza Burianova (xburia28@stud.fit.vutbr.cz)
 * @brief
 * @date 2021-12-14
 */

#include "display.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK 0x1Fu
#define GPIO_PIN(x) (((1)<<(x & GPIO_PIN_MASK)))

// LED digits
#define LED_C1 ~0xC100
#define LED_C2 ~0xE000
#define LED_C3 ~0xD000
#define LED_C4 ~0xC200
#define DELAY_TIME 6000

// SENSOR pins
#define Trig 0x1000000
#define Echo 0x2000000

unsigned int compare = 0x78;
int measurement = 0;
int waitForEcho = 1;
long int result = 0;
char resultS[4];
int trig = 0;
int echo = 0;

/* A delay function */
void delay(long long bound) {
  long long i;
  for(i=0;i<bound;i++);
}

/* Initialize the MCU - basic clock settings, turning the watchdog off */
void MCUInit(void) {
    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

void PortsInit(void) {
    /* Turn on all port clocks */
    SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTD_MASK;

    PORTA->PCR[25] = ( PORT_PCR_ISF(0x01) /* Nuluj ISF (Interrupt Status Flag) */ // Echo
    				| PORT_PCR_IRQC(0x0A) /* Interrupt enable on failing edge */
    				| PORT_PCR_MUX(0x01) /* Pin Mux Control to GPIO */
    				| PORT_PCR_PE(0x01) /* Pull resistor enable... */
    				| PORT_PCR_PS(0x01)); /* ...select Pull-Up */

    NVIC_ClearPendingIRQ(PORTA_IRQn);
    NVIC_EnableIRQ(PORTA_IRQn);

    // LED - PORT A, set pins for GPIO
    PORTA->PCR[6] = PORT_PCR_MUX(0x01); // pin 25
    PORTA->PCR[7] = PORT_PCR_MUX(0x01); // pin 27
    PORTA->PCR[8] = PORT_PCR_MUX(0x01); // pin 23
    PORTA->PCR[9] = PORT_PCR_MUX(0x01); // pin 28
    PORTA->PCR[10] = PORT_PCR_MUX(0x01); // pin 24
    PORTA->PCR[11] = PORT_PCR_MUX(0x01); // pin 26

    // LED - PORT D, set pins for GPIO
    PORTD->PCR[8] = PORT_PCR_MUX(0x01); // pin 17
    PORTD->PCR[9] = PORT_PCR_MUX(0x01); // pin 18
    PORTD->PCR[12] = PORT_PCR_MUX(0x01); // pin 19
    PORTD->PCR[13] = PORT_PCR_MUX(0x01); // pin 20
    PORTD->PCR[14] = PORT_PCR_MUX(0x01); // pin 22
    PORTD->PCR[15] = PORT_PCR_MUX(0x01); // pin 21

    // SENSOR - PORT A, set pins for GPIO
    PORTA->PCR[24] = PORT_PCR_MUX(0x01); // Trig

    // LED - PORT D, set output mode
    PTD->PDDR = GPIO_PDDR_PDD(0xF300);
    PTA->PDDR = GPIO_PDDR_PDD(0x0FC0);

    // SENSOR - PORT A, set output and input mode
    PTA->PDDR |= GPIO_PDDR_PDD(Trig);
    PTA->PDOR &= ~Trig;

}

void setDigit(char digit) {
    int d = digit - '0';
    if (isdigit(digit) == 0) {
        empty();
        return;
    }
    empty();
    setSegments(d);
}

void PORTA_IRQHandler(void) {
    if (PORTA->ISFR & Echo) { // no input, interrupt detected
        if (!(GPIOA_PDIR & Echo)) {
            waitForEcho = 0;
            PORTA->ISFR = ~0;
        }
    }
}

 void PIT1_IRQHandler(void){
		PTA->PDOR &= ~Trig;
		trig = 0;
		echo = 0;
		PIT_TFLG1 |= 0x01;		// clear interrupt flag
		PIT_TCTRL1 &= ~0x1;	// disable timer
 }

 void PIT_enabler(void){
 	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;	// enable the PIT timer
 	PIT_TCTRL1 &= ~0x1;					// TEN disable
 	PIT->MCR &= ~0x2;					// disable standard timers
 	PIT_LDVAL1 = 0x1DF;	// 479 cycles = 10 us
 	PIT_TFLG1 =  0x1;					// clear interrupt flag
 	PIT_TCTRL1 |= 0x2;					// interrupt request
 	NVIC_EnableIRQ(PIT1_IRQn);			// enable interrupts from PIT1
    PIT_TCTRL1 |=  0x1;	// enable timer
 }

 void sensor(void) {
     // send 10us pulse to Trig
     PTA->PDOR |= Trig;
     trig = 1;
     PIT_enabler();
     while (trig);
     // PIT disables Trig signal after 10us
     // wait for Echo signal
     measurement = 0;
     waitForEcho = 1;
     while (waitForEcho) {
    	 echo = 1;
    	 PIT_enabler();
    	 while (echo);
    	 measurement += 10;

     }
     result = trunc(measurement/29/2);
     if (result < 0) result = 0;
     if (result > 9999) result = 9999;
     memset(resultS, 'x', 4);
     sprintf(resultS,"%lu", result);
 }

int main(void) {
    MCUInit();
    PortsInit();

    while (1) {
    	sensor();

    	setDigit(resultS[0]);
    	PTD->PDOR |= GPIO_PDOR_PDO(LED_C1);
    	delay(DELAY_TIME);
    	PTD->PDOR &= GPIO_PDOR_PDO(~LED_C1);

    	setDigit(resultS[1]);
    	PTD->PDOR |= GPIO_PDOR_PDO(LED_C2);
    	delay(DELAY_TIME);
    	PTD->PDOR &= GPIO_PDOR_PDO(~LED_C2);

    	setDigit(resultS[2]);
    	PTD->PDOR |= GPIO_PDOR_PDO(LED_C3);
    	delay(DELAY_TIME);
    	PTD->PDOR &= GPIO_PDOR_PDO(~LED_C3);

    	setDigit(resultS[3]);
    	PTD->PDOR |= GPIO_PDOR_PDO(LED_C4);
    	delay(DELAY_TIME);
    	PTD->PDOR &= GPIO_PDOR_PDO(~LED_C4);
    	delay(2000);
    }

    return 0;
}
