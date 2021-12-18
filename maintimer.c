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
#define DELAY_TIME 2000

// SENSOR pins
#define Trig 0x1000000
#define Echo 0x2000000

unsigned int compare = 0x200;
int measurement = 0;
int waitForEcho = 1;
long int result = 0;
char resultS[4];

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

    SIM->COPC = SIM_COPC_COPT(0x00); // Deaktivace modulu WatchDog
    // Aktivace hodin ridicich modulu portu A a B
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(0x01); // zdroj hodin do casovace TPM
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; // povoleni hodin do portu B (pro RGB LED)
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK; // povoleni hodin do casovace TPM0
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

void sensor(void) {
    // send 10us pulse to Trig
    PTA->PDOR |= Trig;
    delay(30);
    PTA->PDOR &= ~Trig;

    // wait for Echo signal
    measurement = 0;
    waitForEcho = 1;
    while (waitForEcho) {
    	measurement += 1;
    }
    result = (measurement * 340) / 100 / 2;
    if (result < 0) result = 0;
    if (result > 9999) result = 9999;
    memset(resultS, 'x', 4);
    sprintf(resultS,"%lu", result);
}

void PORTA_IRQHandler(void) {
    if (PORTA->ISFR & Echo) { // no input, interrupt detected
        if (!(GPIOA_PDIR & Echo)) {
            waitForEcho = 0;
            PORTA->ISFR = ~0;
        }
    }
}

 void LPTMR0_IRQHandler(void)
 {
     // Set new compare value set by up/down buttons
     LPTMR0_CMR = compare/100;                // !! the CMR reg. may only be changed while TCF == 1
     LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK;   // writing 1 to TCF tclear the flag
     setDigit(resultS[0]);
         	
 }

 void LPTMR0Init(int count)
 {
     SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK; // Enable clock to LPTMR
     LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;   // Turn OFF LPTMR to perform setup
     LPTMR0_PSR = ( LPTMR_PSR_PRESCALE(0) // 0000 is div 2
                  | LPTMR_PSR_PBYP_MASK   // LPO feeds directly to LPT
                  | LPTMR_PSR_PCS(1)) ;   // use the choice of clock
     LPTMR0_CMR = count;                  // Set compare value
     LPTMR0_CSR =(  LPTMR_CSR_TCF_MASK    // Clear any pending interrupt (now)
                  | LPTMR_CSR_TIE_MASK    // LPT interrupt enabled
                 );
     // enable interrupts
     NVIC_EnableIRQ(LPTMR0_IRQn);

     LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;    // Turn ON LPTMR0 and start counting
 }

 void Timer0Init(void)
 {
 	TPM0_CNT = 0x0;
 	TPM0_MOD = 0x78;
 	TPM0_CnSC(0) = 0x28;
 	TPM0_SC = TPM_SC_CMOD(0);
 	TPM0_SC = (TPM_SC_CPWMS(0) | TPM_SC_PS(2) | TPM_SC_DMA(0) | TPM_SC_TOIE(0));
 	TPM0_SC = TPM_SC_CMOD(1);
     // Dalsi ukoly k doplneni jsou uvedeny v hlavni smycce.
 }


int main(void) {
    MCUInit();
    PortsInit();
    LPTMR0Init(compare);
    Timer0Init();

    while (1) {
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
    }

    return 0;
}
