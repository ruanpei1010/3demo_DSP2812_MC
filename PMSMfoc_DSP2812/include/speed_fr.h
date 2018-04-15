/* =================================================================================
File name:        SPEED_FR.H  (IQ version)                    
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
    Header file containing the data types, constants for the rotor angle based
    speed measurement function. Also contains the prototypes for the 
    functions implemented in SPEED_FR.C
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20                                                 
------------------------------------------------------------------------------*/
#ifndef __SPEED_FR_H__
#define __SPEED_FR_H__

typedef struct {
       _iq ElecTheta;  		// Input: Electrical angle (pu) 
       Uint32 DirectionQep; // Variable: Direction of rotation (Q0) - independently with global Q
       _iq OldElecTheta;   	// History: Electrical angle at previous step (pu)
       _iq Speed;       	// Output: Speed in per-unit  (pu)
       Uint32 BaseRpm;      // Parameter: Base speed in rpm (Q0) - independently with global Q
       _iq21 K1;         	// Parameter: Constant for differentiator (Q21) - independently with global Q
       _iq K2;         		// Parameter: Constant for low-pass filter (pu)
       _iq K3;         		// Parameter: Constant for low-pass filter (pu)
       int32 SpeedRpm;      // Output : Speed in rpm  (Q0) - independently with global Q
       void (*calc)();  	// Pointer to the calulation function
       } SPEED_MEAS_QEP;   	// Data type created

typedef SPEED_MEAS_QEP *SPEED_MEAS_QEP_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the SPEED_MEAS_QEP object.
-----------------------------------------------------------------------------*/                     
#define SPEED_MEAS_QEP_DEFAULTS   { 0, \
                                    1, \
                                    0, \
                                    0, \
                                    0, \
                                    0, \
                                    0, \
                                    0, \
                                    0, \
                               (void (*)(Uint32))speed_frq_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in SPEED_FR.C
------------------------------------------------------------------------------*/
void speed_frq_calc(SPEED_MEAS_QEP_handle); 

#endif // __SPEED_FR_H__
