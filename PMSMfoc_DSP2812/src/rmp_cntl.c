/*=====================================================================================
 File name:        RMP_CNTL.C  (IQ version)                  
                    
 Originator:	Digital Control Systems Group
			Texas Instruments

 Description:  The Ramp Control                   

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
-------------------------------------------------------------------------------------*/

#include "IQmathLib.h"         // Include header for IQmath library
// Don't forget to set a proper GLOBAL_Q in "IQmathLib.h" file
#include "dmctype.h"
#include "rmp_cntl.h"


void rmp_cntl_calc(RMPCNTL *v)
{	
     _iq tmp;
       
     tmp = v->TargetValue - v->SetpointValue;
     
      if (_IQabs(tmp) > _IQ(0.0000305))
      {
         v->RampDelayCount += 1;
         if (v->RampDelayCount >= v->RampDelayMax)
         {
           if (v->TargetValue >= v->SetpointValue)
           {
            v->SetpointValue += _IQ(0.0000305);        // 0.0000305 is resolution of Q15
            if (v->SetpointValue > v->RampHighLimit)
              v->SetpointValue = v->RampHighLimit;           
            v->RampDelayCount = 0;
           }
           else
           {
            v->SetpointValue -= _IQ(0.0000305);        // 0.0000305 is resolution of Q15
            if (v->SetpointValue < v->RampLowLimit)
              v->SetpointValue = v->RampLowLimit;           
            v->RampDelayCount = 0;
           }
         }        
      }
      else  v->EqualFlag = 0x7FFFFFFF; 
}


