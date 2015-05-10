/*
 * main implementation: use this 'C' sample to create your own application
 *
 */





#include "derivative.h" /* include peripheral declarations */
#include "MusicMod.h"
#include "Music.h"

unsigned short pNote=0,nDuration=0;
unsigned short ratio=2;

void ESysTick(void)
{
	SYST_RVR=1000000;   //best 750000
	SYST_CVR=0x00;     //clear
	SYST_CSR=0x07;     //Enable clock
}

void DSysTick(void)
{
	SYST_CSR&=~0x01;
}

void TSI_Init(void)
{
	
}

void TPM1_Init(void)
{
	SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1); 
	SIM_SOPT2 &=~ SIM_SOPT2_PLLFLLSEL_MASK; //选择 MCGPLLCLK clock/2
	SIM_SCGC6 |=  SIM_SCGC6_TPM1_MASK; //PORT Init 
	TPM1_CNT = 0; 
	TPM1_MOD = 0x00; 
	TPM1_SC = (0x0008 | 0x0007);  // 设置PWM时钟 
	TPM1_C1SC = (0x0020 | 0x0008);  //设置PEM模式 
	TPM1_C1V = 0x00; 
}

void TPM1_Set (unsigned short period)
{ 
	TPM1_MOD = period; 
	TPM1_C1V = period/ratio; //占空比，可调节音量 
}

void SysTick_Handler(void)
{
	if(!nDuration)
	{
		if(note[pNote]==0xff)
		{
			pNote=0;
			TPM1_Set(0);
			nDuration=1; //保证歌曲结束后nDuration=0
		}
		else
		{
			TPM1_Set((unsigned short)(note[pNote]));
			nDuration=duration[pNote];
			pNote++;
		}
	}
	nDuration--;
}

int main(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM_SCGC5 |= SIM_SCGC5_TSI_MASK;
	PORTB_PCR1 = PORT_PCR_MUX(3);//TPM1_CH0
	
	PORTB_PCR16 = PORT_PCR_MUX(0);
	PORTB_PCR17 = PORT_PCR_MUX(0);
	
	TPM1_Init();
	asm("CPSIE i"); //中断总开关
	ESysTick();
	while(1)
		;
}
