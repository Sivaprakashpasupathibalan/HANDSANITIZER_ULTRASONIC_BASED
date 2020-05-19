/*
 * File:   main.c
 * Author: Sivaprakash Pasupathibalan
 *
 * Created on 19 May, 2020, 2:28 PM
 */


// 'C' source line config statements

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#define _XTAL_FREQ 4000000
#define AN0 1<<0     //Distance set pot
#define AN1 1<<1     //motor time pot
#define AN2 1<<2
#define AN3 1<<3
#define MOT_PIN GP5
#define US_TRIG GP2
#define US_ECHO GP3

void StartTimer1(void);
unsigned int StopTimer1(void);
void InitADC(unsigned char channel);
unsigned int GetADCValue(unsigned char channel);
void main() {
    unsigned int US_POT  = 0;
    unsigned int MOT_POT = 0;
    unsigned int time = 0;
    GPIO = 0x00;
    MOT_PIN = 0;
    US_ECHO = 0;
    while(1)
    {
      InitADC(AN0);
      US_POT = GetADCValue(AN0);
      InitADC(AN1);
      MOT_POT = GetADCValue(AN1);
      US_TRIG = 0;
      US_TRIG = 1;
      __delay_us(10);
      US_TRIG = 0;
      while(!US_ECHO)
      {
         StartTimer1(); 
      }
      while(US_ECHO)
      {
          time = StopTimer1();
      }
      if((US_POT * 63) > time)
      {
         MOT_PIN = 1;
          for(int i = 0 ; i < (MOT_POT / 4); i++)
          {
              __delay_ms(1);
          }
         MOT_PIN = 0;
         for(int i = 0 ; i < 2000; i++)
          {
             __delay_ms(1);
          }
      }
      else
      {
        //  MOT_PIN = 0;
      }
      __delay_ms(10);
    }
}
unsigned int GetADCValue(unsigned char channel)
{
    ADCON0 &= 0xF3;
    switch(channel)
    {
        case AN0: ADCON0 |= 0x00; break;
        case AN1: ADCON0 |= 0x04; break;
        case AN2: ADCON0 |= 0x08; break;
        case AN3: ADCON0 |= 0x0C; break;
        
        default: return 0;
    }
    __delay_ms(10);
    
    GO_nDONE = 1;
    
    while(GO_nDONE);
    
    return ((ADRESH << 8) + ADRESL);
}
void InitADC(unsigned char channel)
{
    ANSEL = 0x10;
    ANSEL |= channel;
    TRISIO = 0x10;
    TRISIO |= channel;
    ADCON0 = 0x81;
    CMCON = 0x07;
    VRCON = 0x00;
}
void StartTimer1(void)
{
    TMR1H = 0;
    TMR1L = 0;
    T1CON = 0x01;
}
unsigned int StopTimer1()
{
    T1CON = 0x00;
    return ((TMR1H << 8) + TMR1L);    
}
/*
void interrupt ISR(void)
{
    if(T1IF)
    {
        GP0 = ~GP0;
        T1IF = 0;
    }
}
*/
