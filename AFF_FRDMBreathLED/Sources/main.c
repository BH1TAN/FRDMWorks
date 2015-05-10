/*
 * main implementation: use this 'C' sample to create your own application
 *
 */





#include "derivative.h" /* include peripheral declarations */

unsigned short ratiox,ratioy,SysTickLoad,SysTickFlag;
unsigned short direction=0;//Ϊ0ʱ��������
unsigned short TempNum;


void ESysTick(unsigned short x) //xΪ 1/64�� �ı���
{
	SysTickLoad=x-1;
	SysTickFlag=x-1;
	SYST_RVR=1500000; //1/64 @ 96MHz
	SYST_CVR=0x00;     //clear
	SYST_CSR=0x07;     //Enable clock
}

void DSysTick(void)
{
	SYST_CSR&=~0x01;
}

void Sys_Init(void)
{
	asm("CPSIE i");
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM_SCGC6 |= SIM_SCGC6_TPM2_MASK;
	
	//PORTB_PCR18 = PORT_PCR_MUX(3);  //��ΪTPM
	PORTB_PCR19 = PORT_PCR_MUX(3);  //��ΪTPM
	
	
}

void TSI_Init(void)
{
	SIM_SCGC5 |= SIM_SCGC5_TSI_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	PORTB_PCR16 |= PORT_PCR_MUX(0);
	PORTB_PCR17 |= PORT_PCR_MUX(0);
	PORTB_PCR16 |= PORT_PCR_IRQC(0b1001);
	
	TSI0_GENCS  = 0;
	TSI0_GENCS |= (TSI_GENCS_ESOR_MASK            // ѡ��ɨ������ж�             
	           | TSI_GENCS_MODE(0)               // ����ΪTSIģʽ                
	           | TSI_GENCS_REFCHRG(4)            // ���òο�����������8uA    
	           | TSI_GENCS_DVOLT(0)              // ����������ѹ��             
	           | TSI_GENCS_EXTCHRG(7)            // ���õ缫����������64uA   
	           | TSI_GENCS_PS(4)                 // ���õ缫�������Ԥ��Ƶֵ/16
	           | TSI_GENCS_NSCN(20)              // ����ÿ���缫ɨ���21�����31 
	           | TSI_GENCS_TSIIEN_MASK           // ʹ��TSI�����ж�ʹ��          
	           | TSI_GENCS_STPE_MASK             // �͹���ģʽ��ʹ��TSIģ��
	           );
	 TSI0_GENCS |= TSI_GENCS_TSIEN_MASK;          // ʹ��TSI����ģ��
	 TSI0_DATA |= TSI_DATA_TSICH(9);
}

void TPM2_Init(void)
{
	SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1); 
	SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; //ѡ�� MCGPLLCLK clock/2
	TPM2_CNT = 0; 
	TPM2_MOD = 0x00; 
	TPM2_SC |= (0x08|0x07);  // ����PWMʱ�� 
	TPM2_C0SC = (0x0020 | 0x0008);  //����PEMģʽ 
	TPM2_C0V = 0x00; 
	TPM2_C1SC = (0x0020 | 0x0008);  //����PEMģʽ 
	TPM2_C1V = 0x00; 
}

void TPM2_Set (void)
{ 
	TPM2_C0V = ratiox; //ռ�ձ�
	TPM2_C1V = ratioy;
}


void Clk_Init(void)
{
	MCG_C2=0X1C;
	MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(4);
	//wait for settings
	while (!(MCG_S & MCG_S_OSCINIT0_MASK));    //�ȴ����໷��ʼ������
	while (MCG_S & MCG_S_IREFST_MASK);    //�ȴ�ʱ���л����ⲿ�ο�ʱ��
	while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT)!= 0x2);   //�ȴ�MCGOUTCLK�л����ⲿʱ�����
	//���ϴ������MCGOUTCLK 8MHz�����ã�ʹ���ⲿ8M����ֱ���ṩ����FBE״̬��
	MCG_C5 = MCG_C5_PRDIV0(1);//��Ƶ��2~4MHz֮�䣬��Ƶ��Ƶ��4MHz               
	MCG_C6 = MCG_C6_PLLS_MASK |MCG_C6_VDIV0(0);  //ѡ��PLL����Ƶ24��

	SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV4(2);//flashΪMCGOUTCLK/3
	
	while (!(MCG_S &MCG_S_PLLST_MASK)){}; // �ȴ��л���PLL 
	while (!(MCG_S &MCG_S_LOCK0_MASK)){}; //�ȴ�PLL����Ƶ��
	//���ϴ���ʵ�ֵ�PBE
	
	MCG_C1 &= ~MCG_C1_CLKS_MASK;//CLKS=00��PLL���
	while (((MCG_S & MCG_S_CLKST_MASK)>> MCG_S_CLKST_SHIFT) != 0x3){};//�ȴ��л���ϡ�
	//���ϴ���ʵ�ֵ�PEE
	
	//FLLʧ�ܣ�PLLΪ  48MHz
}

void SysTick_Handler(void)
{
	if(SysTickFlag==0)
	{
		
		//�ڴ�����Ϊ�жϷ�����
		/******************************/
		if(!direction)
		{
			ratiox=ratiox+10;
			ratioy=ratiox;
			if(ratiox>510)
			{
				
				direction=1;
				ratiox=510;
				ratioy=ratiox;
			}
			
		}
		else
		{
			ratiox=ratiox-10;
			ratioy=ratiox;
			if(ratiox<10)
			{
				direction=0;
				ratiox=10;
				ratioy=ratiox;
			}
		}
		TPM2_Set();
		/*****************************/
		//�ڴ�����Ϊ�жϷ�����
		
		SysTickFlag=SysTickLoad;
	}
	else if(SysTickFlag>0)
		SysTickFlag--;
	else ;
}

void TSI0_IRQHandler(void)
{
	TempNum++;
	if(TempNum>11)
		TempNum=11;
	TempNum = (TSI0_DATA & 0xffff);
	TSI0_GENCS &=~ TSI_GENCS_EOSF_MASK;//���־λ
}

int main(void)
{
	Sys_Init();
	Clk_Init();
	TSI_Init();
	TPM2_Init();
	ratiox=20;
	ratioy=ratiox;
	TPM2_MOD = 500;
	TPM2_Set();
	TempNum=0;
	ESysTick(2);
	unsigned short testnum,testnum2;
	while(1)
	{
		TSI0_GENCS |= TSI_GENCS_TSIEN_MASK;
		TSI0_DATA |= TSI_DATA_SWTS_MASK;//����ɨ��
		while(!(TSI_GENCS_EOSF_MASK & TSI0_GENCS));
		testnum = (TSI0_DATA & 0xffff);
		testnum2 = TempNum;
	}
}
