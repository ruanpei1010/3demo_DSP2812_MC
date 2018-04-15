/* ==================================================================================
File name:        F281XPWMDAC.H                     
                    
Originator:	Digital Control Systems Group
			Texas Instruments
Description:  
Header file containing data type and object definitions and 
initializers. Also contains prototypes for the functions in F281XPWMDAC.C.

Target: TMS320F281x family

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20: Using DSP281x v. 1.00 or higher 
---------------------------------------------------------------------------------- */

#ifndef __F281X_PWMDAC_H__
#define __F281X_PWMDAC_H__

#include "f281xbmsk.h"

/*----------------------------------------------------------------------------
Initialization constant for the F281X Timer T3CON for PWMDAC Generation. 
Sets up the timer to run free upon emulation suspend, continuous up-down mode
prescaler 1, timer enabled.
----------------------------------------------------------------------------*/
#define PWMDAC_INIT_STATE  (SOFT_STOP_FLAG +         \
                        TIMER_CONT_UPDN +        \
                        TIMER_CLK_PRESCALE_X_1 + \
                        TIMER_ENABLE_BY_OWN    + \
                        TIMER_ENABLE)
						//仿真挂起，则当前定时器周期结束停止；150MHZ;连续增减(对称PWM)
	
/*----------------------------------------------------------------------------
Initialization constant for the F281X ACTRx register for PWMDAC Generation. 
Sets up PWMDAC polarities.
----------------------------------------------------------------------------*/
#define ACTRB_INIT_STATE ( COMPARE1_AL + \
                          COMPARE2_AH + \
                          COMPARE3_AL + \
                          COMPARE4_AH + \
                          COMPARE5_AL + \
                          COMPARE6_AH )
						  //每一对都是前者低有效，后者高有效
/*----------------------------------------------------------------------------
Initialization constant for the F281X DBTCONx register for PWMDAC Generation. 
Sets up the dead band for PWM and sets up dead band values.
----------------------------------------------------------------------------*/
#define DBTCONB_INIT_STATE ( DBT_VAL_0 +  \
                            EDBT3_EN   +  \
                            EDBT2_EN   +  \
                            EDBT1_EN   +  \
                            DBTPS_X8 )
                            //死区时间0
/*-----------------------------------------------------------------------------
Define the structure of the PWMDAC Driver Object 
-----------------------------------------------------------------------------*/
typedef struct {   
  	int16 *PwmDacInPointer0;   	// Input: Pointer to source data output on PWMDAC channel 0 
	int16 *PwmDacInPointer1;    // Input: Pointer to source data output on PWMDAC channel 1 
	int16 *PwmDacInPointer2;    // Input: Pointer to source data output on PWMDAC channel 2 
	Uint16 PeriodMax;     		// Parameter: PWMDAC half period in number of clocks  (Q0) 
    void (*init)();     	    // Pointer to the init function 
   	void (*update)();   	    // Pointer to the update function 
 	} PWMDAC ;          

/*-----------------------------------------------------------------------------
Define a PWMDAC_handle
-----------------------------------------------------------------------------*/
typedef PWMDAC *PWMDAC_handle;

/*------------------------------------------------------------------------------
Default Initializers for the F281X PWMGEN Object 
------------------------------------------------------------------------------*/
#define F281X_EV2_PWMDAC_DEFAULTS {(int *)0x300, \
                                  (int *)0x300, \
                                  (int *)0x300, \
                                  500,         \
                                  (void (*)(Uint32))F281X_EV2_PWMDAC_Init,  \
                                  (void (*)(Uint32))F281X_EV2_PWMDAC_Update \
                                 }


#define PWMDAC_DEFAULTS     F281X_EV2_PWMDAC_DEFAULTS
/*------------------------------------------------------------------------------
 Prototypes for the functions in F281XPWMDAC.C
------------------------------------------------------------------------------*/
void F281X_EV2_PWMDAC_Init(PWMDAC_handle);
void F281X_EV2_PWMDAC_Update(PWMDAC_handle);

#endif  // __F281X_PWMDAC_H__
