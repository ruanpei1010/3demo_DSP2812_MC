/* ==============================================================================
System Name:  PMSM31

File Name:	PMSM3_1.C

Description:	Primary system file for the Real Implementation of Sensored  
          		Field Orientation Control for a Three Phase Permanent-Magnet
          		Synchronous Motor (PMSM) using QEP sensor

Originator:		Digital control systems Group - Texas Instruments

Note: In this software, the default inverter is supposed to be DMC1500 board. 
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20: Support both F280x and F281x targets 
 04-25-2005 Version 3.21: Move EINT and ERTM down to ensure that all initialization
 						  is completed before interrupts are allowed.
=================================================================================  */
	#include "DSP281x_Device.h"
	#include "IQmathLib.h"
	#include "pmsm3_1.h"
	#include "parameter.h"
	#include "build.h"

//=========���ļ����õ��ĺ���ԭ������============	
	interrupt void MainISR(void);
	interrupt void QepISR(void);
	interrupt void SCIB_IX_ISR(void);
	void Delay_Us(Uint16,Uint16);
	
	#define CPU_CLOCK_SPEED      6.6667L   // for a 150MHz CPU clock speed
	#define DELAY_US(A)  DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_CLOCK_SPEED) - 9.0L) / 5.0L)
	extern void DSP28x_usDelay(unsigned long Count);

//============ȫ�ֱ�������=======================
	float32 T = 0.001/ISR_FREQUENCY;   	//PWMƵ��20KHZ,����50us
	
	float32 SpeedRef = 0.15;			//�ٶȲο�ֵ(����ֵ),(0.2) 
	float32 IqRef = 0.1;				// Iq�ο�ֵ(����ֵ)
	float32 IdRef = 0;					// Id�ο�ֵ(����ֵ) 
	float32 VqTesting = 0.18;			// Vq����ֵ(����ֵ),(0.25)   
	float32 VdTesting = 0;				// Vd����ֵ(����ֵ)
	float32 angle;
	 
	Uint16 BackTicker = 0;
	Uint16 IsrTicker = 0;

	int16 PwmDacCh1=0;
	int16 PwmDacCh2=0;
	int16 PwmDacCh3=0;

	int16 DlogCh1 = 10;
	int16 DlogCh2 = 10;
	int16 DlogCh3 = 10;
	int16 DlogCh4 = 10;

	Uint16 LockRotorFlag = FALSE;

	Uint16 SpeedLoopPrescaler =10;      // Speed loop prescaler
	Uint16 SpeedLoopCount = 1;           // Speed loop counter

	Uint16 Counter1,Counter2;			//������ʱѭ������
	Uint16 rcounter=0;					//��ת��Ȧ��

	Uint16	SCI_1;	//���͸�SCI�ڵ�4��8λ�����ݣ�SCI_4-SCI_3-SCI_2-SCI_1
	Uint16	SCI_2;
	Uint16	SCI_3;
	Uint16	SCI_4;
//============ȫ�ֱ�����������=======================



//=============ȫ�ֶ�����������ʼ����================
	//����(ֱ����ѹ)ADC����
	ILEG2DCBUSMEAS ilg2_vdc1 = ILEG2DCBUSMEAS_DEFAULTS;
	
	//λ�ýǶȼ�ת�ټ������
	QEP qep1 = QEP_DEFAULTS;
	SPEED_MEAS_QEP speed1 = SPEED_MEAS_QEP_DEFAULTS;
	
	//����任����
	CLARKE clarke1 = CLARKE_DEFAULTS;	
	PARK park1 = PARK_DEFAULTS;	
	IPARK ipark1 = IPARK_DEFAULTS;	
	
	//PID����������
	PIDREG3 pid1_spd = PIDREG3_DEFAULTS;
	PIDREG3 pid1_iq = PIDREG3_DEFAULTS;
	PIDREG3 pid1_id = PIDREG3_DEFAULTS;
	
	//�ռ�ʸ�����㼰PWM��ض���
	SVGENDQ svgen_dq1 = SVGENDQ_DEFAULTS;
	PWMGEN pwm1 = PWMGEN_DEFAULTS;
	PWMDAC pwmdac1 = PWMDAC_DEFAULTS;

	//������������ 
	RMPCNTL rc1 = RMPCNTL_DEFAULTS;
	RAMPGEN rg1 = RAMPGEN_DEFAULTS;
	DLOG_4CH dlog = DLOG_4CH_DEFAULTS;      
//=============ȫ�ֶ�����������ʼ�������================



