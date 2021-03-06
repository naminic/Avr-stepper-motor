/*****************************************************
Chip type               : ATmega32
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*****************************************************/

#include <mega32.h>
#include <delay.h>
#include <stdio.h>

int j,k=493,inc;
long i;
const unsigned char mstt1[4]="ON";  
const unsigned char mstt2[4]="OFF";
const unsigned char mstt3[16]="Terminal A TO B";
const unsigned char mstt4[16]="Terminal B TO A";
unsigned char state,rot,*m_p,*a_p;
bit stsp,reverse_aim;


void init_variable(void)
{
    PORTC=0x00;
    DDRC=0x3F;
    TCCR0=0x02;
    TCNT0=0x00;
    OCR0=0x00;
    TIMSK=0x01;
    UCSRA=0x00;
    UCSRB=0x18;
    UCSRC=0x86;
    UBRRH=0x00;
    UBRRL=0x33;
    m_p=&mstt1[0];
    a_p=&mstt3[0];
    rot=0x01;
}

void show_help(void)
{
   printf("\n\nKeys to Control MOTOR(HELP):\n");
   printf("\t-'1' start/stop MOTOR.\n");
   printf("\t-'2' MOTOR Aim(Left round/Right round).\n");
   printf("\t-'3' Increase MOTOR Speed.\n");
   printf("\t-'4' Decrease MOTOR Speed.\n");
   printf("\t-'5' MOTOR Information.\n");
   printf("\t-'6' Show HELP.\n");
   printf("\t-'7' Exit.\n\n\r");
}

void start_stop(void)
{
    if(!stsp)
    {
       PORTC |=0x30;
    }
    else
    {
       PORTC &=0xCF;
    }
    stsp=~stsp;
}

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    i++;
    PORTC =((PORTC & 0xF0) | rot);
    if(i>k && stsp)
    {
       if(!reverse_aim)
       {
           if(rot<0x08)
           {
              rot=rot<<1;
           }
           else
           {
             rot=0x01;
           }
       }
       else
       {
           if(rot>0x01)
           {
              rot=rot>>1;
           }
           else
           {
             rot=0x08;
           }
       }
       i=0;
    }
}

void i_speed(void)
{
   if(k==493)
   {
      inc=35;
   }
   if(k>5)
   {
     k -=inc;
   }
}

void d_speed(void)
{
   if(k==493)
   {
      inc=200;
   }
   if(k<2300)
   {
     k +=inc;
   }
}

void motor_inf(void)
{
   printf("\n\n\r---Last MOTOR Statuse:");
   if(stsp)
   {
      m_p=&mstt1[0];
   }
   else
   {
     m_p=&mstt2[0];
   }
   if(!reverse_aim)
   {
      a_p=&mstt3[0];
   }
   else
   {
     a_p=&mstt4[0];
   }
   printf("\n\r\t Power: %s --- Aim: %s --- Speed: %d (Pulse/s)\n\n\n\r",m_p,a_p,(int)(3920/k));
}

void main(void)
{

init_variable();

// Global enable interrupts
#asm("sei")

printf("WWW.NAMINIC.COM\n\r");
printf("STEPER MOTOR TEST.\r");

show_help();

while (1)
      {
         printf("\n\rpress any key:");
         gets(&state,1);
         delay_ms(400);
         
         if(state>='1' && state<='7')
         {
            switch(state)
            {
                case '1':
                    start_stop();
                    break;
                case '2':
                    reverse_aim=~reverse_aim;
                    for(j=0;j<1000;j++);
                    break;
                case '3':
                    i_speed();
                    break;
                case '4':
                    d_speed();
                    break;
                case '5':
                    motor_inf();
                    break;    
                case '6':
                    show_help();
                    break;
                case '7':
                    break;
                default:
                
            }
            if(state!='5')
            {
                 motor_inf();
            }
         }
         else
         {
            printf("\n\r--------This key is incorrect. Press '6' to see HELP");
         }
         if(state=='7')
         {
            PORTC &=0xC0;
            printf("\n\r--------The program ENDED.");
            TIMSK &=0xFE;
            TCCR0=0x00;
            while(1);
         } 
      }
}
