/* 
 * Scroll Strip(Synaptics TM61PDZG123) driver for TAVI
 *
 * Copyright(C) 2004 New Media Life Inc.
 *
 * Author: Lee Seok(zstein@newmedialife.com>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/wait.h>

#include <asm/arch/hardware.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include "tavi_tp.h"
#include "meplib.h"
#include "../misc_group.h"

#define MEP_CLK				7
#define MEP_DATA			6
#define MEP_ACK				8

#define REG_WORD( __addr )	*( (volatile unsigned long*)( __addr ) )

#define MEP_DATA_DIR_IN	( *pPIO0_DIR = MK_REG( MEP_DATA, 0 ) )
#define MEP_DATA_DIR_OUT	( *pPIO0_DIR = MK_REG( MEP_DATA, 1 ) )

#define MEP_DATA_HIGH		( *pPIO0_DATA = MK_REG( MEP_DATA, 1 ) )
#define MEP_DATA_LOW		( *pPIO0_DATA = MK_REG( MEP_DATA, 0 ) )
#define MEP_ACK_HIGH		( *pPIO0_DATA = MK_REG( MEP_ACK, 1 ) )
#define MEP_ACK_LOW		( *pPIO0_DATA = MK_REG( MEP_ACK, 0 ) )


#define P0_STATE			0
#define P1_STATE			1
#define P2_STATE			2
#define P3_STATE			3

#define LOW					0
#define HIGH				1

#define DEBUG
#ifdef DEBUG
#define DEBUGMSG			printk
#else
#define DEBUGMSG
#endif

#define DELAY_TIME			20
#define LOOP_TIMEOUT		200

static struct mepdev_struct {
	int irq;
	int busy;
	int ready;
	char txQ[8];
	char rxQ[8];
	int   len;
	int txdone;
	int rxdone;
	wait_queue_head_t mepwq;
	wait_queue_head_t wq;
	struct touchpad_phy phy;
	int( *isr_callback )( void* arg );
} dev;

static int mep_pl_getDATA( void ) 
{
	int data; 

	MEP_DATA_DIR_IN;
	data = *pPIO0_DATA;
	if( data & (1<<MEP_DATA) ) return 1;
	else return 0;
}

static void mep_pl_setDATA( int level )
{
	if( level ) {
		MEP_DATA_DIR_IN;
	}
	else {
		MEP_DATA_LOW;
		MEP_DATA_DIR_OUT;
	}
}

static int mep_pl_getCLK( void )
{
	int data = *pPIO0_DATA & ( 1<<MEP_CLK );
	if( data ) return 1;
	else return 0;
}

static void mep_pl_setACK( int level )
{
	if( level ) {
		MEP_ACK_HIGH;
	}
	else {
		MEP_ACK_LOW;
	}
}

static int mep_pl_getACK( void )
{
	return( (*pPIO0_DATA&(1<<MEP_ACK)) != 0 );
}

static int wait_get_clk( struct mepdev_struct* mepdev, int level )
{
	int i=0;

	for( i=0; i<LOOP_TIMEOUT; i++ ) {
		if( mepdev->phy.get_clk() == level ) break;
		udelay( 1 );
	}

	if(i == LOOP_TIMEOUT) {
		//printk( "hmmm...(%d)\n", level );
		return 0;
	}

	return 1;
}

static int is_idle( struct mepdev_struct* mepdev )
{
	if( mepdev->phy.get_clk() && mepdev->phy.get_data() ) {
		return 1;
	}
	else {
		return 0;
	}
}

static int is_P1_state( struct mepdev_struct* mepdev ) 
{
	return( !mepdev->phy.get_clk() && mepdev->phy.get_ack() );
}

static int is_P3_state( struct mepdev_struct* mepdev )
{
	return( mepdev->phy.get_clk() && !mepdev->phy.get_ack() );
}

static void flushee( struct mepdev_struct* mepdev )
{
	if( is_P1_state( mepdev ) ) {
		mepdev->phy.set_data( HIGH );

		if( mepdev->phy.get_data() && is_P3_state( mepdev ) ) {
		}
	}

	if( is_idle( mepdev ) == 0 ) {
		printk( "flushee: not idle...\n" );
	}
}

static void mep_flusher( struct mepdev_struct* mepdev )
{
	int i;

	/* The flusher holds DATA low for 32 handshake cycles.
	 * and then sends HELLO packet. After sending HELLO packet, the link
	 * gets into idle state.
 	 */
	for( i=0; i<72; i++ ) {
		mepdev->phy.set_ack( mepdev->phy.get_clk() );
		udelay( DELAY_TIME );
	}
}