//==================������===============================
void main(void)
{
	//��ʼ��ϵͳ����
	InitSysCtrl();						//�رտ��Ź�;����PLL(5*30MHZ);���ò��򿪴�Ĭ�ϸߵ�������ʱ��
	EALLOW;
    SysCtrlRegs.HISPCP.all = 0x0000;	// ���ø�������ʱ��Ϊ150MHZ
    SysCtrlRegs.LOSPCP.all=0x0002;		//���ø�������ʱ��(SCI)37.5MHZ  
    EDIS; 
	
	//��ֹ���������CPU�ж�
	DINT;
	IER = 0x0000;
	IFR = 0x0000;
	
	//����ж�������
	InitPieCtrl();						//DINT;��ʹ��PIE;���12��PIEIER��PIEIFR
	InitPieVectTable();					//���ж���������װ��ָ����ʽISR��ָ��;ʹ��PIE	
	EALLOW;
	PieVectTable.T1UFINT = &MainISR;	//���ж���T1�����ж�
	PieVectTable.CAPINT3 = &QepISR;		//cap3�ж�
	PieVectTable.TXBINT=&SCIB_IX_ISR;
	EDIS;
	
	//��ͨ����SCI-A
	EALLOW;
	GpioMuxRegs.GPGMUX.all=0x0030;	//GPIOF4��GPIOF5��ΪSCI��
	EDIS;
	ScibRegs.SCICCR.all=0x0007;		//8λ���ݣ�����żУ�飬һλֹͣλ
	ScibRegs.SCICTL1.all=0x0022;	//ʹSCI�˳���λ״̬��ʹ�ܷ���
	ScibRegs.SCIHBAUD=0x0000;
	ScibRegs.SCILBAUD=0x0027;		//������Ϊ117187b/s

	//��ʼ��EVA��ͨ�ö�ʱ�����ƼĴ���A
	EvaRegs.GPTCONA.all = 0;			//T1��T2���¼�����ADC����ֹ��ʱ���Ƚ����T1PWM��T2PWM

	//��ͨT1UFINT,CAP3,SCITXINTB�ж�
	EvaRegs.EVAIMRA.bit.T1UFINT = 1;	//ʹ��T1UFINT
    EvaRegs.EVAIFRA.bit.T1UFINT = 1;	//���T1UFINT�жϱ�־
    EvaRegs.EVAIMRC.bit.CAP3INT = 1;	//ʹ��CAP3�ж�
    EvaRegs.EVAIFRC.bit.CAP3INT = 1;	//���CAP3�жϱ�־
	
	PieCtrlRegs.PIEIER9.all = M_INT4;
    PieCtrlRegs.PIEIER2.all = M_INT6;
    PieCtrlRegs.PIEIER3.all = M_INT7;
	IER |= (M_INT2 | M_INT3| M_INT9);
	
	//��ʼ��GBIOB6(s4)��GPIOB7(s12)������Ϊ��������,����s4���٣�s12����
	//��ʼ��GBIOB8(s5)��GPIOE0(s13)������Ϊ��������,����s5����ת��,s13����ת��
	GpioMuxRegs.GPBMUX.bit.T3PWM_GPIOB6=0x0;	//I/O
	GpioMuxRegs.GPBMUX.bit.T4PWM_GPIOB7=0x0;	//I/O
	GpioMuxRegs.GPBMUX.bit.CAP4Q1_GPIOB8=0x0;	//I/O
	GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=0x0;	//I/O

	GpioMuxRegs.GPBDIR.bit.GPIOB6=0x0;			//IN
	GpioMuxRegs.GPBDIR.bit.GPIOB7=0x0;			//IN
	GpioMuxRegs.GPBDIR.bit.GPIOB8=0x0;			//IN
	GpioMuxRegs.GPEDIR.bit.GPIOE0=0x0;			//IN
			
//=========��������Ĳ������ü�Ӳ����ʼ������============
	//����(ֱ����ѹ)ADC����
    ilg2_vdc1.ChSelect=0x3210;	//Ia:ADINA0��Ib:ADCINA1��Ic:ADCINA2��vsense:ADCINA3
    ilg2_vdc1.init(&ilg2_vdc1);	//T1����ʱ����ADת�������ò�ѯ��ʽ��ȡ�����͵�ѹ��AD���
	
	//�Ƕȼ��ٶȼ������
	qep1.LineEncoder = 2500;
    qep1.MechScaler = _IQ30(0.25/qep1.LineEncoder);	//һ����ʱ������ֵ����Ļ�еȦ��
    qep1.PolePairs = P/2;
    qep1.CalibratedAngle = -2365;
    qep1.init(&qep1);			//CAP1��CAP2��ΪQEP1��QEP2����,T2��������;CAP3��Ϊ����T2������,����������

	speed1.K1 = _IQ21(1/(BASE_FREQ*T));
    speed1.K2 = _IQ(1/(1+T*2*PI*30));  	// Low-pass cut-off frequency,����Ƶ��Ϊ30HZ
    speed1.K3 = _IQ(1)-speed1.K2;
    speed1.BaseRpm = 120*(BASE_FREQ/P);	//��ͬ��ת��

	//��ʼ��PID���������� 
	pid1_id.Kp = _IQ(0.75);  
	pid1_id.Ki = _IQ(T/0.0005);
	pid1_id.Kd = _IQ(0/T);
	pid1_id.Kc = _IQ(0.2);
    pid1_id.OutMax = _IQ(0.30);
    pid1_id.OutMin = _IQ(-0.30);
 
	pid1_iq.Kp = _IQ(0.75);
	pid1_iq.Ki = _IQ(T/0.0005);
	pid1_iq.Kd = _IQ(0/T);
	pid1_iq.Kc = _IQ(0.2);
    pid1_iq.OutMax = _IQ(0.95);
    pid1_iq.OutMin = _IQ(-0.95);    

    pid1_spd.Kp = _IQ(1);                  
	pid1_spd.Ki = _IQ(T*SpeedLoopPrescaler/0.3);
	pid1_spd.Kd = _IQ(0/(T*SpeedLoopPrescaler));
 	pid1_spd.Kc = _IQ(0.2);
    pid1_spd.OutMax = _IQ(1);
    pid1_spd.OutMin = _IQ(-1); 
	
	//��ʼ��PWM��ض���
	//���������ǰ��ʱ�����ڽ���ֹͣ��150MHZ;��������(�Գ�PWM)
	//ÿһ�Զ���ǰ�߸���Ч�����ߵ���Ч
	//����ʱ��(10��8)/150M��0.533us
	//��T1���������ƥ��ʱ��CMPRx����
    pwm1.PeriodMax = SYSTEM_FREQUENCY*1000000*T/2;  //������������ڵ�CPUʱ��������
    pwm1.init(&pwm1);
	
	//���������ǰ��ʱ�����ڽ���ֹͣ��150MHZ;��������(�Գ�PWM)
	//ÿһ�Զ���ǰ�ߵ���Ч�����߸���Ч
	//����ʱ��0
	//����T1����ʱ��CMPRx����
	pwmdac1.PeriodMax = (SYSTEM_FREQUENCY*200/(30*2))*5;   // PWMDAC Frequency = 30 kHz
    pwmdac1.PwmDacInPointer0 = &PwmDacCh1;
    pwmdac1.PwmDacInPointer1 = &PwmDacCh2;
    pwmdac1.PwmDacInPointer2 = &PwmDacCh3;
	pwmdac1.init(&pwmdac1); 
	
	//��ʼ������ģ��
	dlog.iptr1 = &DlogCh1;
    dlog.iptr2 = &DlogCh2;
    dlog.iptr3 = &DlogCh3;
    dlog.iptr4 = &DlogCh4;
    dlog.trig_value = 0x1;
    dlog.size = 0x400;
    dlog.prescalar = 1;
    dlog.init(&dlog);
	
	rc1.RampDelayMax=40;				//���ٵĿ���
	rg1.StepAngleMax = _IQ(BASE_FREQ*T);//���ȫ��(�ٶȱ���ֵΪ1)������һ��PWM��������ת�ų�ת���ĵ�Ƕ�(����ֵ)
//====��������Ĳ������ü�Ӳ����ʼ���������=============

	//ͨ��ĳһ��ѹʸ��0.5S��ʹת�Ӵų��Ͷ��Ӵų���׼����λ��ʼת��λ��
	
    angle=-0.25;
	ipark1.Ds = _IQ(VdTesting);
    ipark1.Qs = _IQ(VqTesting);	
    ipark1.Angle=_IQ(angle);
    ipark1.calc(&ipark1);
	
	svgen_dq1.Ualpha = ipark1.Alpha;
 	svgen_dq1.Ubeta = ipark1.Beta;
  	svgen_dq1.calc(&svgen_dq1);	
	
	pwm1.MfuncC1 = (int16)_IQtoIQ15(svgen_dq1.Ta); // MfuncC1 is in Q15
    pwm1.MfuncC2 = (int16)_IQtoIQ15(svgen_dq1.Tb); // MfuncC2 is in Q15  
    pwm1.MfuncC3 = (int16)_IQtoIQ15(svgen_dq1.Tc); // MfuncC3 is in Q15
	pwm1.update(&pwm1);
	
	DELAY_US(10000);
	
	
	//ʹ��ȫ���жϡ�INTMʵʱ�ж�DBGM
	EINT;
	ERTM;

	for(;;) 
	{
		BackTicker++;	//��ѭ��

		//=======���������ٰ���========
		if (GpioDataRegs.GPBDAT.bit.GPIOB6==0)	//s4����Ϊ0,����
		{
			//ȥ��
			Delay_Us(2000,1000);
			SpeedRef+=0.01;
			if (SpeedRef>0.99) SpeedRef=0.99;
		}
		if (GpioDataRegs.GPBDAT.bit.GPIOB7==0)	//s12����Ϊ0,����
		{
			//ȥ��
			Delay_Us(2000,1000);
			SpeedRef-=0.01;
			if (SpeedRef<0.01) SpeedRef=0.01;
		}
		//=======�����������ذ���========
		if (GpioDataRegs.GPBDAT.bit.GPIOB8==0)	//s5����Ϊ0,��������
		{
			//ȥ��
			Delay_Us(2000,1000);
			VqTesting+=0.01;
			if (VqTesting>0.8) VqTesting=0.8;
		}
		if (GpioDataRegs.GPEDAT.bit.GPIOE0==0)	//s13����Ϊ0,��������
		{
			//ȥ��
			Delay_Us(2000,1000);
			VqTesting-=0.01;
			if (VqTesting<0.01) VqTesting=0.01;
		}
	}
}




