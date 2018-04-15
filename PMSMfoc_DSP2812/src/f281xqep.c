/* ==================================================================================
File name:       F281XQEP.C
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description:   This file contains source for the QEP drivers for the F281X

Target: TMS320F281x family
          
=====================================================================================
History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20: Using DSP281x v. 1.00 or higher 
----------------------------------------------------------------------------------*/

#include "DSP281x_Device.h"
#include "f281xqep.h"

void  F281X_EV1_QEP_Init(QEP *p)
{
 
          EvaRegs.CAPCONA.all = QEP_CAP_INIT_STATE;    //CAP1、CAP2作为QEP1、QEP2引脚,T2计脉冲数;CAP3作为捕获T2的引脚,捕获上升沿
          EvaRegs.T2CON.all = QEP_TIMER_INIT_STATE;    //自由运行;时钟源和方向为QEP电路;定向增减计数模式
          EvaRegs.T2PR = 4*p->LineEncoder;             // Init Timer 1 period Register 
          EvaRegs.EVAIFRC.bit.CAP3INT = 1;     // Clear CAP3 flag
          EvaRegs.EVAIMRC.bit.CAP3INT = 1;     // Enable CAP3 Interrupt

          EALLOW;                       // Enable EALLOW
          GpioMuxRegs.GPAMUX.all |= 0x0700;     // Set up the capture pins to primary functions
          EDIS;                         // Disable EALLOW
}


void F281X_EV1_QEP_Calc(QEP *p)
{

     int32 Tmp;

// Check the rotational direction 
     p->DirectionQep = 0x4000&EvaRegs.GPTCONA.all;
     p->DirectionQep = p->DirectionQep>>14;				//0和1代表不同的转向

// Check the timer 2 counter for QEP 
     p->RawTheta = EvaRegs.T2CNT + p->CalibratedAngle;

// Compute the mechanical angle in Q15
     Tmp = __qmpy32by16(p->MechScaler,p->RawTheta,31);    // Q15 = Q30*Q0 
     p->MechTheta = (int16)(Tmp);                         // Q15 -> Q15
     p->MechTheta &= 0x7FFF;                              // Wrap around 0x07FFF,把一个-1~1换成两个0~1

// Compute the electrical angle in Q15
     p->ElecTheta = p->PolePairs*p->MechTheta;            // 电角度=p*机械角度,Q0*Q15 = Q15
     p->ElecTheta &= 0x7FFF;                              // Wrap around 0x07FFF,把一个-1~1换成两个0~1
}

void F281X_EV1_QEP_Isr(QEP *p)
{

     p->QepCountIndex = EvaRegs.T2CNT;    // Get the timer 2 counter for one mechanical revolution 
     EvaRegs.T2CNT = 0;                   // Reset the timer 2 counter
     p->IndexSyncFlag = 0x00F0;           // Set the index flag

}
