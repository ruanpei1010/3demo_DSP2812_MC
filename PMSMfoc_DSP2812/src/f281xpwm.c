/* ==================================================================================
File name:       F281XPWM.C
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description:   This file contains source for the Full Compare PWM  drivers for the F281x

Target: TMS320F281x family
     
=====================================================================================
History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20: Using DSP281x v. 1.00 or higher 
----------------------------------------------------------------------------------*/

#include "DSP281x_Device.h"
#include "f281xpwm.h"

void F281X_EV1_PWM_Init(PWMGEN *p) 
{       
        EvaRegs.T1PR = p->PeriodMax;              // Init Timer 1 period Register 
        EvaRegs.T1CON.all = PWM_INIT_STATE;       // Symmetrical Operation 
        EvaRegs.DBTCONA.all = DBTCON_INIT_STATE;  // Init DBTCONA Register                                   
        EvaRegs.ACTRA.all = ACTR_INIT_STATE;      // Init ACTRA Register         

        EvaRegs.COMCONA.all = 0xA200;             //当T1下溢或周期匹配时，CMPRx重载

        EvaRegs.CMPR1 = p->PeriodMax;             // Init CMPR1 Register 
        EvaRegs.CMPR2 = p->PeriodMax;             // Init CMPR2 Register 
        EvaRegs.CMPR3 = p->PeriodMax;             // Init CMPR3 Register 
        EALLOW;                       // Enable EALLOW 
        GpioMuxRegs.GPAMUX.all |= 0x003F;   // Setting PWM1-6 as primary output pins
        EDIS;                         // Disable EALLOW
}


void F281X_EV1_PWM_Update(PWMGEN *p) 
{       
		int16 MPeriod;
        int32 Tmp;

// Compute the timer period (Q0) from the period modulation input (Q15)
        Tmp = (int32)p->PeriodMax*(int32)p->MfuncPeriod;           // Q15 = Q0*Q15
        MPeriod = (int16)(Tmp>>16) + (int16)(p->PeriodMax>>1);     // Q0 = (Q15->Q0)/2 + (Q0/2)
        EvaRegs.T1PR = MPeriod;  

// Compute the compare 1 (Q0) from the PWM 1&2 duty cycle ratio (Q15)
        Tmp = (int32)MPeriod*(int32)p->MfuncC1;                    // Q15 = Q0*Q15
        EvaRegs.CMPR1 = (int16)(Tmp>>16) + (int16)(MPeriod>>1);    // Q0 = (Q15->Q0)/2 + (Q0/2)

// Compute the compare 2 (Q0) from the PWM 3&4 duty cycle ratio (Q15)
        Tmp = (int32)MPeriod*(int32)p->MfuncC2;                   // Q15 = Q0*Q15
        EvaRegs.CMPR2 = (int16)(Tmp>>16) + (int16)(MPeriod>>1);   // Q0 = (Q15->Q0)/2 + (Q0/2)

// Compute the compare 3 (Q0) from the PWM 5&6 duty cycle ratio (Q15)
        Tmp = (int32)MPeriod*(int32)p->MfuncC3;                   // Q15 = Q0*Q15
        EvaRegs.CMPR3 = (int16)(Tmp>>16) + (int16)(MPeriod>>1);   // Q0 = (Q15->Q0)/2 + (Q0/2)
        
}

