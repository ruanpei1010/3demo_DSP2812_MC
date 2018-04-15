/* ==================================================================================
File name:       F281XILEG_VDC.H
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: This header file contains source for the F281X Two leg current measurement 
and DC-bus measurement driver.

Target: TMS320F281x family

=====================================================================================
History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20: Using DSP281x v. 1.00 or higher 
----------------------------------------------------------------------------------*/

#ifndef __F281XILEG_VDC_H__
#define __F281XILEG_VDC_H__

#include "f281xbmsk.h"

/*-----------------------------------------------------------------------------
Define the structure of the ILEG2DCBUSMEAS Object
-----------------------------------------------------------------------------*/
typedef struct { int16 ImeasAGain;     // Parameter: gain for Ia (Q13) 
                 int16 ImeasAOffset;   // Parameter: offset for Ia (Q15) 
                 int16 ImeasA;         // Output: measured Ia (Q15) 
                 int16 ImeasBGain;     // Parameter: gain for Ib (Q13) 
                 int16 ImeasBOffset;   // Parameter: offset for Ib (Q15) 
                 int16 ImeasB;         // Output: measured Ib (Q15) 
                 int16 VdcMeasGain;    // Parameter: gain for Vdc (Q13) 
                 int16 VdcMeasOffset;  // Parameter: offset for Vdc (Q15) 
                 int16 VdcMeas;        // Output: measured Vdc (Q15) 
				 int16 ImeasC;	  	   // Output: computed Ic (Q15) 	
                 Uint16 ChSelect;      // Parameter: ADC channel selection
                 void (*init)();       // Pointer to the init function 
                 void (*read)();       // Pointer to the read function 
               } ILEG2DCBUSMEAS;

typedef ILEG2DCBUSMEAS *ILEG2DCBUSMEAS_handle;
/*-----------------------------------------------------------------------------
 Note 1 : It is necessary to call the init function to change the ADC 
            register settings, for the change in the channel setting for 
            ChSelect setting changes to take effect.
            The read function will not detect or act upon this change.
-----------------------------------------------------------------------------*/
// Default Initializer for the ILEG2DCBUSMEAS Object
// DMC1500 + eZdsp2812: ChSelect = 0x0710
// DMC550 + eZdsp2812: ChSelect = 0x0610
// Note: Assuming the base DC-bus voltage is 24 volt 
//        - ADC input for Vdc_bus range is 24*1/(24.9+1) = 0.927 volt on DMC550
//        - Then, Vdc_bus gain = 3.0/0.927 = 3.2375 (or 0x675C in Q13) 

#define F281X_ILEG2_DCBUS_MEAS_DEFAULTS { 0x1FFF,0x0000,0x0000,             \
                                          0x1FFF,0x0000,0x0000,             \
                                          0x1FFF,0x0000,0x0000,             \
                                          0x0000,0x0710,                    \
                                          (void (*)(Uint32))F281X_ileg2_dcbus_drv_init, \
                                          (void (*)(Uint32))F281X_ileg2_dcbus_drv_read  \
                                         }//Ia、Ib的增益为1

#define ILEG2DCBUSMEAS_DEFAULTS F281X_ILEG2_DCBUS_MEAS_DEFAULTS 

#define ADCTRL1_INIT_STATE  ADC_SUS_MODE0 + ADC_ACQ_PS_2 + \
                            ADC_CPS_1 + ADC_SEQ_CASC
                            //仿真挂起被忽略＋采样脉冲宽度为2个ADCLK周期＋时钟预定标1分频＋级联模式

#define ADCTRL2_INIT_STATE  ADC_EVA_SOC_SEQ1	//允许EVA启动AD级联排序器

/*------------------------------------------------------------------------------
 Prototypes for the functions in F281XILEG_VDC.C
------------------------------------------------------------------------------*/
void F281X_ileg2_dcbus_drv_init(ILEG2DCBUSMEAS *);
void F281X_ileg2_dcbus_drv_read(ILEG2DCBUSMEAS *);

#endif // __F281XILEG_VDC_H__
