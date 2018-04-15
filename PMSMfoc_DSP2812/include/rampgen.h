/* =================================================================================
File name:        RAMPGEN.H  (IQ version)                   
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
This file contains type definitions, constants and initializers for
the ramp generation functions contained in RAMPGEN.C
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20                                                 
------------------------------------------------------------------------------*/
#ifndef __RAMPGEN_H__
#define __RAMPGEN_H__

typedef struct { _iq  Freq; 		// Input: Ramp frequency (pu) 	
		 	     _iq  StepAngleMax;	// Parameter: Maximum step angle (pu)		
	 	 	     _iq  Angle;		// Variable: Step angle (pu)					  
			     _iq  Gain;			// Input: Ramp gain (pu)
			     _iq  Out;  	 	// Output: Ramp signal (pu) 	
			     _iq  Offset;		// Input: Ramp offset (pu) 				 
	  	  	     void  (*calc)();	  	// Pointer to calculation function
			   } RAMPGEN;	            

typedef RAMPGEN *RAMPGEN_handle;  
/*------------------------------------------------------------------------------
      Object Initializers
------------------------------------------------------------------------------*/                       
#define RAMPGEN_DEFAULTS {0,0,0,_IQ(1),0,_IQ(1), \
                         (void (*)(Uint32))rampgen_calc }

/*------------------------------------------------------------------------------
      Funtion prototypes
------------------------------------------------------------------------------*/                                               
void rampgen_calc(RAMPGEN_handle);

#endif // __RAMPGEN_H__
