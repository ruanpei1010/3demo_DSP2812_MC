/*=====================================================================================
 File name:        SPEED_FR.C  (IQ version)                  
                    
 Originator:	Digital Control Systems Group
			Texas Instruments

 Description:  Speed measurement computation based QEP encoder (rotor angle)                   

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
-------------------------------------------------------------------------------------*/

#include "IQmathLib.h"         // Include header for IQmath library
// Don't forget to set a proper GLOBAL_Q in "IQmathLib.h" file
#include "dmctype.h"
#include "speed_fr.h"

void speed_frq_calc(SPEED_MEAS_QEP *v)
{

   _iq Tmp1;

// Differentiator  
  // Synchronous speed computation   
   if ((v->ElecTheta < _IQ(0.9))&(v->ElecTheta > _IQ(0.1)))
		// Q21 = Q21*(GLOBAL_Q-GLOBAL_Q)
   		Tmp1 = _IQmpy(v->K1,(v->ElecTheta - v->OldElecTheta));
   else Tmp1 = _IQtoIQ21(v->Speed);


// Low-pass filter 
   // Q21 = GLOBAL_Q*Q21 + GLOBAL_Q*Q21
   Tmp1 = _IQmpy(v->K2,_IQtoIQ21(v->Speed))+_IQmpy(v->K3,Tmp1);  

   if (Tmp1>_IQ21(1))
     v->Speed = _IQ(1);
   else if (Tmp1<_IQ21(-1))
     v->Speed = _IQ(-1);      
   else
     v->Speed = _IQ21toIQ(Tmp1);

// Update the electrical angle
    v->OldElecTheta = v->ElecTheta;
     
// Change motor speed from pu value to rpm value (GLOBAL_Q -> Q0)
// Q0 = Q0*GLOBAL_Q => _IQXmpy(), X = GLOBAL_Q
   v->SpeedRpm = _IQmpy(v->BaseRpm,v->Speed); 

}
