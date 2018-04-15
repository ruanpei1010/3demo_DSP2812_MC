/* ==================================================================================
File name:        F281XBMSK.H                     
                    
Originator:	Digital Control Systems Group
			Texas Instruments
Description:  
Header file containing handy bitmasks for setting up register values.
This file defines the bitmasks for F281X.

Target: TMS320F281x family

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20: Using DSP281x v. 1.00 or higher 
---------------------------------------------------------------------------------- */

#ifndef  __F281X_BMSK_H__
#define  __F281X_BMSK_H__

/*------------------------------------------------------------------------------
 F281X Register TxCON
------------------------------------------------------------------------------*/
#define FREE_RUN_FLAG               0x8000
#define SOFT_STOP_FLAG              0x4000
                                      
//TxCON(12~11),计数模式选择
#define TIMER_STOP                  0x0000	//00 停止、保持
#define TIMER_CONT_UPDN             0x0800	//01 连续增/减计数模式
#define TIMER_CONT_UP               0x1000	//10 连续增计数模式
#define TIMER_DIR_UPDN              0x1800	//11 定向增/减计数模式

//TxCON(10～8)，输入时钟预定标参数
#define TIMER_CLK_PRESCALE_X_1      0x0000
#define TIMER_CLK_PRESCALE_X_2      0x0100
#define TIMER_CLK_PRESCALE_X_4      0x0200
#define TIMER_CLK_PRESCALE_X_8  	0x0300
#define TIMER_CLK_PRESCALE_X_16  	0x0400
#define TIMER_CLK_PRESCALE_X_32  	0x0500
#define TIMER_CLK_PRESCALE_X_64  	0x0600
#define TIMER_CLK_PRESCALE_X_128 	0x0700

#define TIMER_ENABLE_BY_OWN		    0x0000
#define TIMER_ENABLE_BY_T1      	0x0080

#define TIMER_ENABLE            	0x0040
#define TIMER_DISABLE           	0x0000

#define TIMER_CLOCK_SRC_INTERNAL  	0x0000
#define TIMER_CLOCK_SRC_EXTERNAL  	0x0010
#define TIMER_CLOCK_SRC_QEP       	0x0030

#define TIMER_COMPARE_LD_ON_ZERO   	0x0000
#define TIMER_COMPARE_LD_ON_ZERO_OR_PRD 0x0004
#define TIMER_COMPARE_LD_IMMEDIATE 	0x0008

#define TIMER_ENABLE_COMPARE        0x0002
#define TIMER_SELECT_T1_PERIOD      0x0001

/*------------------------------------------------------------------------------
 F281X Register ACTR 0x7413 BIT FIELD MASKS
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Space Vector Direction Commands
------------------------------------------------------------------------------*/
#define SV_DIRECTION_CW             0x8000
#define SV_DIRECTION_CCW            0x0000

/*------------------------------------------------------------------------------
Space Vector Generation Vectors
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
#define SPACE_VECTOR_0		        0x0000
#define SPACE_VECTOR_1		        0x1000
#define SPACE_VECTOR_2		        0x2000
#define SPACE_VECTOR_3		        0x3000
#define SPACE_VECTOR_4		        0x4000
#define SPACE_VECTOR_5		        0x5000
#define SPACE_VECTOR_6		        0x6000
#define SPACE_VECTOR_7		        0x7000

/*------------------------------------------------------------------------------
 Compare action definitions
------------------------------------------------------------------------------*/
#define COMPARE6_FL		        0x0000
#define COMPARE6_AL		        0x0400
#define COMPARE6_AH		        0x0800
#define COMPARE6_FH		        0x0C00
//------------------------------------------------------------------------------
#define COMPARE5_FL		        0x0000
#define COMPARE5_AL		        0x0100
#define COMPARE5_AH		        0x0200
#define COMPARE5_FH		        0x0300
//------------------------------------------------------------------------------
#define COMPARE4_FL		        0x0000
#define COMPARE4_AL		        0x0040
#define COMPARE4_AH		        0x0080
#define COMPARE4_FH		        0x00C0
//------------------------------------------------------------------------------
#define COMPARE3_FL		        0x0000
#define COMPARE3_AL		        0x0010
#define COMPARE3_AH		        0x0020
#define COMPARE3_FH		        0x0030
//------------------------------------------------------------------------------
#define COMPARE2_FL		        0x0000
#define COMPARE2_AL		        0x0004
#define COMPARE2_AH		        0x0008
#define COMPARE2_FH		        0x000C
//------------------------------------------------------------------------------
#define COMPARE1_FL		        0x0000
#define COMPARE1_AL		        0x0001
#define COMPARE1_AH		        0x0002
#define COMPARE1_FH		        0x0003
//------------------------------------------------------------------------------

/*------------------------------------------------------------------------------
 F281X Register COMCONA/COMCONB
------------------------------------------------------------------------------*/
#define CMPR_ENABLE					0x8000
#define CMPR_LD_ON_ZERO         	0x0000
#define CMPR_LD_ON_ZERO_OR_PRD  	0x2000
#define CMPR_LD_IMMEDIATE       	0x4000
#define SVENABLE                	0x1000
#define SVDISABLE               	0x0000
#define ACTR_LD_ON_ZERO         	0x0000
#define ACTR_LD_ON_ZERO_OR_PRD  	0x0400
#define ACTR_LD_IMMEDIATE       	0x0800
#define FCOMPOE                 	0x0100     