static int mep_SendPacket( struct mepdev_struct* mepdev )
{
	char bit, clk;
	int parity=0;
	int error = 0;
	int data, i;

	if( mepdev->txQ[0]&0x08 ) mepdev->len = 1;
	else mepdev->len = GET_PACKET_LENGTH( mepdev->txQ )+1;
	mepdev->txdone = 0;

	// Host RTS
	mepdev->phy.set_data( LOW );
	wait_get_clk( mepdev, LOW ); // P1
	mepdev->phy.set_ack( LOW );  // P2
	udelay( DELAY_TIME );
	wait_get_clk( mepdev, HIGH ); // P3
	
	// transmit data
	bit = 1;
	i = 0;
	while( mepdev->len ) {
		data = mepdev->txQ[i] & bit ? 1 : 0;
		mepdev->phy.set_data( data );
		clk = mepdev->phy.get_clk();
		mepdev->phy.set_ack( clk );
		wait_get_clk( mepdev, !clk );
		parity ^= mepdev->txQ[i]&bit ? 1 : 0;
		bit <<= 1;
		if( bit == 0 ) {
			bit = 1;
			mepdev->len--;
			i++;
		}
	}

	// send parity
	mepdev->phy.set_data( parity );
	mepdev->phy.set_ack( mepdev->phy.get_clk() );
	udelay( DELAY_TIME );

	// stop
	wait_get_clk( mepdev, LOW );  // P1
	mepdev->phy.set_data( HIGH ); // release data line 
	clk = mepdev->phy.get_clk();
	mepdev->phy.set_ack( clk );  // P2
	wait_get_clk( mepdev, HIGH ); // P3
	if( mepdev->phy.get_data() == 0 ) {
		error = 1;
//		printk( "tx: stop error\n" );
	}
	clk = mepdev->phy.get_clk();
	mepdev->phy.set_ack( clk ); // idle state

	mepdev->txdone = 1;
	mepdev->len = 0;
	mepdev->isr_callback = NULL;

	return error;
}

