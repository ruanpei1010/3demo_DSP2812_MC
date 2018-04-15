/*=====================================================================================
 File name:        RAMPGEN.C  (IQ version)                  
                    
 Originator:	Digital Control Systems Group
			Texas Instruments

 Description:  The Ramp Generation                   

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
-------------------------------------------------------------------------------------*/

#include "IQmathLib.h"         // Include header for IQmath library
// Don't forget to set a proper GLOBAL_Q in "IQmathLib.h" file
#include "dmctype.h"
#include "rampgen.h"

void rampgen_calc(RAMPGEN *v)
{	

	// Compute the angle rate
	v->Angle += _IQmpy(v->StepAngleMax,v->Freq);       

	// Saturate the angle rate within (-1,1)        
	if (v->Angle>_IQ(1.0))
	v->Angle -= _IQ(1.0);
	else if (v->Angle<_IQ(-1.0))
	v->Angle += _IQ(1.0);

	// Compute the ramp output
	v->Out = _IQmpy(v->Angle,v->Gain) + v->Offset;

	// Saturate the ramp output within (-1,1)     
	if (v->Out>_IQ(1.0))
	v->Out -= _IQ(1.0);
	else if (v->Out<_IQ(-1.0))
	v->Out += _IQ(1.0);

}

