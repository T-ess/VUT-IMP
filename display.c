#include "display.h"

void set0() {
    PTA->PDOR |= GPIO_PDOR_PDO(LED_A);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_B);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_C);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_D);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_E);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_F);
};

void set1() {
    PTA->PDOR |= GPIO_PDOR_PDO(LED_B);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_C);
};

void set2() {
    PTA->PDOR |= GPIO_PDOR_PDO(LED_A);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_B);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_D);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_E);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_G);
};

void set3() {
    PTA->PDOR |= GPIO_PDOR_PDO(LED_A);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_B);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_C);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_D);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_G);

};

void set4() {
    PTA->PDOR |= GPIO_PDOR_PDO(LED_B);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_C);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_F);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_G);
};

void set5() {
    PTA->PDOR |= GPIO_PDOR_PDO(LED_A);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_C);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_D);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_F);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_G);
};

void set6() {
    PTA->PDOR |= GPIO_PDOR_PDO(LED_A);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_C);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_D);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_E);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_F);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_G);
};

void set7() {
    PTA->PDOR |= GPIO_PDOR_PDO(LED_A);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_B);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_C);
};

void set8() {
    PTA->PDOR |= GPIO_PDOR_PDO(LED_A);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_B);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_C);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_D);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_E);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_F);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_G);
};

void set9() {
    PTA->PDOR |= GPIO_PDOR_PDO(LED_A);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_B);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_C);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_D);
    PTA->PDOR |= GPIO_PDOR_PDO(LED_F);
    PTD->PDOR |= GPIO_PDOR_PDO(LED_G);
};

void empty() {
    PTA->PDOR &= GPIO_PDOR_PDO(~LED_A);
    PTA->PDOR &= GPIO_PDOR_PDO(~LED_B);
    PTD->PDOR &= GPIO_PDOR_PDO(~LED_C);
    PTA->PDOR &= GPIO_PDOR_PDO(~LED_D);
    PTA->PDOR &= GPIO_PDOR_PDO(~LED_E);
    PTA->PDOR &= GPIO_PDOR_PDO(~LED_F);
    PTD->PDOR &= GPIO_PDOR_PDO(~LED_G);
};

void setSegments(int num) {
    switch (num)
    {
    case 0:
        set0();
        break;
    case 1:
        set1();
        break;
    case 2:
        set2();
        break;
    case 3:
        set3();
        break;
    case 4:
        set4();
        break;
    case 5:
        set5();
        break;
    case 6:
        set6();
        break;
    case 7:
        set7();
        break;
    case 8:
        set8();
        break;
    case 9:
        set9();
        break;
    
    default:
        break;
    }
};