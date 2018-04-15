/*=====================================================================================
 File name:        CLARKE.C  (IQ version)                  
                    
 Originator:	Digital Control Systems Group
			Texas Instruments

 Description:  Clarke Transformation                   

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
-------------------------------------------------------------------------------------*/

#include "IQmathLib.h"         // Include header for IQmath library
// Don't forget to set a proper GLOBAL_Q in "IQmathLib.h" file
#include "dmctype.h"
#include "clarke.h"

void clarke_calc(CLARKE *v)
{	

   v->Alpha = v->As;

   v->Beta = _IQmpy((v->As + _IQmpy(_IQ(2),v->Bs)),_IQ(0.57735026918963)); // 1/sqrt(3) = 0.57735026918963
 
}


