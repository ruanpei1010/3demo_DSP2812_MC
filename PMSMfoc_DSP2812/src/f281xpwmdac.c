/* ==================================================================================
File name:       F281XPWMDAC.C
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description:   This file contains source for the Full Compare PWMDAC drivers for the F281x

Target: TMS320F281x family
           
=====================================================================================
History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20: Using DSP281x v. 1.00 or higher 
-------------------------------------------------------------------------------------*/

#include "DSP281x_Device.h"
#include "f281xpwmdac.h"

void F281X_EV2_PWMDAC_Init(PWMDAC *p) 
{       

        EvbRegs.T3PR = p->PeriodMax;               // Init Timer 3 period Register
        EvbRegs.T3CON.all = PWMDAC_INIT_STATE;     // Symmetrical Operation 
        EvbRegs.DBTCONB.all = DBTCONB_INIT_STATE;  // Init DBTCONB Register                                  
        EvbRegs.ACTRB.all = ACTRB_INIT_STATE;      // Init ACTRB Register          
        EvbRegs.COMCONB.all = 0x8200;              // 仅当T1下溢时，CMPRx重载
        EALLOW;                           // Enable EALLOW 
        GpioMuxRegs.GPBMUX.all |= 0x003F; // Set up the full compare PWM 7-12 pins to primary functions.
        EDIS;                             // Disable EALLOW 
}

void F281X_EV2_PWMDAC_Update(PWMDAC *p) 
{       
        int32 Tmp;

        EvbRegs.T3PR = p->PeriodMax;        // Update Timer 3 period Register

// Compute the compare 4 (Q0) from the PWM 7&8 duty cycle ratio (Q15)
        Tmp = (int32)p->PeriodMax*(int32)(*p->PwmDacInPointer0);      // Q15 = Q0*Q15
        EvbRegs.CMPR4 = (int16)(Tmp>>16) + (int16)(p->PeriodMax>>1);  // Q0 = (Q15->Q0)/2 + (Q0/2)

// Compute the compare 5 (Q0) from the PWM 9&10 duty cycle ratio (Q15)
        Tmp = (int32)p->PeriodMax*(int32)(*p->PwmDacInPointer1);      // Q15 = Q0*Q15
        EvbRegs.CMPR5 = (int16)(Tmp>>16) + (int16)(p->PeriodMax>>1);  // Q0 = (Q15->Q0)/2 + (Q0/2)

// Compute the compare 6 (Q0) from the PWM 11&12 duty cycle ratio (Q15)
        Tmp = (int32)p->PeriodMax*(int32)(*p->PwmDacInPointer2);      // Q15 = Q0*Q15
        EvbRegs.CMPR6 = (int16)(Tmp>>16) + (int16)(p->PeriodMax>>1);  // Q0 = (Q15->Q0)/2 + (Q0/2)

}

