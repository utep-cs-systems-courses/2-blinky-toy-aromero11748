#include <msp430.h>
#include "libTimer.h"
#include "buzzer.h"
#include <stdio.h>

#define LED_RED BIT0
#define LED_GREEN BIT6
#define LEDS (BIT0 | BIT6)
#define SW1 BIT0
#define SW2 BIT1
#define SW3 BIT2
#define SW4 BIT3
#define SWITCHES (SW1 | SW2 | SW3 | SW4)


static char
switch_update_interrupt_sense()
{
  char p2val = P2IN;
  P2IES |= (p2val & SWITCHES);
  P2IES &= (p2val | ~SWITCHES);
  return p2val;
}

void
switch_init(){
  P2REN |= SWITCHES;// enable res
  P2IE |= SWITCHES; // enable interupts
  P2OUT |= SWITCHES; //pull up res for switches
  P2DIR &= ~SWITCHES; // sets up for input
  switch_update_interrupt_sense();
}

int buzzerVol = 0;

int secTimer = 0;

void
__interrupt_vec(WDT_VECTOR) WDT()
{
  secTimer ++;        //causes the blinks to occur
  if(secTimer > 250){
    secTimer = 0;
    P1OUT ^= LED_RED;
    P1OUT ^= LED_GREEN;
    buzzer_set_period(buzzerVol);	/* makes buzzer turn on */
  }
 
  

}

int switchNum = 0;

void
switch_interrupt_handler()
{
  char p2val = switch_update_interrupt_sense();   // when switches are press
  switchNum = ~p2val & SWITCHES;

}
  

  void
__interrupt_vec(PORT2_VECTOR) Port_2(){
    
     
  if(P2IFG & SW1){
    P2IFG &= ~SW1;
    P1OUT ^= LED_RED; // does the xor of red
    buzzerVol = 0;
  }

  if(P2IFG & SW2){
    P2IFG &= ~SW2;
    P1OUT &= ~LED_RED; /// turns off ref makes green stay
    P1OUT |= LED_GREEN;
    buzzerVol = 5000;
    
  }
  if(P2IFG & SW3){
    P2IFG &= ~SW3;
    P1OUT |= LED_RED;
    P1OUT |= LED_GREEN;
    buzzerVol = 5000;	/* makes buzzer turn on */
  }
  
  if(P2IFG & SW4){
    P2IFG &= ~SW4;
    buzzerVol = 0;
   }
    
    
}


void wdt_c_handler(){
  static int sec = 0;
}



int main() {
    P1DIR |= LEDS;
    P1OUT &= ~LEDS;
    switch_init();
    // configureClocks();
    // switch_update_interrupt_sense();
    buzzer_init();
    enableWDTInterrupts();
    buzzer_set_period(6000);	/* makes buzzer turn on */
    
    or_sr(0x18);          // CPU off, GIE on
}
