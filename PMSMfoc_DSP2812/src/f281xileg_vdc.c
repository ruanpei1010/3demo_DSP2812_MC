/* ==================================================================================
File name:       F281XILEG_VDC.C
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: This file contains source for the F281X Two leg current measurement 
and DC-bus measurement driver.

Target: TMS320F281x family

=====================================================================================
History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20: Using DSP281x v. 1.00 or higher 
----------------------------------------------------------------------------------*/

#include "DSP281x_Device.h"
#include "f281xileg_vdc.h"

#define CPU_CLOCK_SPEED      6.6667L   // for a 150MHz CPU clock speed
#define ADC_usDELAY 5000L
#define DELAY_US(A)  DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_CLOCK_SPEED) - 9.0L) / 5.0L)

extern void DSP28x_usDelay(unsigned long Count);

void F281X_ileg2_dcbus_drv_init(ILEG2DCBUSMEAS *p)
{

    DELAY_US(ADC_usDELAY);						//延时5us	
  
    AdcRegs.ADCTRL1.all = ADC_RESET_FLAG; 		//复位ADC模块 
	asm(" NOP ");
	asm(" NOP ");    

    AdcRegs.ADCTRL3.bit.ADCBGRFDN = 0x3;		// Power up bandgap/reference circuitry
	DELAY_US(ADC_usDELAY);			    		// Delay before powering up rest of ADC 
    
    AdcRegs.ADCTRL3.bit.ADCPWDN = 1;	   		// Power up rest of ADC
    AdcRegs.ADCTRL3.bit.ADCCLKPS = 6;     		//ADCLK=12.5MHZ 
	DELAY_US(ADC_usDELAY);	

    AdcRegs.ADCTRL1.all |= ADCTRL1_INIT_STATE;	//仿真挂起被忽略；采集窗口为2个ADCLK周期数；AD时钟不再进行二分频了;级联模式 
    AdcRegs.ADCTRL2.all |= ADCTRL2_INIT_STATE; 	//允许EVA启动级联模式的SEQ
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1|= 0x0003; //用4个转换通道
    AdcRegs.ADCCHSELSEQ1.all = p->ChSelect;     //Ia:通道0；Ib:通道1；Ic：通道2；vsense:通道3

	EvaRegs.GPTCONA.bit.T1TOADC = 1;      		//用T1的下溢中断启动ADC
}  

void F281X_ileg2_dcbus_drv_read(ILEG2DCBUSMEAS *p)
{
       int16 DatQ15;
       int32 Tmp;

        // Wait until ADC conversion is completed
        while (AdcRegs.ADCST.bit.SEQ1_BSY == 1)
        {};
		
		//Ia,正数,电流流向电机,-1～+1代表－25A～25A
        DatQ15 = AdcRegs.ADCRESULT0^0x8000;       // ^按位异或,将转换结果转换为Q15格式的双极性数据
        Tmp = (int32)p->ImeasAGain*(int32)DatQ15; // Tmp = gain*dat => Q28 = Q13*Q15
        p->ImeasA = (int16)(Tmp>>13);             // Convert Q28 to Q15
        p->ImeasA += p->ImeasAOffset;             // Add offset
        p->ImeasA *= -1;                   	  // 正数,电流流向电机,-1～+1代表－25A～25A
		
		//Ib,正数,电流流向电机,-1～+1代表－25A～25A
        DatQ15 = AdcRegs.ADCRESULT1^0x8000;   	  // Convert raw result to Q15 (bipolar signal)
        Tmp = (int32)p->ImeasBGain*(int32)DatQ15; // Tmp = gain*dat => Q28 = Q13*Q15
        p->ImeasB = (int16)(Tmp>>13);             // Convert Q28 to Q15
        p->ImeasB += p->ImeasBOffset;             // Add offset
        p->ImeasB *= -1;                   		  // Positive direction, current flows to motor
 		
		//Ic,正数,电流流向电机,-1～+1代表－25A～25A,是计算出来的，不是由ADCRESULT3读出的
		p->ImeasC = -(p->ImeasA + p->ImeasB);      // Compute phase-c current
		
		//VSENSE,0代表0V,1代表5V
        DatQ15 = (AdcRegs.ADCRESULT3>>1)&0x7FFF;   // 转化为Q15单极性，范围为[0,1)
        Tmp = (int32)p->VdcMeasGain*(int32)DatQ15; // Tmp = gain*dat => Q28 = Q13*Q15
        if (Tmp > 0x0FFFFFFF)                      // Limit Tmp to 1.0 in Q28
           Tmp = 0x0FFFFFFF;
        p->VdcMeas = (int16)(Tmp>>13);             // Convert Q28 to Q15
        p->VdcMeas += p->VdcMeasOffset;            // Add offset

        

        AdcRegs.ADCTRL2.all |= 0x4040;             // Reset the sequence

}            
          
