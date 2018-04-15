/* ==================================================================================
File name:        F281XQEP.H                     
                    
Originator:	Digital Control Systems Group
			Texas Instruments
Description:  
Header file containing data type and object definitions and 
initializers. Also contains prototypes for the functions in F281XQEP.C.

Target: TMS320F281x family

=====================================================================================
History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20: Using DSP281x v. 1.00 or higher 
----------------------------------------------------------------------------------*/

#ifndef __F281X_QEP_H__
#define __F281X_QEP_H__

#include "f281xbmsk.h"

/*------------------------------------------------------------------------------
Initialization states for T2CON and CAPCON 
------------------------------------------------------------------------------*/
#define QEP_CAP_INIT_STATE    0x9004
		//CAP1、CAP2作为QEP1、QEP2引脚+CAP3作为捕获引脚+CAP3捕获T2;T2的+CAP3捕获上升沿
#define QEP_TIMER_INIT_STATE (FREE_RUN_FLAG +          \
                          TIMER_DIR_UPDN +         \
                          TIMER_CLK_PRESCALE_X_1 + \
                          TIMER_ENABLE_BY_OWN +    \
                          TIMER_ENABLE +           \
                          TIMER_CLOCK_SRC_QEP +    \
                          TIMER_COMPARE_LD_ON_ZERO)
                          //自由运行+定向增减计数模式+时钟预定标为1+启动定时器+时钟源和方向为QEP电路+比较寄存器下溢刷新;

/*-----------------------------------------------------------------------------
Define the structure of the QEP (Quadrature Encoder) Driver Object 
-----------------------------------------------------------------------------*/
typedef struct {int16 ElecTheta;        // Output: Motor Electrical angle (Q15)
                int16 MechTheta;        // Output: Motor Mechanical Angle (Q15) 
                Uint16 DirectionQep;    // Output: Motor rotation direction (Q0)
                Uint16 QepCountIndex;   // Variable: Encoder counter index (Q0) 
                Uint16 RawTheta;        // Variable: Raw angle from Timer 2 (Q0)
                Uint32 MechScaler;      // Parameter: 0.9999/total count (Q30) 
                Uint16 LineEncoder;     // Parameter: Number of line encoder (Q0) 
                Uint16 PolePairs;       // Parameter: Number of pole pairs (Q0) 
                int16 CalibratedAngle; // Parameter: Raw angular offset between encoder index and phase a (Q0)
                Uint16 IndexSyncFlag;   // Output: Index sync status (Q0) 
                void (*init)();         // Pointer to the init function 
                void (*calc)();         // Pointer to the calc function 
                void (*isr)();          // Pointer to the isr function  
                }  QEP;

/*-----------------------------------------------------------------------------
Define a QEP_handle
-----------------------------------------------------------------------------*/
typedef QEP *QEP_handle;

/*-----------------------------------------------------------------------------
Default initializer for the QEP Object.
-----------------------------------------------------------------------------*/
// Applied-motion PMSM motor: 24-v, 8-pole, 2000 line encoder, CalibratedAngle = -1250
// MechScaler = 1/8000 = 0x00020C4A (Q30) 

// PacSci 1-hp PMSM motor: 320-v, 4-pole, 1000 line encoder, CalibratedAngle = -2365
// MechScaler = 1/4000 = 0x00041893 (Q30)

#define QEP_DEFAULTS { 0x0, 0x0,0x0,0x0,0x0,0x00020C4A,0x0,4,-1250,0x0,  \
                (void (*)(Uint32))F281X_EV1_QEP_Init,            \
                (void (*)(Uint32))F281X_EV1_QEP_Calc, 	  		  \
                (void (*)(Uint32))F281X_EV1_QEP_Isr }
				//需要设置MechScaler、LineEncoder、PolePairs、CalibratedAngle这四个参数

/*-----------------------------------------------------------------------------
Prototypes for the functions in F281XQEP.C                                 
-----------------------------------------------------------------------------*/
void F281X_EV1_QEP_Init(QEP_handle);                                              
void F281X_EV1_QEP_Calc(QEP_handle);
void F281X_EV1_QEP_Isr(QEP_handle);
                
#endif // __F281X_QEP_H__ 




