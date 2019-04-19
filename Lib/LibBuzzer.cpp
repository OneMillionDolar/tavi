//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Buzzer Tick sound
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibBuzzer.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/04/07 10:58:50 $
// $Author: etnlwind $
//
//*****************************************************************************
// $Log: LibBuzzer.cpp,v $
// Revision 1.2  2006/04/07 10:58:50  etnlwind
// Add instruction for port alt.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.7  2006/02/04 05:58:38  ywkim
// Change port of BuzzerTick to PIO0_0 from PIO2_13 for HW0602
//
// Revision 1.6  2005/05/06 06:15:33  ywkim
// add header
//
// Revision 1.5  2005/05/04 02:24:30  ywkim
// Change buzzertick()
//
// Revision 1.4  2005/05/03 12:56:31  etnlwind
// Update for ES4
//
// Revision 1.3  2005/05/03 11:39:43  ywkim
// use gpio for buzzer
//
// Revision 1.2  2005/04/20 11:36:04  zstein
// *** empty log message ***
//
// Revision 1.1  2005/03/29 03:26:02  ywkim
// Initial revision for buzzer tick sound
//
//*****************************************************************************

#include <unistd.h>
#include <Task/tavi_global.h>
#include "../include/define_fs.h"

#define PWM0_CONTROL			*(volatile int *)(0x00501c10)
#define UART1_ALTFUNCTION		*(volatile int *)(0x00501330)

#define JASPER_PBI_SLAVE_BASE	0x008009C0
#define JASPER_PIO0_BASE		0x00500600
#define JASPER_PIO2_BASE		0x00501B00

#define PBI_SL_CTRL				0x00
#define PBI_SL_CTRL_REG			*(volatile unsigned long *)(JASPER_PBI_SLAVE_BASE + PBI_SL_CTRL)
#define PBI_MODE_LBC_PADS		0x0
#define PBI_MODE_IDE_PADS		0x8
#define pPIO0_DIR                   		*( volatile int* )( JASPER_PIO0_BASE + 0x08 )
#define pPIO0_DATA                  	*( volatile int* )( JASPER_PIO0_BASE + 0x04 )
#define pPIO2_DIR                   		*( volatile int* )( JASPER_PIO2_BASE + 0x08 )
#define pPIO2_DATA                  	*( volatile int* )( JASPER_PIO2_BASE + 0x04 )
#define MK_REG( _bit, _level ) 		( (0x10000<<_bit) |(_level<<_bit) ) 

extern SysParameter_t	TaviSysParam;

void BuzzerTick(void)
{
	int	i;
	int	j;
	
	if (TaviSysParam.etc.buzzer)
		return;
#ifdef HW0601		// HW0601
#ifdef HW0602	// HW0602
	PBI_SL_CTRL_REG = PBI_MODE_LBC_PADS;
	pPIO0_DIR = MK_REG(0,1);
	for(j=0;j<2;j++)
	{
		pPIO0_DATA = MK_REG(0,1);
		for(i=0;i<3000;i++);
		pPIO0_DATA = MK_REG(0,0);
		for(i=0;i<3000;i++);

		pPIO0_DATA = MK_REG(0,1);
		for(i=0;i<1000;i++);
		pPIO0_DATA = MK_REG(0,0);
		for(i=0;i<1000;i++);

		pPIO0_DATA = MK_REG(0,1);
		for(i=0;i<2000;i++);
		pPIO0_DATA = MK_REG(0,0);
		for(i=0;i<2000;i++);
	}
	pPIO0_DIR = MK_REG(0,0);
	PBI_SL_CTRL_REG = PBI_MODE_IDE_PADS;
#else			// HW0602
	#if USE_PBI_DRV
	PBI_SL_CTRL_REG = PBI_MODE_LBC_PADS;

	pPIO2_DIR = MK_REG(13,1);
	for(j=0;j<2;j++)
	{
		pPIO2_DATA = MK_REG(13,1);
		for(i=0;i<3000;i++);
		pPIO2_DATA = MK_REG(13,0);
		for(i=0;i<3000;i++);

		pPIO2_DATA = MK_REG(13,1);
		for(i=0;i<1000;i++);
		pPIO2_DATA = MK_REG(13,0);
		for(i=0;i<1000;i++);

		pPIO2_DATA = MK_REG(13,1);
		for(i=0;i<2000;i++);
		pPIO2_DATA = MK_REG(13,0);
		for(i=0;i<2000;i++);
	}
	PBI_SL_CTRL_REG = PBI_MODE_IDE_PADS;
	#endif
#endif			// HW0602
#else				// HW0601

	#if USE_PBI_DRV
	PBI_SL_CTRL_REG = PBI_MODE_LBC_PADS;

	pPIO2_DIR = MK_REG(13,1);
	for(j=0;j<2;j++)
	{
		pPIO2_DATA = MK_REG(13,1);
		for(i=0;i<3000;i++);
		pPIO2_DATA = MK_REG(13,0);
		for(i=0;i<3000;i++);

		pPIO2_DATA = MK_REG(13,1);
		for(i=0;i<1000;i++);
		pPIO2_DATA = MK_REG(13,0);
		for(i=0;i<1000;i++);

		pPIO2_DATA = MK_REG(13,1);
		for(i=0;i<2000;i++);
		pPIO2_DATA = MK_REG(13,0);
		for(i=0;i<2000;i++);
	}
	PBI_SL_CTRL_REG = PBI_MODE_IDE_PADS;
	#endif

#endif				// HW0601
}
