/**
 * @file main.c
 * @author Tereza Burianova (xburia28@stud.fit.vutbr.cz)
 * @brief
 * @date 2021-12-14
 */

#include "display.h"
#include <string.h>
#include <stdio.h>
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

// unsigned int compare = 0x200;

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

    /* Set corresponding PTA pins for GPIO functionality */
    PORTA->PCR[6] = PORT_PCR_MUX(0x01); // pin 25
    PORTA->PCR[7] = PORT_PCR_MUX(0x01); // pin 27
    PORTA->PCR[8] = PORT_PCR_MUX(0x01); // pin 23
    PORTA->PCR[9] = PORT_PCR_MUX(0x01); // pin 28
    PORTA->PCR[10] = PORT_PCR_MUX(0x01); // pin 24
    PORTA->PCR[11] = PORT_PCR_MUX(0x01); // pin 26

    /* Set corresponding PTD pins for GPIO functionality */
    PORTD->PCR[8] = PORT_PCR_MUX(0x01); // pin 17
    PORTD->PCR[9] = PORT_PCR_MUX(0x01); // pin 18
    PORTD->PCR[12] = PORT_PCR_MUX(0x01); // pin 19
    PORTD->PCR[13] = PORT_PCR_MUX(0x01); // pin 20
    PORTD->PCR[14] = PORT_PCR_MUX(0x01); // pin 22
    PORTD->PCR[15] = PORT_PCR_MUX(0x01); // pin 21

    /* Change corresponding PTB port pins as outputs */
    PTD->PDDR = GPIO_PDDR_PDD(0xF300); // LED digits as outputs
    PTA->PDDR = GPIO_PDDR_PDD(0x0FC0); // LED digits as outputs
}

//** void LPTMR0_IRQHandler(void)
// {
//     // Set new compare value set by up/down buttons
//     LPTMR0_CMR = compare;                // !! the CMR reg. may only be changed while TCF == 1
//     LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK;   // writing 1 to TCF tclear the flag
//     GPIOB_PDOR ^= LED_D9;                // invert D9 state
//     GPIOB_PDOR ^= LED_D10;               // invert D10 state
//     GPIOB_PDOR ^= LED_D11;               // invert D11 state
//     GPIOB_PDOR ^= LED_D12;               // invert D12 state
//     beep_flag = !beep_flag;              // see beep_flag test in main()
// }

//** void LPTMR0Init(int count)
// {
//     SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK; // Enable clock to LPTMR
//     LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;   // Turn OFF LPTMR to perform setup
//     LPTMR0_PSR = ( LPTMR_PSR_PRESCALE(0) // 0000 is div 2
//                  | LPTMR_PSR_PBYP_MASK   // LPO feeds directly to LPT
//                  | LPTMR_PSR_PCS(1)) ;   // use the choice of clock
//     LPTMR0_CMR = count;                  // Set compare value
//     LPTMR0_CSR =(  LPTMR_CSR_TCF_MASK    // Clear any pending interrupt (now)
//                  | LPTMR_CSR_TIE_MASK    // LPT interrupt enabled
//                 );
//     NVIC_EnableIRQ(LPTMR0_IRQn);         // enable interrupts from LPTMR0
//     LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;    // Turn ON LPTMR0 and start counting
// }

void setDigit(char digit) {
    int d = digit - '0';
    if (isdigit(digit) == 0) {
        empty();
        return;
    }
    empty();
    setSegments(d);
}

int main(void) {
    MCUInit();
    PortsInit();
    // LPTMR0Init(compare);

    while (1) {
        long int result = 0;
        if (result < 0) result = 0;
        if (result > 9999) result = 9999;
        char resultS[4];
        memset(resultS, 'x', 4);
        sprintf(resultS,"%lu", result);

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

    }

    return 0;
}