static int mep_rx( struct mepdev_struct* mepdev )
{
	int i, j;
	int parity=0;
	int size;
	int clk;
	char header;

	mepdev->rxdone = 0;

	// clear Queue
	for( i=0; i<8; i++ ) mepdev->rxQ[i] = 0x00;

	// module RTS
	i = LOOP_TIMEOUT;
	while( i ) {
		if( mepdev->phy.get_clk() == 0 ) break;
		i--;
		udelay( 10 );
	}
	if( i == 0 ) return 0;

	mepdev->phy.set_ack( mepdev->phy.get_clk() );
	wait_get_clk( mepdev, HIGH );
	clk = mepdev->phy.get_clk();
	if( mepdev->phy.get_data() ) {
		printk( "failed to rts module\n" );
		return -1;
	}
	mepdev->phy.set_ack( clk );
	
	// get first 1byte(header)
	header = 0;
	for( i=0; i<8; i++ ) {
		clk = !clk;
		wait_get_clk( mepdev, clk );
		mepdev->rxQ[0] |= mepdev->phy.get_data() << i;
		parity ^= mepdev->phy.get_data();
		mepdev->phy.set_ack( mepdev->phy.get_clk() );
	}
	mepdev->len = GET_PACKET_LENGTH( mepdev->rxQ );

	if( mepdev->len == 0 ) { 
		// parity
		clk = !clk;
		wait_get_clk( mepdev, clk );
		if( mepdev->phy.get_data() == parity ) {
			mepdev->phy.set_ack( clk );
			udelay( DELAY_TIME );
		}
		else {
			// flusher
			printk( "mep_rx: flusher...\n" );
#if 0
			mepdev->phy.set_data( LOW );
			udelay( DELAY_TIME );
			mepdev->phy.set_ack( LOW );
			udelay( DELAY_TIME );
			mepdev->phy.set_ack( HIGH );
			udelay( DELAY_TIME );
#endif
			mep_flusher( mepdev );
			return 0;
		}

		// stop
		clk = !clk;
		wait_get_clk( mepdev, clk );
		if( mepdev->phy.get_data() ) {
			mepdev->phy.set_ack( mepdev->phy.get_clk() );
			udelay( DELAY_TIME );
		}
		else {
			printk( "mep_rx: stop???\n" );
		}
		mepdev->phy.set_ack( clk );

		return 1;
	}

	i = 0;
	j = 1;
	mepdev->rxQ[j] = 0;
	size = mepdev->len+1;
	while( mepdev->len ) {
		clk = !clk;
		wait_get_clk( mepdev, clk );
		mepdev->rxQ[j] |= mepdev->phy.get_data() << i;
		mepdev->phy.set_ack( clk );
		parity ^= mepdev->phy.get_data();
		i++; 
		if( i == 8 ) {
			i = 0;
			j++;
			mepdev->len--;
			mepdev->rxQ[j] = 0;
		}
	}

	// parity
	clk = !clk;
	wait_get_clk( mepdev, clk );
	if( mepdev->phy.get_data() == parity ) {
		mepdev->phy.set_ack( clk );
		udelay( DELAY_TIME );
	}
	else {
		// flusher
		printk( "mep_rx: flusher...\n" );
		mep_flusher( mepdev );
		return 0;
	}

	// stop
	clk = !clk;
	wait_get_clk( mepdev, clk );
	if( mepdev->phy.get_data() ) {
		mepdev->phy.set_ack( mepdev->phy.get_clk() );
		udelay( DELAY_TIME );
	}
	else {
		printk( "mep_rx: stop???\n" );
	}
	mepdev->phy.set_ack( clk );

	return size;
}


static int mep_get_hello( struct mepdev_struct* mepdev )
{
	mep_flusher( mepdev );

	if( !mep_rx( &dev ) ) {
		if( is_idle(&dev) ) return 1;
		else return 0; // failed to initialized mep link
	}
	// verify HELLO packet

	return is_hello( dev.rxQ );
}

static int mep_link_init( void )
{
	*pPIO0_DIR = MK_REG( MEP_CLK, 0 ) |
				MK_REG( MEP_ACK, 1 );
	MEP_ACK_HIGH;

	MEP_DATA_DIR_IN;

	mep_flusher( &dev );
	
	if( is_idle( &dev ) ) {
		printk( "already idle\n" );
		return 1;
	}

	if( mep_get_hello( &dev ) ) {
		printk( "tavi_tp.o: module said 'HELLO'\n" );
	}
	else {
		return 0;
	}

	return is_idle( &dev );
}

/* File operations
 */
static int ss_open( struct inode* inode, struct file* filep )
{
	if( dev.busy ) return -EBUSY;
	mep_flusher( &dev );
	dev.busy = 1;

	return 0;
}

static int ss_release( struct inode* inde, struct file* filep )
{
	dev.busy = 0;
	filep->private_data = NULL;

	return 0;
}

static int ss_read( struct file* filep, char* buf, size_t size, loff_t* offp )
{
	int rsize;

	rsize = mep_rx( &dev );
	if( rsize > 0 )
		copy_to_user( buf, dev.rxQ, size );

	return rsize;
}

