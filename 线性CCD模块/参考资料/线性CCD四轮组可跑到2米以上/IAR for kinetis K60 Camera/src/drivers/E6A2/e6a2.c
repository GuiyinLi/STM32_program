#include "E6A2.h"
/*
pit_init_ms
FTM_Input_init*/


void init_E6A2()
{
  extern short int countl;
  extern short int countr;
  countl=0;countr=0;
  exti_init(PORTA,10,rising_down);
  exti_init(PORTA,12,rising_down);
  pit_init_ms(PIT1,20);
}

/*
void E6A2_debug_PrinttoScreen()
{
  extern short int vlnow;
  extern short int vrnow;
  Screen_PrintInt(vlnow,10,50);
  Screen_PrintInt(vrnow,70,50);
}*/