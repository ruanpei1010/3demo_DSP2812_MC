/* =================================================================================
File name:        RMP_CNTL.H  (IQ version)                  
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
Header file containing constants, data type definitions, and 
function prototypes for the RMPCNTL module.
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20                                                   
------------------------------------------------------------------------------*/
#ifndef __RMP_CNTL_H__
#define __RMP_CNTL_H__

typedef struct { _iq    TargetValue; 	// Input: Target input (pu)
				 Uint32 RampDelayMax;	//过多少个采样周期调整一次,可以控制调节速度SetpointValue,Parameter: Maximum delay rate (Q0) - independently with global Q			
		 	 	 _iq    RampLowLimit;	// Parameter: Minimum limit (pu)				  
				 _iq    RampHighLimit;	// Parameter: Maximum limit (pu)
				 Uint32 RampDelayCount; // Variable: Incremental delay (Q0) - independently with global Q	
				 _iq    SetpointValue;	// Output: Target output (pu)				 
				 Uint32 EqualFlag;		// Output: Flag output (Q0) - independently with global Q
		  	  	 void (*calc)();	  	// Pointer to calculation function
				 } RMPCNTL;	            

typedef RMPCNTL *RMPCNTL_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the RMPCNTL object.
-----------------------------------------------------------------------------*/                     
#define RMPCNTL_DEFAULTS { 0, \
                           40, \
                          	_IQ(-1), \
                          	_IQ(1), \
                          	0, \
                          	0, \
                          	0, \
                   			(void (*)(Uint32))rmp_cntl_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in RMP_CNTL.C
------------------------------------------------------------------------------*/
void rmp_cntl_calc(RMPCNTL_handle);

#endif // __RMP_CNTL_H__