/*------------------------------------------------------------------------------
 F281X Register DBTCON
------------------------------------------------------------------------------*/
#define DBT_VAL_0	        	0x0000
#define DBT_VAL_1	        	0x0100
#define DBT_VAL_2               0x0200
#define DBT_VAL_3               0x0300
#define DBT_VAL_4               0x0400
#define DBT_VAL_5               0x0500
#define DBT_VAL_6               0x0600
#define DBT_VAL_7               0x0700
#define DBT_VAL_8               0x0800
#define DBT_VAL_9               0x0900
#define DBT_VAL_10              0x0A00
#define DBT_VAL_11              0x0B00
#define DBT_VAL_12              0x0C00
#define DBT_VAL_13              0x0D00
#define DBT_VAL_14              0x0E00
#define DBT_VAL_15              0x0F00

#define EDBT3_DIS               0x0000
#define EDBT3_EN                0x0080
#define EDBT2_DIS               0x0000
#define EDBT2_EN                0x0040
#define EDBT1_DIS               0x0000
#define EDBT1_EN                0x0020

#define DBTPS_X32               0x0014
#define DBTPS_X16               0x0010
#define DBTPS_X8                0x000C
#define DBTPS_X4                0x0008
#define DBTPS_X2                0x0004
#define DBTPS_X1                0x0000

/*------------------------------------------------------------------------------
 F281X Register ADCTRL1
------------------------------------------------------------------------------*/
#define ADC_SUS_MODE0           0x0000
#define ADC_SUS_MODE1           0X1000
#define ADC_SUS_MODE2           0x2000
#define ADC_SUS_MODE3           0X3000
#define ADC_RESET_FLAG          0x4000

#define ADC_ACQ_PS_1            0x0000
#define ADC_ACQ_PS_2            0x0100
#define ADC_ACQ_PS_3            0x0200
#define ADC_ACQ_PS_4            0x0300
#define ADC_ACQ_PS_5            0x0400
#define ADC_ACQ_PS_6            0x0500
#define ADC_ACQ_PS_7            0x0600
#define ADC_ACQ_PS_8            0x0700
#define ADC_ACQ_PS_9            0x0800
#define ADC_ACQ_PS_10           0x0900
#define ADC_ACQ_PS_11           0x0A00
#define ADC_ACQ_PS_12           0x0B00
#define ADC_ACQ_PS_13           0x0C00
#define ADC_ACQ_PS_14           0x0D00
#define ADC_ACQ_PS_15           0x0E00
#define ADC_ACQ_PS_16           0x0F00

#define ADC_CPS_1               0x0000
#define ADC_CPS_2               0x0080
#define ADC_CONT_RUN            0x0040
#define ADC_SEQ_CASC            0x0010
#define ADC_SEQ_DUAL            0x0000

/*------------------------------------------------------------------------------
 F281X Register ADCTRL2
------------------------------------------------------------------------------*/
#define ADC_EVB_SOC             0x8000
#define ADC_RST_SEQ1            0x4000
#define ADC_SOC_SEQ1            0x2000

#define ADC_INT_ENA_SEQ1        0x0800
#define ADC_INT_MODE_SEQ1       0X0400
#define ADC_EVA_SOC_SEQ1        0x0100

#define ADC_EXT_SOC_SEQ1        0x0080
#define ADC_RST_SEQ2            0x0040
#define ADC_SOC_SEQ2            0x0020

#define ADC_INT_ENA_SEQ2        0x0008
#define ADC_INT_MODE_SEQ2       0x0004
#define ADC_EVB_SOC_SEQ2        0x0001

/*------------------------------------------------------------------------------
 F281X Register ADCTRL3
------------------------------------------------------------------------------*/
#define ADC_RFDN                0x0080
#define ADC_BGDN                0x0040
#define ADC_PWDN                0x0020

#define ADC_CLKPS_X_1           0x0000
#define ADC_CLKPS_X_2           0x0002
#define ADC_CLKPS_X_4           0x0004
#define ADC_CLKPS_X_6           0x0006
#define ADC_CLKPS_X_8           0x0008
#define ADC_CLKPS_X_10          0x000A
#define ADC_CLKPS_X_12          0x000C
#define ADC_CLKPS_X_14          0x000E
#define ADC_CLKPS_X_16          0x0010
#define ADC_CLKPS_X_18          0x0012
#define ADC_CLKPS_X_20          0x0014
#define ADC_CLKPS_X_22          0x0016
#define ADC_CLKPS_X_24          0x0018
#define ADC_CLKPS_X_26          0x001A
#define ADC_CLKPS_X_28          0x001C
#define ADC_CLKPS_X_30          0x001E

#define ADC_SMODE_SIMULTANEOUS  0x0001
#define ADC_SMODE_SEQUENTIAL    0x0000

#endif  // __F281X_BMSK_H__
// EOF