//======================�жϺ�������=====================
interrupt void MainISR(void)
{

	// Verifying the ISR
    IsrTicker++;

	#if (BUILDLEVEL==LEVEL1)
	//***************** LEVEL1 *****************
	
	// ------------------------------------------------------------------------------
	// 	����RampDelayMax(40)��PWM���ڵ���һ��SetPointValue��˲ʱ�ٶȱ���ֵ)
	// 	ֱ��SetPointValue�ӽ�SpeedRef,ģ��������   
	// ------------------------------------------------------------------------------
    rc1.TargetValue = _IQ(SpeedRef);
    rc1.calc(&rc1);

	// ------------------------------------------------------------------------------
	// 	�������ڴų��ĵ�Ƕ�
	// 
	// ------------------------------------------------------------------------------
    rg1.Freq = rc1.SetpointValue;
    rg1.calc(&rg1);

	// ------------------------------------------------------------------------------
	//   ipark��������г�Ա����_iq��ʽ
	//    
	// ------------------------------------------------------------------------------
    ipark1.Ds = _IQ(VdTesting);	//VdTesting��VqTestingΪ����ֵ
    ipark1.Qs = _IQ(VqTesting);	
    ipark1.Angle = rg1.Out;
    ipark1.calc(&ipark1);

	// ------------------------------------------------------------------------------
	//   svgen_dq1��������г�Ա����_iq��ʽ 
	//    
	// ------------------------------------------------------------------------------
  	svgen_dq1.Ualpha = ipark1.Alpha;
 	svgen_dq1.Ubeta = ipark1.Beta;
  	svgen_dq1.calc(&svgen_dq1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWM_DRV module and call the PWM signal generation 
	//    update function.
	// ------------------------------------------------------------------------------
    pwm1.MfuncC1 = (int16)_IQtoIQ15(svgen_dq1.Ta); // MfuncC1 is in Q15
    pwm1.MfuncC2 = (int16)_IQtoIQ15(svgen_dq1.Tb); // MfuncC2 is in Q15  
    pwm1.MfuncC3 = (int16)_IQtoIQ15(svgen_dq1.Tc); // MfuncC3 is in Q15
	pwm1.update(&pwm1);

	// ------------------------------------------------------------------------------
	//    Call the QEP calculation function 
	// ------------------------------------------------------------------------------
	qep1.calc(&qep1);	//�����ǶȺͷ���

	// ------------------------------------------------------------------------------
	//    Connect inputs of the SPEED_FR module and call the speed calculation function 
	// ------------------------------------------------------------------------------
	speed1.ElecTheta = _IQ15toIQ((int32)qep1.ElecTheta);
	speed1.DirectionQep = (int32)(qep1.DirectionQep);
	speed1.calc(&speed1);

	if (SpeedLoopCount==SpeedLoopPrescaler)
	 {
		SCI_1=speed1.Speed;
		SCI_2=speed1.Speed>>8;
		SCI_3=speed1.Speed>>16;
		SCI_4=speed1.Speed>>24;
		
		//ScibRegs.SCITXBUF=0x88;			//�����������ǣ�����	
     	ScibRegs.SCICTL2.bit.TXINTENA=0x1;	//�������ж�				
	  	ScibRegs.SCITXBUF=0xFF;
		SpeedLoopCount=1;
	 }
	else SpeedLoopCount++; 
	
	
	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWMDAC module 
	// ------------------------------------------------------------------------------	
    PwmDacCh1 = (int16)_IQtoIQ15(svgen_dq1.Ta);	//��Ta��Tb��Tc��ֵ�浽DACͨ������
    PwmDacCh2 = (int16)_IQtoIQ15(svgen_dq1.Tb);    
    PwmDacCh3 = (int16)_IQtoIQ15(svgen_dq1.Tc);    
	pwmdac1.update(&pwmdac1); 

	// ------------------------------------------------------------------------------
	//    Connect inputs of the DATALOG module 
	// ------------------------------------------------------------------------------
    DlogCh1 = (int16)_IQtoIQ15(svgen_dq1.Ta);	//��Ta��Tb��Tc��ֵ������
    DlogCh2 = (int16)_IQtoIQ15(svgen_dq1.Tb);
    DlogCh3 = (int16)_IQtoIQ15(svgen_dq1.Tc);
    DlogCh4 = (int16)_IQtoIQ15(speed1.Speed);
	dlog.update(&dlog);	
	// ************* END LEVEL1 *****************/
	#endif

	#if (BUILDLEVEL==LEVEL2)
	//***************** LEVEL2 *****************
	
	// ------------------------------------------------------------------------------
	//    Connect inputs of the RMP module and call the Ramp control
	//    calculation function.
	// ------------------------------------------------------------------------------
    rc1.TargetValue = _IQ(SpeedRef);
    rc1.calc(&rc1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the RAMP GEN module and call the Ramp generator
	//    calculation function.
	// ------------------------------------------------------------------------------
    rg1.Freq = rc1.SetpointValue;
    rg1.calc(&rg1);

	// ------------------------------------------------------------------------------
	//    Call the ILEG2_VDC read function.
	// ------------------------------------------------------------------------------
    ilg2_vdc1.read(&ilg2_vdc1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the CLARKE module and call the clarke transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
 	clarke1.As = _IQ15toIQ((int32)ilg2_vdc1.ImeasA);
  	clarke1.Bs = _IQ15toIQ((int32)ilg2_vdc1.ImeasB);
	clarke1.calc(&clarke1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PARK module and call the park transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
    park1.Alpha = clarke1.Alpha;
    park1.Beta = clarke1.Beta;
    park1.Angle = rg1.Out;
    park1.calc(&park1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the INV_PARK module and call the inverse park transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
    ipark1.Ds = _IQ(VdTesting);
    ipark1.Qs = _IQ(VqTesting);	
    ipark1.Angle = rg1.Out;
    ipark1.calc(&ipark1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the SVGEN_DQ module and call the space-vector gen.
	//    calculation function.
	// ------------------------------------------------------------------------------
  	svgen_dq1.Ualpha = ipark1.Alpha;
 	svgen_dq1.Ubeta = ipark1.Beta;
  	svgen_dq1.calc(&svgen_dq1);	

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWM_DRV module and call the PWM signal generation 
	//    update function.
	// ------------------------------------------------------------------------------
    pwm1.MfuncC1 = (int16)_IQtoIQ15(svgen_dq1.Ta); // MfuncC1 is in Q15
    pwm1.MfuncC2 = (int16)_IQtoIQ15(svgen_dq1.Tb); // MfuncC2 is in Q15  
    pwm1.MfuncC3 = (int16)_IQtoIQ15(svgen_dq1.Tc); // MfuncC3 is in Q15
	pwm1.update(&pwm1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWMDAC module 
	// ------------------------------------------------------------------------------	
    PwmDacCh1 = (int16)_IQtoIQ15(svgen_dq1.Ta);
    PwmDacCh2 = (int16)_IQtoIQ15(clarke1.As);
    PwmDacCh3 = (int16)_IQtoIQ15(rg1.Out);
	pwmdac1.update(&pwmdac1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the DATALOG module 
	// ------------------------------------------------------------------------------
    DlogCh1 = (int16)_IQtoIQ15(svgen_dq1.Ta);
    DlogCh2 = ilg2_vdc1.ImeasC;
    DlogCh3 = ilg2_vdc1.ImeasA;
    DlogCh4 = ilg2_vdc1.ImeasB;
	dlog.update(&dlog);
	// ************* END LEVEL2 *****************
	#endif

	#if (BUILDLEVEL==LEVEL3)
	//****************** LEVEL3 *****************
	
	// ------------------------------------------------------------------------------
	//    Connect inputs of the RMP module and call the Ramp control
	//    calculation function.
	// ------------------------------------------------------------------------------
    rc1.TargetValue = _IQ(SpeedRef);
    rc1.calc(&rc1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the RAMP GEN module and call the Ramp generator
	//    calculation function.
	// ------------------------------------------------------------------------------
    rg1.Freq = rc1.SetpointValue;
    rg1.calc(&rg1);

	// ------------------------------------------------------------------------------
	//    Call the ILEG2_VDC read function.
	// ------------------------------------------------------------------------------
    ilg2_vdc1.read(&ilg2_vdc1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the CLARKE module and call the clarke transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
	clarke1.As = _IQ15toIQ((int32)ilg2_vdc1.ImeasA);
	clarke1.Bs = _IQ15toIQ((int32)ilg2_vdc1.ImeasB);
	clarke1.calc(&clarke1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PARK module and call the park transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
	park1.Alpha = clarke1.Alpha;
	park1.Beta = clarke1.Beta;
	park1.Angle = rg1.Out;
	park1.calc(&park1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PID_REG3 module and call the PID IQ controller
	//    calculation function.
	// ------------------------------------------------------------------------------  
	pid1_iq.Ref = _IQ(IqRef);
	pid1_iq.Fdb = park1.Qs;
	pid1_iq.calc(&pid1_iq);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PID_REG3 module and call the PID ID controller
	//    calculation function.
	// ------------------------------------------------------------------------------  
	pid1_id.Ref = _IQ(IdRef);
	pid1_id.Fdb = park1.Ds;
	pid1_id.calc(&pid1_id);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the INV_PARK module and call the inverse park transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
	ipark1.Ds = pid1_id.Out;
	ipark1.Qs = pid1_iq.Out;	
	ipark1.Angle = rg1.Out;
	ipark1.calc(&ipark1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the SVGEN_DQ module and call the space-vector gen.
	//    calculation function.
	// ------------------------------------------------------------------------------
	svgen_dq1.Ualpha = ipark1.Alpha;
	svgen_dq1.Ubeta = ipark1.Beta;
	svgen_dq1.calc(&svgen_dq1);	

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWM_DRV module and call the PWM signal generation 
	//    update function.
	// ------------------------------------------------------------------------------
	pwm1.MfuncC1 = (int16)_IQtoIQ15(svgen_dq1.Ta); // MfuncC1 is in Q15
	pwm1.MfuncC2 = (int16)_IQtoIQ15(svgen_dq1.Tb); // MfuncC2 is in Q15  
	pwm1.MfuncC3 = (int16)_IQtoIQ15(svgen_dq1.Tc); // MfuncC3 is in Q15
	pwm1.update(&pwm1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWMDAC module 
	// ------------------------------------------------------------------------------	
	PwmDacCh1 = (int16)_IQtoIQ15(clarke1.As);
	PwmDacCh2 = (int16)_IQtoIQ15(rg1.Out);    
	PwmDacCh3 = (int16)_IQtoIQ15(svgen_dq1.Ta);    
	pwmdac1.update(&pwmdac1);
	
	// ------------------------------------------------------------------------------
	//    Connect inputs of the DATALOG module 
	// ------------------------------------------------------------------------------
	DlogCh1 = (int16)_IQtoIQ15(svgen_dq1.Ta);
	DlogCh2 = (int16)_IQtoIQ15(svgen_dq1.Tb);
	DlogCh3 = (int16)_IQtoIQ15(rg1.Out);
	DlogCh4 = (int16)_IQtoIQ15(clarke1.As);
	dlog.update(&dlog);
	// ************* END LEVEL3 *****************/
	#endif
	
	#if (BUILDLEVEL==LEVEL4)
	// ***************** LEVEL4 *****************
	
	// ------------------------------------------------------------------------------
	//    Connect inputs of the RMP module and call the Ramp control
	//    calculation function.
	// ------------------------------------------------------------------------------
	rc1.TargetValue = _IQ(SpeedRef);
	rc1.calc(&rc1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the RAMP GEN module and call the Ramp generator
	//    calculation function.
	// ------------------------------------------------------------------------------
	rg1.Freq = rc1.SetpointValue;
	rg1.calc(&rg1);

	// ------------------------------------------------------------------------------
	//    Call the ILEG2_VDC read function.
	// ------------------------------------------------------------------------------
	ilg2_vdc1.read(&ilg2_vdc1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the CLARKE module and call the clarke transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
	clarke1.As = _IQ15toIQ((int32)ilg2_vdc1.ImeasA);
		clarke1.Bs = _IQ15toIQ((int32)ilg2_vdc1.ImeasB);
	clarke1.calc(&clarke1);
	   
	// -----------------------------------------------------------------------------
	// Checking LockRotorFlag=FALSE for spinning mode, LockRotorFlag=TRUE for locked rotor mode 
	// -----------------------------------------------------------------------------
	if(LockRotorFlag==TRUE)
	{
	 	// locked rotor mode if LockRotorFlag = 1

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PARK module and call the park transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
	   park1.Alpha = clarke1.Alpha;
	   park1.Beta = clarke1.Beta;
	   park1.Angle = 0;
	   park1.calc(&park1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PID_REG3 module and call the PID IQ controller
	//    calculation function.
	// ------------------------------------------------------------------------------  
	   pid1_iq.Ref = _IQ(IqRef);
	   pid1_iq.Fdb = park1.Qs;
	   pid1_iq.calc(&pid1_iq);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PID_REG3 module and call the PID ID controller
	//    calculation function.
	// ------------------------------------------------------------------------------  
	   pid1_id.Ref = _IQ(IdRef);
	   pid1_id.Fdb = park1.Ds;
	   pid1_id.calc(&pid1_id);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the INV_PARK module and call the inverse park transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
	   ipark1.Ds = pid1_id.Out;
	   ipark1.Qs = pid1_iq.Out;	
	   ipark1.Angle = 0;
	   ipark1.calc(&ipark1);

	}   // End: LockRotorFlag==TRUE

	else if(LockRotorFlag==FALSE)
	{   // spinning mode if LockRotorFlag = 0

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PARK module and call the park transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
	   park1.Alpha = clarke1.Alpha;
	   park1.Beta = clarke1.Beta;
	   park1.Angle = rg1.Out;
	   park1.calc(&park1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PID_REG3 module and call the PID IQ controller
	//    calculation function.
	// ------------------------------------------------------------------------------  
	   pid1_iq.Ref = _IQ(IqRef);
	   pid1_iq.Fdb = park1.Qs;
	   pid1_iq.calc(&pid1_iq);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PID_REG3 module and call the PID ID controller
	//    calculation function.
	// ------------------------------------------------------------------------------  
	   pid1_id.Ref = _IQ(IdRef);
	   pid1_id.Fdb = park1.Ds;
	   pid1_id.calc(&pid1_id);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the INV_PARK module and call the inverse park transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
	   ipark1.Ds = pid1_id.Out;
	   ipark1.Qs = pid1_iq.Out;	
	   ipark1.Angle = rg1.Out;
	   ipark1.calc(&ipark1);

	}   // End: LockRotorFlag==FALSE

	// ------------------------------------------------------------------------------
	//    Connect inputs of the SVGEN_DQ module and call the space-vector gen.
	//    calculation function.
	// ------------------------------------------------------------------------------
	svgen_dq1.Ualpha = ipark1.Alpha;
	svgen_dq1.Ubeta = ipark1.Beta;
	svgen_dq1.calc(&svgen_dq1);	

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWM_DRV module and call the PWM signal generation 
	//    update function.
	// ------------------------------------------------------------------------------
	pwm1.MfuncC1 = (int16)_IQtoIQ15(svgen_dq1.Ta); // MfuncC1 is in Q15
	pwm1.MfuncC2 = (int16)_IQtoIQ15(svgen_dq1.Tb); // MfuncC2 is in Q15  
	pwm1.MfuncC3 = (int16)_IQtoIQ15(svgen_dq1.Tc); // MfuncC3 is in Q15
	pwm1.update(&pwm1);

	// ------------------------------------------------------------------------------
	//    Call the QEP calculation function 
	// ------------------------------------------------------------------------------
	qep1.calc(&qep1);	//�����ǶȺͷ���

	// ------------------------------------------------------------------------------
	//    Connect inputs of the SPEED_FR module and call the speed calculation function 
	// ------------------------------------------------------------------------------
	speed1.ElecTheta = _IQ15toIQ((int32)qep1.ElecTheta);
	speed1.DirectionQep = (int32)(qep1.DirectionQep);
	speed1.calc(&speed1);


	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWMDAC module 
	// ------------------------------------------------------------------------------	
	PwmDacCh1 = (int16)_IQtoIQ15(svgen_dq1.Ta);
	PwmDacCh2 = (int16)_IQtoIQ15(rg1.Out);    
	PwmDacCh3 = (int16)_IQtoIQ15(speed1.ElecTheta);    
	pwmdac1.update(&pwmdac1); 

	// ------------------------------------------------------------------------------
	//    Connect inputs of the DATALOG module 
	// ------------------------------------------------------------------------------
	DlogCh1 = (int16)_IQtoIQ15(svgen_dq1.Ta);
	DlogCh2 = (int16)_IQtoIQ15(clarke1.As);
	DlogCh3 = (int16)_IQtoIQ15(speed1.ElecTheta);
	DlogCh4 = (int16)_IQtoIQ15(rg1.Out);
	dlog.update(&dlog);
	// ************* END LEVEL4 *****************
	#endif

	#if (BUILDLEVEL==LEVEL5)
	//**************** LEVEL5 *****************

	// ------------------------------------------------------------------------------
	//    Call the ILEG2_VDC read function.
	// ------------------------------------------------------------------------------
	ilg2_vdc1.read(&ilg2_vdc1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the CLARKE module and call the clarke transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
		clarke1.As = _IQ15toIQ((int32)ilg2_vdc1.ImeasA);
		clarke1.Bs = _IQ15toIQ((int32)ilg2_vdc1.ImeasB);
	clarke1.calc(&clarke1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PARK module and call the park transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
	park1.Alpha = clarke1.Alpha;
	park1.Beta = clarke1.Beta;
	park1.Angle = speed1.ElecTheta;
	park1.calc(&park1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PID_REG3 module and call the PID speed controller
	//    calculation function.
	// ------------------------------------------------------------------------------  
	if (SpeedLoopCount==SpeedLoopPrescaler)
	 {
	  pid1_spd.Ref = _IQ(SpeedRef);
	  pid1_spd.Fdb = speed1.Speed;
	  pid1_spd.calc(&pid1_spd);
	  SpeedLoopCount=1;
	 }
	else SpeedLoopCount++; 

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PID_REG3 module and call the PID IQ controller
	//    calculation function.
	// ------------------------------------------------------------------------------  
	pid1_iq.Ref = pid1_spd.Out;
	pid1_iq.Fdb = park1.Qs;
	pid1_iq.calc(&pid1_iq);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PID_REG3 module and call the PID ID controller
	//    calculation function.
	// ------------------------------------------------------------------------------  
	pid1_id.Ref = _IQ(IdRef);
	pid1_id.Fdb = park1.Ds;
	pid1_id.calc(&pid1_id);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the INV_PARK module and call the inverse park transformation
	//    calculation function.
	// ------------------------------------------------------------------------------
	ipark1.Ds = pid1_id.Out;
	ipark1.Qs = pid1_iq.Out;	
	ipark1.Angle = speed1.ElecTheta;
	ipark1.calc(&ipark1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the SVGEN_DQ module and call the space-vector gen.
	//    calculation function.
	// ------------------------------------------------------------------------------
	svgen_dq1.Ualpha = ipark1.Alpha;
	svgen_dq1.Ubeta = ipark1.Beta;
	svgen_dq1.calc(&svgen_dq1);	

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWM_DRV module and call the PWM signal generation 
	//    update function.
	// ------------------------------------------------------------------------------
	pwm1.MfuncC1 = (int16)_IQtoIQ15(svgen_dq1.Ta); // MfuncC1 is in Q15
	pwm1.MfuncC2 = (int16)_IQtoIQ15(svgen_dq1.Tb); // MfuncC2 is in Q15  
	pwm1.MfuncC3 = (int16)_IQtoIQ15(svgen_dq1.Tc); // MfuncC3 is in Q15
	pwm1.update(&pwm1);

	// ------------------------------------------------------------------------------
	//    Call the QEP calculation function 
	// ------------------------------------------------------------------------------
	qep1.calc(&qep1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the SPEED_FR module and call the speed calculation function 
	// ------------------------------------------------------------------------------

	speed1.ElecTheta = _IQ15toIQ((int32)qep1.ElecTheta);
	speed1.DirectionQep = (int32)(qep1.DirectionQep);
	speed1.calc(&speed1);


	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWMDAC module 
	// ------------------------------------------------------------------------------	
	PwmDacCh1 = (int16)_IQtoIQ15(svgen_dq1.Ta);
	PwmDacCh2 = (int16)_IQtoIQ15(speed1.ElecTheta);    
	PwmDacCh3 = (int16)_IQtoIQ15(clarke1.As);    
	pwmdac1.update(&pwmdac1);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the DATALOG module 
	// ------------------------------------------------------------------------------
	DlogCh1 = (int16)_IQtoIQ15(svgen_dq1.Ta);
	DlogCh2 = (int16)_IQtoIQ15(speed1.ElecTheta);
	DlogCh3 = (int16)_IQtoIQ15(pid1_spd.Ref);
	DlogCh4 = (int16)_IQtoIQ15(speed1.speed);
	dlog.update(&dlog);
	// ************* END LEVEL5 *****************/ 
	#endif
	
	//������ͨT1UFINT�ж�
	// Enable more interrupts from this timer
	EvaRegs.EVAIMRA.bit.T1UFINT = 1;
	// Note: To be safe, use a mask value to write to the entire
	// EVAIFRC register.  Writing to one bit will cause a read-modify-write
	// operation that may have the result of writing 1's to clear 
	// bits other then those intended.
	EvaRegs.EVAIFRA.all = BIT9; //
	// Acknowledge interrupt to recieve more interrupts from PIE group 3
	PieCtrlRegs.PIEACK.all |= PIEACK_GROUP2;

}


//******************************************************
void Delay_Us(Uint16 Counter1,Uint16 Counter2)
{
	Uint16 i,j;
	for (i=0;i<Counter1;i++)
		{
			for (j=0;j<Counter2;j++){}
		}
}

//*********************************************************
interrupt void QepISR(void)
{
	
	rcounter++;
	// ------------------------------------------------------------------------------
	//    Call the QEP_DRV isr function.
	// ------------------------------------------------------------------------------
	qep1.isr(&qep1);

	// Enable more interrupts from this timer
	EvaRegs.EVAIMRC.bit.CAP3INT = 1;

	// Note: To be safe, use a mask value to write to the entire
	// EVAIFRC register.  Writing to one bit will cause a read-modify-write
	// operation that may have the result of writing 1's to clear 
	// bits other then those intended. 
	EvaRegs.EVAIFRC.all = BIT2;

	// Acknowledge interrupt to recieve more interrupts from PIE group 3
	PieCtrlRegs.PIEACK.all |= PIEACK_GROUP3;

}


interrupt void SCIB_IX_ISR(void)
{
	static int i=0;
	i++;

	if (i==1)
	ScibRegs.SCITXBUF=SCI_4;
	else if (i==2)
	ScibRegs.SCITXBUF=SCI_3;
	else if (i==3)
	ScibRegs.SCITXBUF=SCI_2;
	else if (i==4)
	{
	ScibRegs.SCITXBUF=SCI_1;
	i=0;
	ScibRegs.SCICTL2.bit.TXINTENA=0x0;
	}

}



//===========================================================================
// No more.
//===========================================================================
