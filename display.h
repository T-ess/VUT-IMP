#include "MK60D10.h"

// LED segments
#define LED_A 0x0800
#define LED_B 0x0200
#define LED_C 0x4000
#define LED_D 0x0400
#define LED_E 0x0040
#define LED_F 0x0080
#define LED_G 0x8000
#define LED_DP 0x0100

void set0();
void set1();
void set2();
void set3();
void set4();
void set5();
void set6();
void set7();
void set8();
void set9();
void empty();
void setSegments(int num);