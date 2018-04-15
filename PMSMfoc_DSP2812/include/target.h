/* =================================================================================
File name:        TARGET.H                    
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
Target device selection file.
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
------------------------------------------------------------------------------*/
 
#ifndef DSP_TARGET 

/*------------------------------------------------------------------------------
Following is the selection list of the target choices.
Note that the F2808 also represents the F2801 and F2806 devices, 
and F2812 also represents the F2810 and F2811 devices.
------------------------------------------------------------------------------*/
#define F2808   1
#define F2812   2
#define UNKNOWN 4

/*------------------------------------------------------------------------------
This line sets the target to one of the available choices.
------------------------------------------------------------------------------*/
#ifdef TARGET_IS_F2808
#define DSP_TARGET F2808
#endif

#ifdef TARGET_IS_F2812
#define DSP_TARGET F2812
#endif


#ifndef DSP_TARGET     
#error  Critical: A target must be defined Please define a target in  the \
CCS Build Options/Compiler Tab/Preprocessor/ Symbol defines.
#endif  

#endif  // TARGET

