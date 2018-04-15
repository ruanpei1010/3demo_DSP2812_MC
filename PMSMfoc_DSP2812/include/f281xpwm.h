/* ==================================================================================
File name:        F281XPWM.H                     
                    
Originator:	Digital Control Systems Group
			Texas Instruments
Description:  
Header file containing data type and object definitions and 
initializers. Also contains prototypes for the functions in F281XPWM.C.

Target: TMS320F281x family

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20: Using DSP281x v. 1.00 or higher 
---------------------------------------------------------------------------------- */

#ifndef __F281X_PWM_H__
#define __F281X_PWM_H__

#include "f281xbmsk.h"

/*----------------------------------------------------------------------------
Initialization constant for the F281X Timer TxCON for PWM Generation. 
Sets up the timer to run free upon emulation suspend, continuous up-down mode
prescaler 1, timer enabled.
----------------------------------------------------------------------------*/
#define PWM_INIT_STATE  (SOFT_STOP_FLAG +         \
                        TIMER_CONT_UPDN +        \
                        TIMER_CLK_PRESCALE_X_1 + \
                        TIMER_ENABLE_BY_OWN    + \
                        TIMER_ENABLE)	
                        //仿真挂起时，定时器计完本周期回到0后停止+连续增减模式(对称PWM)
						//＋无输入预定标＋定时器使能

/*----------------------------------------------------------------------------
Initialization constant for the F281X ACTRx register for PWM Generation. 
Sets up PWM polarities.
----------------------------------------------------------------------------*/
#define ACTR_INIT_STATE ( COMPARE1_AH + \
                          COMPARE2_AL + \
                          COMPARE3_AH + \
                          COMPARE4_AL + \
                          COMPARE5_AH + \
                          COMPARE6_AL )
						  //PWM1、3、5高有效，PWM2、4、6低有效

/*----------------------------------------------------------------------------
Initialization constant for the F281X DBTCONx register for PWM Generation. 
Sets up the dead band for PWM and sets up dead band values.
----------------------------------------------------------------------------*/
#define DBTCON_INIT_STATE ( DBT_VAL_15 +  \
                            EDBT3_EN   +  \
                            EDBT2_EN   +  \
                            EDBT1_EN   +  \
                            DBTPS_X32 )
							//死区定时器周期数为10＋死区定时器周期1、2、3使能＋死区定时器预定标为8
							//死区时间(15*32)/150M＝3.2us

/*-----------------------------------------------------------------------------
Define the structure of the PWM Driver Object 
-----------------------------------------------------------------------------*/
typedef struct {   
        Uint16 PeriodMax;     // Parameter: PWM Half-Period in CPU clock cycles (Q0)
        int16 MfuncPeriod;    // Input: Period scaler (Q15) 
        int16 MfuncC1;        // Input: PWM 1&2 Duty cycle ratio (Q15)
        int16 MfuncC2;        // Input: PWM 3&4 Duty cycle ratio (Q15)
        int16 MfuncC3;        // Input: PWM 5&6 Duty cycle ratio (Q15)
        void (*init)();       // Pointer to the init function 
        void (*update)();     // Pointer to the update function 
        } PWMGEN ;    

/*-----------------------------------------------------------------------------
Define a PWMGEN_handle
-----------------------------------------------------------------------------*/
typedef PWMGEN *PWMGEN_handle;

/*------------------------------------------------------------------------------
Default Initializers for the F281X PWMGEN Object 
------------------------------------------------------------------------------*/
#define F281X_EV1_FC_PWM_GEN {1000,   \
                              0x7FFF, \
                              0x4000, \
                              0x4000, \
                              0x4000, \
                             (void (*)(Uint32))F281X_EV1_PWM_Init,  \
                             (void (*)(Uint32))F281X_EV1_PWM_Update \
                             }

#define F281X_EV2_FC_PWM_GEN {1000,   \
                              0x7FFF, \
                              0x4000, \
                              0x4000, \
                              0x4000, \
                             (void (*)(Uint32))F281X_EV2_PWM_Init,  \
                             (void (*)(Uint32))F281X_EV2_PWM_Update \
                             }

#define PWMGEN_DEFAULTS 	F281X_EV1_FC_PWM_GEN
/*------------------------------------------------------------------------------
 Prototypes for the functions in F281XPWM.C
------------------------------------------------------------------------------*/
void F281X_EV1_PWM_Init(PWMGEN_handle);
void F281X_EV1_PWM_Update(PWMGEN_handle);

#endif  // __F281X_PWM_H__

