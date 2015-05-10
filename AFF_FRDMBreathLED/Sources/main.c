/*
 * main implementation: use this 'C' sample to create your own application
 *
 */





#include "derivative.h" /* include peripheral declarations */

unsigned short ratiox,ratioy,SysTickLoad,SysTickFlag;
unsigned short direction=0;//为0时亮度增大
unsigned short TempNum;


void ESysTick(unsigned short x) //x为 1/64秒 的倍数
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
	
	//PORTB_PCR18 = PORT_PCR_MUX(3);  //设为TPM
	PORTB_PCR19 = PORT_PCR_MUX(3);  //设为TPM
	
	
}

void TSI_Init(void)
{
	SIM_SCGC5 |= SIM_SCGC5_TSI_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	PORTB_PCR16 |= PORT_PCR_MUX(0);
	PORTB_PCR17 |= PORT_PCR_MUX(0);
	PORTB_PCR16 |= PORT_PCR_IRQC(0b1001);
	
	TSI0_GENCS  = 0;
	TSI0_GENCS |= (TSI_GENCS_ESOR_MASK            // 选择扫描结束中断             
	           | TSI_GENCS_MODE(0)               // 设置为TSI模式                
	           | TSI_GENCS_REFCHRG(4)            // 设置参考振荡器充电电流8uA    
	           | TSI_GENCS_DVOLT(0)              // 设置振荡器电压轨             
	           | TSI_GENCS_EXTCHRG(7)            // 设置电极振荡器充电电流64uA   
	           | TSI_GENCS_PS(4)                 // 设置电极振荡器输出预分频值/16
	           | TSI_GENCS_NSCN(20)              // 设置每个电极扫描次21，最大31 
	           | TSI_GENCS_TSIIEN_MASK           // 使能TSI输入中断使能          
	           | TSI_GENCS_STPE_MASK             // 低功耗模式下使能TSI模块
	           );
	 TSI0_GENCS |= TSI_GENCS_TSIEN_MASK;          // 使能TSI输入模块
	 TSI0_DATA |= TSI_DATA_TSICH(9);
}

void TPM2_Init(void)
{
	SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1); 
	SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; //选择 MCGPLLCLK clock/2
	TPM2_CNT = 0; 
	TPM2_MOD = 0x00; 
	TPM2_SC |= (0x08|0x07);  // 设置PWM时钟 
	TPM2_C0SC = (0x0020 | 0x0008);  //设置PEM模式 
	TPM2_C0V = 0x00; 
	TPM2_C1SC = (0x0020 | 0x0008);  //设置PEM模式 
	TPM2_C1V = 0x00; 
}

void TPM2_Set (void)
{ 
	TPM2_C0V = ratiox; //占空比
	TPM2_C1V = ratioy;
}


void Clk_Init(void)
{
	MCG_C2=0X1C;
	MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(4);
	//wait for settings
	while (!(MCG_S & MCG_S_OSCINIT0_MASK));    //等待锁相环初始化结束
	while (MCG_S & MCG_S_IREFST_MASK);    //等待时钟切换到外部参考时钟
	while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT)!= 0x2);   //等待MCGOUTCLK切换到外部时钟输出
	//以上代码完成MCGOUTCLK 8MHz的设置，使用外部8M晶振直接提供。（FBE状态）
	MCG_C5 = MCG_C5_PRDIV0(1);//分频在2~4MHz之间，分频后频率4MHz               
	MCG_C6 = MCG_C6_PLLS_MASK |MCG_C6_VDIV0(0);  //选择PLL，倍频24倍

	SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV4(2);//flash为MCGOUTCLK/3
	
	while (!(MCG_S &MCG_S_PLLST_MASK)){}; // 等待切换到PLL 
	while (!(MCG_S &MCG_S_LOCK0_MASK)){}; //等待PLL锁定频率
	//以上代码实现到PBE
	
	MCG_C1 &= ~MCG_C1_CLKS_MASK;//CLKS=00，PLL输出
	while (((MCG_S & MCG_S_CLKST_MASK)>> MCG_S_CLKST_SHIFT) != 0x3){};//等待切换完毕。
	//以上代码实现到PEE
	
	//FLL失能，PLL为  48MHz
}

void SysTick_Handler(void)
{
	if(SysTickFlag==0)
	{
		
		//在此线下为中断服务函数
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
		//在此线上为中断服务函数
		
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
	TSI0_GENCS &=~ TSI_GENCS_EOSF_MASK;//清标志位
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
		TSI0_DATA |= TSI_DATA_SWTS_MASK;//启动扫描
		while(!(TSI_GENCS_EOSF_MASK & TSI0_GENCS));
		testnum = (TSI0_DATA & 0xffff);
		testnum2 = TempNum;
	}
}