static int ss_write( struct file* filep, const char* buf, size_t size, loff_t* offp )
{
	copy_from_user( dev.txQ, buf, size );
	if( mep_SendPacket( &dev ) ) {
//		printk( "tavi_tp.o: failed to SendPacket\n" );
		return -EBUSY;
	}

	return 0;
}

static int ss_ioctl( struct inode* inode, struct file* file, unsigned int cmd, 
		unsigned long arg )
{
	int data;
	struct mepdev_struct* mepdev = &dev;

	switch( cmd ) {
	case TAVI_IOCSACK:
		if( arg ) MEP_ACK_HIGH;
		else MEP_ACK_LOW;
		break;
	case TAVI_IOCSDATA:
		if( arg ) MEP_DATA_DIR_IN;
		else {
			MEP_DATA_DIR_OUT;
			MEP_DATA_LOW;
		}
		break;
	case TAVI_IOCGCLK:
		if( (*pPIO0_DATA&(1<<MEP_CLK))  ) {
			data = 1;
			copy_to_user( (int*)arg, &data, sizeof(int) );
		}
		else {
			data = 0;
			copy_to_user( (int*)arg, &data, sizeof(int) );
		}
		break;
	case TAVI_IOCGDATA:
		if( (*pPIO0_DATA&(1<<MEP_DATA)) ) {
			data = 1;
			copy_to_user( (int*)arg, &data, sizeof(int) );
		}
		else {
			data = 0;
			copy_to_user( (int*)arg, &data, sizeof(int) );
		}
		break;
	case TAVI_IOCGLINK:
		{
		struct tp_link_state_t link;
		link.clock = (*pPIO0_DATA&(1<<MEP_CLK)) ? 1 : 0; 
		link.data  = (*pPIO0_DATA&(1<<MEP_DATA)) ? 1 : 0; 
		link.ack   = (*pPIO0_DATA&(1<<MEP_ACK)) ? 1 : 0; 
		copy_to_user( (int*)arg, &link, sizeof(link) );
		}
	case TAVI_IOCFLUSH:
		mep_flusher( mepdev );
		break;
	case TAVI_IOCGHELLO:
		*( int* )arg = mep_get_hello( mepdev );
		break;
	}
	return 0;
}

static struct file_operations tp_fops = {
	owner: THIS_MODULE,
	open: ss_open,
	read: ss_read,
	write: ss_write,
	release: ss_release,
	ioctl: ss_ioctl
};

static struct miscdevice tavi_touchpad = {
	TAVI_TOUCHPAD_MINOR,
	"tp",
	&tp_fops
};

static int __init ss_init( void )
{
	int result;

	printk( "TAVI Touchpad(ScrollStrip(tm)) Driver 150\n" );

	dev.phy.set_data = mep_pl_setDATA;
	dev.phy.get_data = mep_pl_getDATA;
	dev.phy.set_ack  = mep_pl_setACK;
	dev.phy.get_clk  = mep_pl_getCLK;
	dev.phy.get_ack  = mep_pl_getACK;

	if( mep_link_init() == 0 ) {
		// retry
		if( mep_link_init() == 0 ) {
			printk( "failed to initialize touchpad\n" );
			return -1;
		}
	}

	dev.irq = 5;
	dev.busy = 0;
	init_waitqueue_head( &dev.mepwq );
	init_waitqueue_head( &dev.wq );

	if( (result=misc_register( &tavi_touchpad )) < 0 ) {
		printk( "tavi_tp.o: failed to register chardev(%d)\n", TAVI_TOUCHPAD_MINOR );
		return result;
	}

	return 0;
}

static void __init ss_exit( void )
{
	misc_deregister( &tavi_touchpad );
}

module_init( ss_init );
module_exit( ss_exit );

MODULE_AUTHOR( "Lee Seok<zstein@kiscos.net>" );
MODULE_DESCRIPTION( "ScrollStrip Driver for TAVI" );
MODULE_LICENSE( "GPL" );
