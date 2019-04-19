//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : MusicPlayer 
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/MusicPlayer.cpp,v $
// $Revision: 1.12 $
// $Date: 2006/06/21 10:56:36 $
// $Author: ywkim $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: MusicPlayer.cpp,v $
// Revision 1.12  2006/06/21 10:56:36  ywkim
// Add instruction of fat_dcreate_dir("Music") in Init() for resolving write error
//
// Revision 1.11  2006/06/20 07:24:40  ywkim
// Add case of MEDIA_DCF_MP3 in my_release() for melon
//
// Revision 1.10  2006/06/19 02:36:23  ywkim
// Change argument type of DHfree in ParseDCFTag()
//
// Revision 1.9  2006/06/12 07:59:01  ywkim
// Change printf to DEBUGMSG
//
// Revision 1.8  2006/06/12 04:31:40  ywkim
// Change some function for melon
//
// Revision 1.7  2006/06/08 06:51:42  zstein
// - removed unused codes.
//
// Revision 1.6  2006/05/10 02:26:45  zstein
// *** empty log message ***
//
// Revision 1.5  2006/04/19 02:29:42  zstein
// - skipped sending ack message when recived CMD_HDD_WAKEUP.
//
// Revision 1.4  2006/04/17 04:30:06  zstein
// - added CMD_HDD_WAKEUP command.
//
// Revision 1.3  2006/03/17 07:00:05  zstein
// - removed a pop-noise when next play in HOME MODE.
//
// Revision 1.2  2006/03/06 06:20:12  zstein
// - replaced mpegdec->Stop().
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//*******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "Component/object.h"
#include "Component/String.h"
#include "Component/FileUtil.h"
#include "mpegdec/mpegdec.h"
#include "mpegdec/em85xx.h"
#include "mp3/mp3.h"
#include "AudioCmd.h"
#include "Task/tavi_global.h"
#include "Task/tavi_mq.h"
#include "MP3HeaderInfo.h"
#include "ogg/ogg.h"
#include "wma/wma.h"
#include "ID3/libid3.h"

#include "ata_ioctl.h"
#include "atapi_ioctl.h"
#include "fatfs.h"

#include "Task/tavi_global.h"
#include "musicdef.h"

#include "SKTDRM.h"
#include "./Melon/Melon.h"

#undef DEBUG
#define DEBUG	0
#if DEBUG
static void debug_break (void)
{
}
#define ASSERT(exp)					((void)((exp)?1:(printf ("***ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
//#define DEBUGMSG( fmg, args... )		printf( "[MusicPlayer]"fmt, ## args )
#define DEBUGMSG					printf
#else
#define ASSERT(exp)
#define DEBUGMSG
#endif

#undef MSG_RECV_ID
#undef MSG_SEND_ID
#define MSG_RECV_ID		0x10
#define MSG_SEND_ID	0x20
#define SLASH			"/\000"

#if SHM_SCHEME		//	shared memory scheme
#define MP3_BLOCK_SIZE					(1024*256)
#define MP3_NBLOCKS						(32)
#define PCM_MP3_BLOCK_SIZE				(1152*4)
#define PCM_MP3_NBLOCKS				(64)

#define OGG_BLOCK_SIZE					(1024*512)
#define OGG_NBLOCKS					(16)
#define PCM_OGG_BLOCK_SIZE				(4096)
#define PCM_OGG_NBLOCKS				(64)

#define WMA_BLOCK_SIZE					(1024*512)
#define WMA_NBLOCKS					(16)
#define PCM_BLOCK_SIZE					(1152*4)
#define PCM_NBLOCKS						(64)
#else
//	9MB definition
#define MP3_BLOCK_SIZE					(1024*256)
#define MP3_NBLOCKS						(32)
#define PCM_MP3_BLOCK_SIZE				(1152*4)
#define PCM_MP3_NBLOCKS				(128)

#define OGG_BLOCK_SIZE					(1024*512)
#define OGG_NBLOCKS					(16)
#define PCM_OGG_BLOCK_SIZE				(4096)
#define PCM_OGG_NBLOCKS				(128)

#define WMA_BLOCK_SIZE					(1024*512)
#define WMA_NBLOCKS					(16)
#define PCM_BLOCK_SIZE					(1152*4)
#define PCM_NBLOCKS						(128)
/*
//	old 4MB definition
#define MP3_BLOCK_SIZE					(1024*512)
#define MP3_NBLOCKS						(8)
#define PCM_MP3_BLOCK_SIZE				(1152*4)
#define PCM_MP3_NBLOCKS				(16)

#define OGG_BLOCK_SIZE					(1024*256)
#define OGG_NBLOCKS					(16)
#define PCM_OGG_BLOCK_SIZE				(4096)
#define PCM_OGG_NBLOCKS				(16)

#define WMA_BLOCK_SIZE					(1024*512)
#define WMA_NBLOCKS					(8)
#define PCM_BLOCK_SIZE					(1152*4)
#define PCM_NBLOCKS						(16)
*/
#define PCM_DATA_BUFFER_SIZE			(1024*512)
#endif

extern Melon_Security_t MelonData;

//////////////////////////////////////////////////////////////////////
// global data
static MP3Decoder *pMP3Decoder;
static OGGDecoder *pOGGDecoder;
static WMADecoder* pWMADecoder;
#if SHM_SCHEME
static RMuint32 *databuffers;		// shared memory sheme
#else
static RMuint32 databuffers[1024*1024*2+ 1152*128];		// 9MB definition
//static RMuint32 databuffers[1024*1024+ 1152*16];		// old 4MB definition
#endif
static BufferPool BPmp3;
static BufferPool BPogg;
static BufferPool BPwma;
static BufferPool BPpcm;

static RMuint32 *mp3_buffer = (databuffers);
static RMuint32* ogg_buffer = databuffers;
static RMuint32* wma_buffer = databuffers;
static RMuint32* pcm_buffer = (databuffers+(1024*1024*2));	// 9MB definition

static MpegDecoder* pMpegDec;
static RMuint32 NumberOfBitsPerSample = 0;
static RMuint32 NumberOfChannels = 0;
static RMuint32 SamplesPerSecond = 0;
static RMuint32 BytesPerSecond = 0;
static RMuint32 Bitrate = 0;
static bool VBR;
static RMuint32 prebuffering = 0;
static int g_msgFlag;

static QID_t qid;


static MP3_CALLBACK_TABLE mp3_callback_table;
static OGG_CALLBACK_TABLE ogg_callback_table; 
static WMA_CALLBACK_TABLE wma_callback_table;
// end global data
//////////////////////////////////////////////////////////////////////

struct {
	bool ab;
	bool init;
	bool start;
	int media;
	int MBshmid;
	int error;
	int fatfserr;
	int playtime;
	int playstate;
	int totaltime;
	int mpegtime;
	int ab_stime;
	int ab_etime;
	bool noinfo; // OTP
} prop;

struct AudioCmd cmd;	
QBUF_t* buf;
AckInfo s_ackinfo;

// Prototypes
static void StopMusicPlayer( void );
//////////////////////////////////////////////////////////////////////
// file system callbacks required by the mp3 decoder
// power management for hdd

static int ata_fd = -1;
static RMuint32 fatfs_pbi_SendCmdRWS (
	#ifdef FATFS_PBI_V_3_0_3
	RMuint32 dwHandle,
	#endif
	RMuint8  bDrive, 
	RMuint8  bCmd, 
	RMuint32 nSectorLBA, 
	RMuint16 nNumSectors, 
	RMuint8  *pbBuf, 
	RMuint32 SizeInBytes)
{
	RMint32		result;
	ATA_COMMAND	AtaCommand;
	CMD_PACKET	*pCmdPacket;
	RMuint8		CmdPacket[16];

	if (ata_fd == -1)
	{
		DEBUGMSG ("***Error: Ata_ReadSectorsLBA: Invalid gAtaFd. Kernel Driver not open.\n");
		return (RMuint32)-1;
	}

	pCmdPacket = (CMD_PACKET *)&(CmdPacket[0]);

	pCmdPacket->bCmd				= bCmd;
	pCmdPacket->dwLBA				= nSectorLBA;
	pCmdPacket->wXxLength			= nNumSectors;		// N sectors

	AtaCommand.pbCmdPkt			= CmdPacket;
	AtaCommand.wPacketSize			= 12;
	AtaCommand.bDrv					= bDrive;	// Primary Drive
	AtaCommand.pbBuffer				= pbBuf;
	AtaCommand.dwRequestedXferCount	= SizeInBytes;		// SizeOf AtaCommand.pbBuffer
	AtaCommand.dwActualXferCount		= 0;
	AtaCommand.wAtaStatus			= 0;
	AtaCommand.wAtaError				= 0;
	AtaCommand.wExStatusBytes		= 0;

	result = ioctl (ata_fd, ATA_IOCTL_ISSUE_ATA_COMMAND, &AtaCommand);
	if (AtaCommand.wAtaError != 0) 
	{
		// Some Error
		DEBUGMSG ("***Error: Ata_SendCmd(): ioctl(): AtaCommand.wAtaError = 0x%04X.\n", AtaCommand.wAtaError);
	}
	return ((RMuint32)AtaCommand.wAtaError);
}

static RMuint32 fatfs_pbi_SendCmd (
	#ifdef FATFS_PBI_V_3_0_3
	RMuint32 dwHandle,
	#endif
	RMuint8 bDrive, 
	RMuint8	bCmd, 
	RMuint8 bFeature, 
	RMuint8	bSectCounter, 
	RMuint8 bSectNumber, 
	RMuint8 bCylinderLo,
	RMuint8	bCylinderHi, 
	RMuint8 bDevHead, 
	RMuint8 *pbIoBuf,
	RMuint32 dwBufSizeInBytes)
{
	RMint32 result;
	ATA_COMMAND AtaCommand;
	RMuint8 CmdPacket[16];

	if (ata_fd == -1)
	{
		DEBUGMSG ("***Error: Ata_ReadSectorsLBA: Invalid gAtaFd. Kernel Driver not open.\n");
		return (RMuint32)-1;
	}

	// Universal control - Send values for all registers + bDrv
	CmdPacket[0]	= bCmd;
	CmdPacket[1]	= bFeature;
	CmdPacket[2]	= bSectCounter;
	CmdPacket[3]	= bSectNumber;
	CmdPacket[4]	= bCylinderLo;
	CmdPacket[5]	= bCylinderHi;
	CmdPacket[6]	= bDevHead;

	AtaCommand.pbCmdPkt				= CmdPacket;
	AtaCommand.wPacketSize			= 12;
	AtaCommand.bDrv					= bDrive;			// Primary Drive
	AtaCommand.pbBuffer				= pbIoBuf;
	AtaCommand.dwRequestedXferCount	= dwBufSizeInBytes;	// SizeOf AtaCommand.pbBuffer
	AtaCommand.dwActualXferCount	= 0;
	AtaCommand.wAtaStatus			= 0;
	AtaCommand.wAtaError			= 0;
	AtaCommand.wExStatusBytes		= 0;

	result = ioctl (ata_fd, ATA_IOCTL_ISSUE_ATA_COMMAND, &AtaCommand);
	if (AtaCommand.wAtaError != 0) 
	{
		// Some Error
		DEBUGMSG ("***Error: Ata_SendCmd(): %02x %02x %02x %02x %02x %02x %02x %02x\n", 
			bDrive, bCmd, bFeature, bSectCounter, 
			bSectNumber, bCylinderLo, bCylinderHi, bDevHead);
		DEBUGMSG ("***Error: Ata_SendCmd(): ioctl(): AtaCommand.wAtaError = 0x%04X.\n", AtaCommand.wAtaError);
	}
	return ((RMuint32)AtaCommand.wAtaError);
}
static void *fatfs_malloc (RMuint32 s)
{
	void *p;	
	p = malloc (s);
	return p;
}

static void fatfs_free (void *p)
{
	return free (p);
}


static RMuint32 my_fat_mount (void)
{
	FATFS_ERROR err;
	FATFS_CALLBACK_TABLE callbacktable;

	if (ata_fd == -1)
		ata_fd = open ("/dev/atapi", O_RDWR | O_NONBLOCK); 
	ASSERT (ata_fd != -1);
	if (ata_fd == -1)
	{
		return (RMuint32)-1;
	}
	callbacktable.Ata_SendCmd = fatfs_pbi_SendCmd;
	callbacktable.Ata_SendCmdRWS = fatfs_pbi_SendCmdRWS;
	callbacktable.malloc = fatfs_malloc;
	callbacktable.mfree = fatfs_free;

	err = fatfs_mount (0, &callbacktable, 0xff);
	if (err != FATFS_ERROR_NO_ERROR)
	{
		DEBUGMSG ( "error mounting filesystem.: 0x%x\n", err );
		close (ata_fd);
		ata_fd = -1;
		return (RMuint32)-1;
	}
	return 0;
}

static void my_fat_exit( void )
{
	fatfs_umount( 0 );
	close( ata_fd );
	ata_fd = -1;
}

static RMuint32 my_fat_set_logical_drive (int index)
{
	RMuint32 err;
	err = fatfs_setcurlogdrive (index);
	return err;
}

////////////////////////////////////////////////////////////////////
RMuint32 Melon_fopen(RMint8 *filename, void *context)
{
	DH_INT32 fd;

	fd = DRM_Open( filename, DH_FILE_OPEN_RDONLY, O_USAGE_CNT_CONSUME );
	DEBUGMSG( "--- 800th :: Melon_fopen =%s, fd=%d\n", filename, fd );
	if ( fd < 0 ) {
		prop.error = AUDIO_ERR_MELON;
		prop.fatfserr = fd;
		DEBUGMSG( "--- 801th :: Melon_fopen err, fd=%d\n", filename, fd );
		return 0;
		}
	else {
		prop.fatfserr = 0;
		}
#if 0
	// melon drm decryption
	RMuint32 fddst;
	char buf[1024];
	int	ret;
	RMuint32 len;
	fatfs_fopen( (RMuint8*)"/hd1/test.mp3", _O_CREAT | _O_WRONLY, &fddst);
	while( (ret = DRM_Read( (DH_INT16)fd, (DH_BYTE*)buf, 1024)) > 0) { 
		fatfs_fwrite( fddst, (RMuint8*)buf, ret, &len);
		}
	fatfs_fclose( fddst );
#endif

	return (RMuint32)fd;
}
RMuint32 Melon_fread(RMuint32 handle, void *buf, RMuint32 length, void *context)
{
	DH_INT32 n = 0;

	n = DRM_Read( (DH_INT16)handle, (DH_BYTE*)buf, (DH_INT32)length);
	DEBUGMSG( "--- 701th :: Melon_fread : handle=%d, length=%d,  n=%d\n", 
		handle, length, n );

	if( n < 0 ) {
		prop.error = AUDIO_ERR_MELON;
		prop.fatfserr = n;
		return 0;
		}
	else {
		prop.fatfserr = 0;
		}

	return (RMuint32)n;
}
RMuint32 Melon_fseek(RMuint32 handle, RMuint32 pos, RMuint32 whence, void *context)
{
	DH_INT32 readed;
	
	switch ( whence ) {
		case SEEK_CUR:
			whence = DH_FILE_SEEK_CUR;
			break;
		case SEEK_END:
			whence = DH_FILE_SEEK_END;
			break;
		case SEEK_SET:
			whence = DH_FILE_SEEK_SET;
			break;
		}
	readed = DRM_Seek( (DH_INT16)handle, (DH_INT32) pos, (DH_FHANDLE_SET_POSITION)whence );

	if( readed < 0 ) {
		prop.error = AUDIO_ERR_MELON;
		prop.fatfserr = readed;
		readed = 0;
		}
	else {
		prop.fatfserr = 0;
		}

	DEBUGMSG( "--- 803th :: Melon_fseek :: readed=%d, handle=%d, whence=%d, pos=%d\n",
		readed, handle, whence, pos );

	return (RMuint32)readed;
	
}
RMuint32 Melon_ftell (RMuint32 handle, void *context)
{
	DH_INT32 pos = 0;

	pos = DRM_GetFileSize ( (DH_INT16)handle );
	
	if( pos < 0 ) {
		prop.error = AUDIO_ERR_MELON;
		prop.fatfserr = pos;
		}
	else {
		prop.fatfserr = 0;
		}

	DEBUGMSG( "--- 804th :: Melon_ftell : handle=%d, pos=%d\n", handle, pos );

	return (RMuint32) pos;
}
RMuint32 Melon_fclose(RMuint32 handle, void *context)
{
	DRM_Close( (DH_INT16)handle );
	
	DEBUGMSG( "--- 805th :: Melon_fclose : handle=%d\n", handle );
	return 0;
}
////////////////////////////////////////////////////////////////////
RMuint32 my_fopen (RMint8 *filename, void *context)
{
	RMuint32 fd=0;
	
	prop.fatfserr = fatfs_wfopen ((RMuint16*)filename, _O_RDONLY, &fd);
	if( prop.fatfserr != FATFS_ERROR_NO_ERROR ) {
		prop.error = AUDIO_ERR_FILEOPEN;
		DEBUGMSG( "failed to fs_fopen(0x%x)\n", prop.fatfserr );
		return 0;
	}
	
	return fd;
}

RMuint32 my_fread (RMuint32 handle, void *buf, RMuint32 length, void *context)
{		
	RMuint32 n = 0;
	prop.fatfserr = fatfs_fread (handle, (RMuint8 *)buf, length, &n);
	if( prop.fatfserr != FATFS_ERROR_NO_ERROR ) {
		DEBUGMSG( "failed to fs_fread\n" );
		return 0;
	}
	
	return n;
}

static RMuint32 my_fseek (RMuint32 handle, RMuint32 pos, RMuint32 whence, void *context)
{
	prop.fatfserr = fatfs_fseek (handle, pos, whence);
	if (prop.fatfserr != FATFS_ERROR_NO_ERROR) {
		printf( "fatfs_fseek error\n" );
		return 0;
	}

	return pos;
}


RMuint32 my_ftell (RMuint32 handle, void *context)
{
	RMuint32 pos = 0;
	prop.fatfserr = fatfs_ftell (handle, &pos);
	
	return pos;
}

RMuint32 my_feof (RMuint32 handle, void *context)
{
	RMuint32 eof = 0;
	prop.fatfserr = fatfs_feof (handle, &eof);

	return eof;
}

RMuint32 my_fclose (RMuint32 handle, void *context)
{
	prop.fatfserr = fatfs_fclose (handle);

	return 0;
}

#define JASPER_PIO0_BASE		0x00500600
#define PIO_DATA			    0x00000004
#define PIO_DIR			        0x00000008
#define pPIO0_DIR               ( volatile int* )( JASPER_PIO0_BASE + PIO_DIR )
#define pPIO0_DATA              ( volatile int* )( JASPER_PIO0_BASE + PIO_DATA )
RMuint32 my_powerup ()
{
	// ATA reset

	*pPIO0_DATA = 0x00020002;
	*pPIO0_DIR = 0x00020002;
	*pPIO0_DATA = 0x00020000;
	usleep( 1 );
	*pPIO0_DATA = 0x00020002;

	ATA_COMMAND AtaCommand;
	RMuint8 CmdPacket[16];

	CmdPacket[0]	= 0xe1;
	CmdPacket[1]	= 0;
	CmdPacket[2]	= 0;
	CmdPacket[3]	= 0;
	CmdPacket[4]	= 0;
	CmdPacket[5]	= 0;
	CmdPacket[6]	= 0;

	AtaCommand.pbCmdPkt				= CmdPacket;
	AtaCommand.wPacketSize			= 12;
	AtaCommand.bDrv					= 0;
	AtaCommand.pbBuffer				= 0;
	AtaCommand.dwRequestedXferCount	= 0;
	AtaCommand.dwActualXferCount	= 0;
	AtaCommand.wAtaStatus			= 0;
	AtaCommand.wAtaError			= 0;
	AtaCommand.wExStatusBytes		= 0;

	ioctl (ata_fd, ATA_IOCTL_ISSUE_ATA_COMMAND, &AtaCommand);
	if (AtaCommand.wAtaError != 0) 
	{
		printf("IDLE IMMEDIATE command error\n");
		printf("error:  %04x\n", AtaCommand.wAtaError);
		printf("status: %04x\n", AtaCommand.wAtaStatus);
	}
	
	return 0;
}


// end file system callbacks required by mp3 decoder
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// callbacks required by the mp3 decoder
RMuint32 my_addref (RMuint8 *pBuffer, void *context)
{
	BufferPool *BP;
	if ((RMuint32)pBuffer < (RMuint32)pcm_buffer) {
		switch( prop.media ) {
		case MEDIA_MP3:
		case MEDIA_DCF_MP3:
			BP = &BPmp3;
			break;
		case MEDIA_OGG:
			BP = &BPogg;
			break;
		case MEDIA_WMA:
			BP = &BPwma;
			break;
		}
	}
	else {
		BP = &BPpcm;
	}
	RMint32 idx = ((RMint32)pBuffer - (RMint32)BP->start_address) / BP->buffers_size;
	ASSERT (idx >= 0);
	addref_buffer (&(BP->B[idx]));
	return 0;
}

RMuint32 my_release (RMuint8 *pBuffer, void *context)
{
	BufferPool *BP;
	if ((RMuint32)pBuffer < (RMuint32)pcm_buffer) {
		switch( prop.media ) {
		case MEDIA_MP3:
		case MEDIA_DCF_MP3:			
			BP = &BPmp3;
			break;
		case MEDIA_OGG:
			BP = &BPogg;
			break;
		case MEDIA_WMA:
			BP = &BPwma;
			break;
		}
	}
	else {
		BP = &BPpcm;
	}
	RMint32 idx = ((RMint32)pBuffer - (RMint32)BP->start_address) / BP->buffers_size;
	ASSERT (idx >= 0);
	release_buffer (&(BP->B[idx]));
	return 0;
}


void SendMessage( int command, int state, int error )
{
	if( prop.noinfo ) return;
	
	cmd.lMsgID = MSG_SEND_ID;
	cmd.lCmd = command;
	cmd.lLength = sizeof(struct AudioCmd)-4;
	cmd.lError = error;
	cmd.lState = state;
	SysMSGSend( qid, ::buf, ::buf->lLength, 0 );
}

void SendPlaybackInfo( void )
{
	struct PlaybackInfo* info = ( struct PlaybackInfo* )cmd.szArg;
	info->lTime = prop.playtime;
	info->lSamplingrate = SamplesPerSecond;
	info->lBitrate = Bitrate;
	info->lVBR = VBR;
	SendMessage( AUDIO_CMD_PLAYBACK, prop.playstate, prop.error );
}

// mp3 callback - the mp3 decoder is informing teh application of 
// some useful mp3 information
static RMuint32 mp3_info (RMint32 msg, void *info, void *context)
{
	if (msg == MP3_MSG_MP3_DECODE_INFO)
	{
		MP3_DECODE_INFO *pInfo = (MP3_DECODE_INFO *)info;
		if ((pInfo->NumberOfBitsPerSample != NumberOfBitsPerSample) ||
			(pInfo->NumberOfChannels != NumberOfChannels) ||
			(pInfo->SamplesPerSecond != SamplesPerSecond))
		{
			PCM_PARAMETERS pcmparams;
			pcmparams.NumberOfBitsPerSample = pInfo->NumberOfBitsPerSample;
			pcmparams.NumberOfChannels = pInfo->NumberOfChannels;
			pcmparams.SamplesPerSecond = pInfo->SamplesPerSecond;
			DEBUGMSG ("PCM update:\n");
			DEBUGMSG (" NumberOfBitsPerSample: %lu\n", pcmparams.NumberOfBitsPerSample);
			DEBUGMSG (" NumberOfChannels:      %lu\n", pcmparams.NumberOfChannels);
			DEBUGMSG (" SamplesPerSecond:      %lu\n", pcmparams.SamplesPerSecond);
			pMpegDec->SetPCMParameters (&pcmparams);
			NumberOfBitsPerSample = pInfo->NumberOfBitsPerSample;
			NumberOfChannels = pInfo->NumberOfChannels;
			SamplesPerSecond = pInfo->SamplesPerSecond;
			Bitrate = pInfo->Bitrate/1000;
			if( Bitrate ) SendPlaybackInfo();
			BytesPerSecond = (NumberOfBitsPerSample / 8) * NumberOfChannels * SamplesPerSecond;
			prebuffering = 1;
			if (prebuffering)
			{
				pMpegDec->Stop ();
				pMpegDec->Pause ();	
			}
		}
		if( Bitrate != pInfo->Bitrate )  Bitrate = pInfo->Bitrate;
	}
	return 0;
}
static RMuint32 dcf_info (RMint32 msg, void *info, void *context)
{
	if (msg == MP3_MSG_MP3_DECODE_INFO)
	{
		MP3_DECODE_INFO *pInfo = (MP3_DECODE_INFO *)info;
		if ((pInfo->NumberOfBitsPerSample != NumberOfBitsPerSample) ||
			(pInfo->NumberOfChannels != NumberOfChannels) ||
			(pInfo->SamplesPerSecond != SamplesPerSecond))
		{
			PCM_PARAMETERS pcmparams;
			pcmparams.NumberOfBitsPerSample = pInfo->NumberOfBitsPerSample;
			pcmparams.NumberOfChannels = pInfo->NumberOfChannels;
			pcmparams.SamplesPerSecond = pInfo->SamplesPerSecond;
			DEBUGMSG ("PCM update:\n");
			DEBUGMSG (" NumberOfBitsPerSample: %lu\n", pcmparams.NumberOfBitsPerSample);
			DEBUGMSG (" NumberOfChannels:      %lu\n", pcmparams.NumberOfChannels);
			DEBUGMSG (" SamplesPerSecond:      %lu\n", pcmparams.SamplesPerSecond);
			pMpegDec->SetPCMParameters (&pcmparams);
			NumberOfBitsPerSample = pInfo->NumberOfBitsPerSample;
			NumberOfChannels = pInfo->NumberOfChannels;
			SamplesPerSecond = pInfo->SamplesPerSecond;
			Bitrate = pInfo->Bitrate/1000;
			if( Bitrate ) SendPlaybackInfo();
			BytesPerSecond = (NumberOfBitsPerSample / 8) * NumberOfChannels * SamplesPerSecond;
			prebuffering = 1;
			if (prebuffering)
			{
				pMpegDec->Stop ();
				pMpegDec->Pause ();	
			}
		}
		if( Bitrate != pInfo->Bitrate )  Bitrate = pInfo->Bitrate;
	}
	return 0;
}

// mp3 callback - the mp3 decoding object is requesting
// the application render some pcm samples
static RMuint32 mp3_putPCM (RMuint8 *pBuffer, RMuint32 length, void *context)
{
	BufferPool *BP;
	BP = &BPpcm;

	ASSERT ((RMuint32)pBuffer >= (RMuint32)pcm_buffer);
	ASSERT ((RMuint32)pBuffer >= (RMuint32)BP->start_address);
	RMint32 idx = ((RMuint32)pBuffer - (RMuint32)BP->start_address) / BP->buffers_size;
	addref_buffer (&(BP->B[idx]));

	// no need to send a timestamp when dealing with only PCM samples
	if (pMpegDec->WritePCM (pBuffer, length, &(BP->B[idx]), 0, 0, 0) == MPEG_DECODER_ERROR_NO_ERROR)
	{
		ASSERT (BytesPerSecond);
		return 0;
	}
	if (prebuffering)
	{
		prebuffering = 0;
		pMpegDec->Play ();
	}
	release_buffer (&(BP->B[idx]));
	return 1;
}

// mp3 callback - the mp3 decoding object is requesting
// the application to provide a buffer to place the decoded
// pcm samples into
static RMuint32 mp3_getPCM (RMuint8 **pBuffer, RMuint32 *plength, void *context)
{
 	int idx = find_free_buffer (&BPpcm, 0);
	if (idx < 0)
	{
		if (prebuffering)
		{
			prebuffering = 0;
			pMpegDec->Play ();
		}
		return 1;
	}
	*plength = (RMuint32)BPpcm.buffers_size;
	*pBuffer = (RMuint8 *)get_buffer_address (&BPpcm, idx);
	ASSERT ((RMuint32)*pBuffer >= (RMuint32)pcm_buffer);
	return 0;
}

// mp3 decode callback - the mp3 decoding object is requesting the
// application to provide a buffer to read mp3 data into
static RMuint32 mp3_getMP3 (RMuint8 **pBuffer, RMuint32 *plength, void *context)
{
 	int idx = find_free_buffer (&BPmp3, 0);
	if (idx < 0)
	{
		if (prebuffering)
		{
			prebuffering = 0;
			pMpegDec->Play ();
		}
		return 1;
	}
	*plength = (RMuint32)BPmp3.buffers_size;
	*pBuffer = (RMuint8 *)get_buffer_address (&BPmp3, idx);
	return 0;
}
// end decoding callbacks required by mp3 decoder
//////////////////////////////////////////////////////////////////////

/* OGG Callbacks
 */
static RMuint32 ogg_info (RMint32 msg, void *info, void *context)
{
 	if (msg == OGG_MSG_OGG_DECODE_INFO)
	{
		OGG_DECODE_INFO *pInfo = (OGG_DECODE_INFO *)info;
		if ((pInfo->NumberOfBitsPerSample != NumberOfBitsPerSample) ||
			(pInfo->NumberOfChannels != NumberOfChannels) ||
			(pInfo->SamplesPerSecond != SamplesPerSecond))
		{
			PCM_PARAMETERS pcmparams;
			pcmparams.NumberOfBitsPerSample = pInfo->NumberOfBitsPerSample;
			pcmparams.NumberOfChannels = pInfo->NumberOfChannels;
			pcmparams.SamplesPerSecond = pInfo->SamplesPerSecond;
			DEBUGMSG("PCM update:\n");
			DEBUGMSG(" NumberOfBitsPerSample: %lu\n", pcmparams.NumberOfBitsPerSample);
			DEBUGMSG(" NumberOfChannels:      %lu\n", pcmparams.NumberOfChannels);
			DEBUGMSG(" SamplesPerSecond:      %lu\n", pcmparams.SamplesPerSecond);
			pMpegDec->SetPCMParameters (&pcmparams);
			NumberOfBitsPerSample = pInfo->NumberOfBitsPerSample;
			NumberOfChannels = pInfo->NumberOfChannels;
			SamplesPerSecond = pInfo->SamplesPerSecond;
			Bitrate = pInfo->Bitrate;
			BytesPerSecond = (NumberOfBitsPerSample / 8) * NumberOfChannels * SamplesPerSecond;
			prebuffering = 1;
			if (prebuffering)
			{
				pMpegDec->Stop ();
				pMpegDec->Pause ();	
			}
		}
	}
	return 0;
}

// ogg decode callback - the ogg decoding object is requesting the
// application to provide a buffer to read ogg data into
static RMuint32 ogg_getOGG (RMuint8 **pBuffer, RMuint32 *plength, void *context)
{
 	int idx = find_free_buffer (&BPogg, 0);
	if (idx < 0)
	{
		if (prebuffering)
		{
			//fs_powerdown ();
			prebuffering = 0;
			pMpegDec->Play ();
		}
		return 1;
	}
	*plength = (RMuint32)BPogg.buffers_size;
	*pBuffer = (RMuint8 *)get_buffer_address (&BPogg, idx);
	return 0;
}

/* OGG Callback END
 */

/* WMA Callback 
 */
static RMuint32 wma_info (RMint32 msg, void *info, void *context)
{
	if (msg == WMA_MSG_MP3_DECODE_INFO)
	{
		WMA_DECODE_INFO *pInfo = (WMA_DECODE_INFO *)info;
		if ((pInfo->NumberOfBitsPerSample != NumberOfBitsPerSample) ||
			(pInfo->NumberOfChannels != NumberOfChannels) ||
			(pInfo->SamplesPerSecond != SamplesPerSecond))
		{
			PCM_PARAMETERS pcmparams;
			pcmparams.NumberOfBitsPerSample = pInfo->NumberOfBitsPerSample;
			pcmparams.NumberOfChannels = pInfo->NumberOfChannels;
			pcmparams.SamplesPerSecond = pInfo->SamplesPerSecond;
			DEBUGMSG("PCM update:\n");
			DEBUGMSG(" NumberOfBitsPerSample: %lu\n", pcmparams.NumberOfBitsPerSample);
			DEBUGMSG(" NumberOfChannels:      %lu\n", pcmparams.NumberOfChannels);
			DEBUGMSG(" SamplesPerSecond:      %lu\n", pcmparams.SamplesPerSecond);
			pMpegDec->SetPCMParameters (&pcmparams);
			NumberOfBitsPerSample = pInfo->NumberOfBitsPerSample;
			NumberOfChannels = pInfo->NumberOfChannels;
			SamplesPerSecond = pInfo->SamplesPerSecond;
			Bitrate = pInfo->Bitrate ;
			BytesPerSecond = (NumberOfBitsPerSample / 8) * NumberOfChannels * SamplesPerSecond;
			prebuffering = 1;
			if (prebuffering)
			{
				pMpegDec->Stop ();
				pMpegDec->Pause ();	
			}
		}
	}
	return 0;
}

// wma callback function - the wma decoding object is requesting
// a buffer to read wma data into
static RMuint32 wma_getWMA (RMuint8 **pBuffer, RMuint32 *plength, void *context)
{
    RMint32 idx = find_free_buffer (&BPwma, 0);
    if (idx < 0)
    {
        if (prebuffering)
        {
            prebuffering = 0;
            pMpegDec->Play ();
        }
        return 1;
    }
    *plength = (RMuint32)BPwma.buffers_size;
    *pBuffer = (RMuint8 *)get_buffer_address (&BPwma, idx);
    return 0;
}
/* WMA Callback END
 */

// ============ power save ==========
static RMuint32 Reach_Eof( void )			// add callback function 
{
	SendMessage( AUDIO_CMD_SET_EOF, prop.playstate, AUDIO_ERR_NOERROR );
	DEBUGMSG("[%s] Reach_Eof\n", __FILE__ );
	return 0;
}
static RMuint32 Clear_Eof( void )
{
	SendMessage( AUDIO_CMD_CLEAR_EOF, prop.playstate, AUDIO_ERR_NOERROR );
	DEBUGMSG("[%s] Clear_Eof\n", __FILE__ );
	return 0;
}
static void WakeUp( void )
{
	SendMessage( AUDIO_CMD_HDD_WAKEUP, prop.playstate, AUDIO_ERR_NOERROR );
	DEBUGMSG("[%s] WakeUp\n", __FILE__ );
}
static void ClearBufferPool( BufferPool *pBP )	// for power save
{
	int	i;
	
	for(i=0; i<pBP->nb_buffers; i++)
		*(Buffer*)(&(pBP->B[i]))= 0;
}
// ============ power save ==========

/* 
 * Convert Rule
 * 0x0000 ~ 0x007F: 0xxxxxxx
 * 0x0080 ~ 0x07FF: 110xxxxx 10xxxxxx
 * 0x0800 ~ 0xFFFF: 1110xxxx 10xxxxxx 10xxxxxx
 * 0x10000 ~ 0x10FFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 * This function DOES NOT support 32-bit character.
 */
static int ConvertUTF8toUTF16( uchar_t* u16String, int nMax, RMuint8* szUTF8 )
{
	uchar_t ch;
	int i=0, j=0, count;
	
	if( !u16String || !szUTF8 ) return 0;

	while( (ch=szUTF8[i++]) ) {
		count = ch >> 4;
		count = 4-(count^0x0f);
		if( count < 0 ) count = 1;
		switch( count ) {
		case 1:
			u16String[j] = ch;
			break;
		case 2:
			u16String[j] = (szUTF8[i++]&0x3f) | (ch&0x3f)<<6;
			break;
		case 3:
			u16String[j] = (szUTF8[i+1]&0x3f) | (szUTF8[i]&0x3f)<<6 | (ch&0x1f)<<12;
			i+=2;
			break;
		}

		if( j == nMax-1 ) break;
		j++;
	}
	u16String[j] = 0;

	return j;
}


void ParseMP3Tag( uchar_t* path )
{
	// Get Artist, Album and title
	ID3Tag* tag;
	bool unicode;
	char* p, *psrc;
	int len;

	tag = ( ID3Tag* )calloc( 1, sizeof(ID3Tag) );
	if( GetID3v2Tag( tag, path, true ) ) {
		if( GetID3v1Tag( tag, path, true ) == 1 ) return;
	}
	

	// TITLE
	unicode = false;
	p = s_ackinfo.u.play.title.bytes;
	if( tag->title[0] == 0 && tag->title[1] == 0  ) {
		psrc = GetFilename( (char*)path, true );
		unicode = true;
	}
	else { 
		// unicode
		if( tag->title[0] == 0xff && tag->title[1] == 0xfe ) {
			psrc = &(tag->title[2]);
			unicode = true;
		}
		else {
			psrc = tag->title;
		}
	}
	if( unicode ) ustrncpy( (uchar_t*)p, (uchar_t*)psrc, (BYTE_LENGTH/2-1) );
	else strncpy( p, psrc, BYTE_LENGTH-1 );
	len = ( unicode ) ? ustrlen((uchar_t*)p) : strlen(p);
	CString::ClearWhite( p, unicode, len );
	s_ackinfo.u.play.title.unicode = unicode;

	// ARTIST
	unicode = false;
	p = s_ackinfo.u.play.artist.bytes;
	if( tag->artist[0] == 0 || CString::IsSpace(tag->artist) ) {
		strcpy( p, "unknown" );
	}
	else {
		// unicode
		if( tag->artist[0] == 0xff && tag->artist[1] == 0xfe ) {
			ustrncpy( (uchar_t*)p, (uchar_t*)&(tag->artist[2]), (BYTE_LENGTH/2-1) );
			unicode = true;
		}
		else {
			strncpy( p, tag->artist, BYTE_LENGTH-1 );
		}
	}
	len = ( unicode ) ? ustrlen((uchar_t*)p) : strlen(p);
	CString::ClearWhite( p, unicode, len );
	s_ackinfo.u.play.artist.unicode = unicode;

	// ALBUM
	unicode = false;
	p = s_ackinfo.u.play.album.bytes;
	if( tag->album[0] == 0 || CString::IsSpace(tag->album) ) {
		strcpy( p, "unknown" );
	}
	else {
		// unicode
		if( tag->album[0] == 0xff && tag->album[1] == 0xfe ) {
			ustrncpy( (uchar_t*)p, (uchar_t*)&(tag->album[2]), (BYTE_LENGTH/2-1) );
			unicode = true;
		}
		else {
			strncpy( p, tag->album, BYTE_LENGTH-1 );
		}
	}
	len = ( unicode ) ? ustrlen((uchar_t*)p) : strlen(p);
	CString::ClearWhite( p, unicode, len );
	s_ackinfo.u.play.album.unicode = unicode;

	free( tag );
}

void ParseOGGTag( uchar_t* path )
{
	char* psz;

	psz = pOGGDecoder->GetArtist();
	if( psz ) {
		ConvertUTF8toUTF16( (uchar_t*)s_ackinfo.u.play.artist.bytes, BYTE_LENGTH/2, (RMuint8*)psz );
		s_ackinfo.u.play.artist.unicode = true;
	}
	else {
		strcpy( s_ackinfo.u.play.artist.bytes, "unknown" );
		s_ackinfo.u.play.artist.unicode = false;
	}
	psz = pOGGDecoder->GetAlbum();
	if( psz ) {
		ConvertUTF8toUTF16( (uchar_t*)s_ackinfo.u.play.album.bytes, BYTE_LENGTH/2, (RMuint8*)psz );
		s_ackinfo.u.play.album.unicode = true;
	}
	else {
		strcpy( s_ackinfo.u.play.album.bytes, "unknown" );
		s_ackinfo.u.play.album.unicode = false;
	}
	psz = pOGGDecoder->GetTitle();
	if( psz ) {
		ConvertUTF8toUTF16( (uchar_t*)s_ackinfo.u.play.title.bytes, BYTE_LENGTH/2, (RMuint8*)psz );
		s_ackinfo.u.play.title.unicode = 1;
	}
	else {
		psz = GetFilename( (char*)path, true );
		ustrcpy( (uchar_t*)s_ackinfo.u.play.title.bytes, (uchar_t*)psz );
		s_ackinfo.u.play.title.unicode = false;
	}

}

void ParseWMATag( uchar_t* path )
{
	char* psz;
	int len;
	// Get Artist
	psz = pWMADecoder->GetArtist( &len );
	CString::ClearWhite( psz, true, len );
	len = ustrlen( (const uchar_t*)psz );
	if( len >= BYTE_LENGTH ) len = BYTE_LENGTH-2;
	if( psz && len ) {
		memcpy( s_ackinfo.u.play.artist.bytes, psz, len );
		s_ackinfo.u.play.artist.unicode = true;
	}
	else {
		strcpy( s_ackinfo.u.play.artist.bytes, UNKNOWN_STRING );
		s_ackinfo.u.play.artist.unicode = false;
	}
	// Get Title
	psz = pWMADecoder->GetTitle( &len );
	CString::ClearWhite( psz, true, len );
	len = ustrlen( (const uchar_t*)psz );
	if( len >= BYTE_LENGTH ) len = BYTE_LENGTH-2;
	if( psz && len ) {
		memcpy( s_ackinfo.u.play.title.bytes, psz, len );
	}
	else {
		psz = GetFilename( (char*)path, true );
		ustrcpy( (uchar_t*)s_ackinfo.u.play.title.bytes, (uchar_t*)psz );
	}
	s_ackinfo.u.play.title.unicode = true;
	// Get Album
	psz = pWMADecoder->GetAlbum( &len );
	CString::ClearWhite( psz, true, len );
	len = ustrlen( (const uchar_t*)psz );
	if( len >= BYTE_LENGTH ) len = BYTE_LENGTH-2;
	if( psz && len ) {
		memcpy( s_ackinfo.u.play.album.bytes, psz, len );
		s_ackinfo.u.play.album.unicode = true;
	}
	else {
		strcpy( s_ackinfo.u.play.album.bytes, UNKNOWN_STRING );
		s_ackinfo.u.play.album.unicode = false;
	}
}
void ParseDCFTag( uchar_t* path )
{
	// Get Artist, Album and title
	bool unicode;
	char* p, *psrc;
	int len;
	RMuint32 fd;
	DH_INT16 ret;
	DRM_ContentInfoPtr CI = NULL;
	DH_CHAR value[255] = {0,}; // 반드시 최소 255bytes 이상이어야 한다.
	
	fd = Melon_fopen( (char*)path, 0);
	if( fd < 0 ) { 
		goto skip;
		}
	if((ret = DRM_GetContentInfo( (DH_INT16)fd, &CI)) != NO_ERROR) { 
		goto skip;
		}	
#if 1
	// TITLE
	unicode = false;
	p = s_ackinfo.u.play.title.bytes;
	if((ret = DRM_GetUnsupportedValue(CI, (DH_BYTE*)"title", (DH_BYTE*)value)) != NO_ERROR) { 
		printf(" There is not Title : Error Code = %d\n", ret);
		psrc = GetFilename( (char*)path, false);
		strcpy( p, psrc );
		} 
	else { 
		{
			// ARTIST
			char *pArtist, *pp;
			int 	len;
			int	i;

			s_ackinfo.u.play.artist.unicode = unicode;
			pp=s_ackinfo.u.play.artist.bytes;
			
			len =strlen(value);
			for ( i = len; i>=0; i-- ) {
				if ( value[i]=='-' ) {
					pArtist = &value[i+2];
					value[i-1] = '\0';	
					printf(" Aritst : %s\n", pArtist);
					strcpy( pp, pArtist );
					break;
					}
				}
			if ( i < 0 ) {
				strcpy( pp, "unknown" );
				}
		}

		printf(" title : %s\n", value);		
		strcpy( p, value );
		}
#else
	// TITLE
	unicode = false;
	p = s_ackinfo.u.play.title.bytes;
	if((ret = DRM_GetUnsupportedValue(CI, (DH_BYTE*)"title", (DH_BYTE*)value)) != NO_ERROR) { 
		printf(" There is not Title : Error Code = %d\n", ret);
		psrc = GetFilename( (char*)path, false);
		strcpy( p, psrc );
		} 
	else { 
		printf(" title : %s\n", value);		
		strcpy( p, value );
		}

	// ARTIST
	unicode = false;
 	s_ackinfo.u.play.artist.unicode = unicode;
	p = s_ackinfo.u.play.artist.bytes;
	if((ret = DRM_GetUnsupportedValue(CI, (DH_BYTE*)"copyright", (DH_BYTE*)value)) != NO_ERROR) {
		printf(" There is not copyright : Error Code = %d\n", ret);
		strcpy( p, "unknown" );
		} 
	else { 
		printf(" copyright : %s\n", value);
		strcpy( p, value );
		}
#endif
	// ALBUM
	unicode = false;
 	s_ackinfo.u.play.album.unicode = unicode;
	p = s_ackinfo.u.play.album.bytes;
	if((ret = DRM_GetUnsupportedValue(CI, (DH_BYTE*)"x-album", (DH_BYTE*)value)) != NO_ERROR) { 
		printf(" There is not x-album : Error Code = %d\n", ret);
		strcpy( p, "unknown" );
		}
	else { 
		printf(" x-album : %s\n", value);
		strcpy( p, value );
		}

	// total time
	if((ret = DRM_GetUnsupportedValue(CI, (DH_BYTE*)"length", (DH_BYTE*)value)) != NO_ERROR) { 
		printf(" There is not length : Error Code = %d\n", ret);
		} 
	else { 
		printf(" length : %s\n", value);
		struct {
			char Hou[4];
			char Min[4];
			char Sec[4];
			} szTime;
		struct {
			int Hou;
			int Min;
			int Sec;
			} tTime;
		sscanf(value,"%[0-9]:%[0-9]:%[0-9]", szTime.Hou, szTime.Min, szTime.Sec );
		tTime.Hou = atoi( szTime.Hou );
		tTime.Min = atoi( szTime.Min );
		tTime.Sec = atoi( szTime.Sec );
		s_ackinfo.u.play.totaltime = tTime.Hou*3600 + tTime.Min*60 + tTime.Sec;
		printf(" total time : %d.%d.%d = %d\n", tTime.Hou, tTime.Min, tTime.Sec, s_ackinfo.u.play.totaltime);
		}
#if 0	
	// size
	if((ret = DRM_GetUnsupportedValue(CI, (DH_BYTE*)"size", (DH_BYTE*)value)) != NO_ERROR) { 
		printf(" There is not size : Error Code = %d\n", ret); 
		} 
	else { 
		printf(" size : %s\n", value);
		} 
	// bitrate
	if((ret = DRM_GetUnsupportedValue(CI, (DH_BYTE*)"bitrate", (DH_BYTE*)value)) != NO_ERROR) { 
		printf(" There is not bitrate : Error Code = %d\n", ret); 
		} 
	else { 
		printf(" bitrate : %s\n", value); 
		}
	// sampling rate
	if((ret = DRM_GetUnsupportedValue(CI, (DH_BYTE*)"sampling rate", (DH_BYTE*)value)) != NO_ERROR) { 
		printf(" There is not sampling rate : Error Code = %d\n", ret); 
		}
	else { 
		printf(" sampling rate : %s\n", value); 
		}
	// codec
	if((ret = DRM_GetUnsupportedValue(CI, (DH_BYTE*)"codec", (DH_BYTE*)value)) != NO_ERROR) { 
		printf(" There is not codec : Error Code = %d\n", ret);
		} 
	else { 
		printf(" codec : %s\n", value);
		}
#endif	

	DHfree( CI );
skip:	
	Melon_fclose(fd, 0);

}

void ParseTag( uchar_t* path )
{
	switch( prop.media ) {
	case MEDIA_MP3: ParseMP3Tag( path ); break;
	case MEDIA_OGG: ParseOGGTag( path ); break;
	case MEDIA_WMA: ParseWMATag( path ); break;
	case MEDIA_DCF_MP3: ParseDCFTag( path ); break;
	}
}

void ParseTotalTime( uchar_t* path )
{
	MP3HdrInfo info;
//	s_ackinfo.u.play.totaltime = 0;
	switch( prop.media ) {
	case MEDIA_MP3: 
		info.Decode( (char*)path, true );
		s_ackinfo.u.play.totaltime = info.m_nTotalTime;
		VBR = info.m_fVBR;
		break;
	case MEDIA_OGG: 
		s_ackinfo.u.play.totaltime = pOGGDecoder->GetTotalTime();
		break;
	case MEDIA_WMA: 
		s_ackinfo.u.play.totaltime = pWMADecoder->GetTotalTime();
		break;
	case MEDIA_DCF_MP3:
		
		break;
	}
	prop.totaltime = s_ackinfo.u.play.totaltime;
}

static void MP3Stop( void )
{
    if( pMP3Decoder ) {
    	pMP3Decoder->DecodeBufferEnd();
    	delete pMP3Decoder;
    	pMP3Decoder = NULL;
   	}
   	
	deinit_buffer_pool (&BPmp3);
	deinit_buffer_pool (&BPpcm);
}

static void OGGStop( void )
{
	if( !pOGGDecoder ) return;
	
	deinit_buffer_pool (&BPogg);
	deinit_buffer_pool (&BPpcm);

	if( pOGGDecoder ) {
    	delete pOGGDecoder;
    	pOGGDecoder = NULL;
	}
}

static void WMAStop( void )
{
	deinit_buffer_pool( &BPwma );
	deinit_buffer_pool( &BPpcm );

	if( pWMADecoder ) {
    	delete pWMADecoder;
    	pWMADecoder = NULL;
	}
}



static bool MP3PlayInit( const char* szPath )
{
	if( pMP3Decoder == NULL ) {
		pMP3Decoder = (MP3Decoder *) new MP3Decoder;
	}
	init_buffer_pool( &BPmp3, mp3_buffer, MP3_BLOCK_SIZE, MP3_NBLOCKS );
	init_buffer_pool( &BPpcm, pcm_buffer, PCM_MP3_BLOCK_SIZE, PCM_MP3_NBLOCKS );
	
	mp3_callback_table.context = &BPmp3;		// saved context information for application
	mp3_callback_table.fopen = my_fopen;		// fopen for file decoding
	mp3_callback_table.fseek = my_fseek;		// fseek for file decoding
	mp3_callback_table.ftell = my_ftell;		// tell for file decoding
	mp3_callback_table.fread = my_fread;		// fread for file decoding
	mp3_callback_table.fclose = my_fclose;		// fclose for file decoding
	mp3_callback_table.addref = my_addref;		// addref a buffer
	mp3_callback_table.release = my_release;	// release a buffer
	mp3_callback_table.info = mp3_info;			// inform app of some mp3 information
	mp3_callback_table.putPCM = mp3_putPCM;		// output PCM bytes
	mp3_callback_table.getPCM = mp3_getPCM;		// get a buffer for PCM data
	mp3_callback_table.getMP3 = mp3_getMP3;		// get a buffer for reading MP3 data
	mp3_callback_table.reachEof = Reach_Eof;		// talk reaching eof	power save
	
	pMP3Decoder->Init ();
	pMP3Decoder->InitCallbackTable (&mp3_callback_table);
	
	pMP3Decoder->EnableMP3BufferQ ( true );		// power save
//	Clear_Eof();									// power save

	pMpegDec->XEN_SetPower( true );				// eq on
	pMpegDec->XEN_ClearSpectrumData();			// clear spectrum

	if (pMP3Decoder->DecodeFile( (char*)szPath ) != MP3_DECODER_ERROR_NO_ERROR)
	{
		DEBUGMSG( "failed to decode\n" );
 		//prebuffering = 1;
		//prop.playtime = 0;
		delete pMP3Decoder;
		pMP3Decoder = 0;
		deinit_buffer_pool (&BPmp3);
		deinit_buffer_pool (&BPpcm);
	
		return false;
	}
	return true;
}

static bool OGGPlayInit( const char* szPath )
{
	ogg_callback_table.context = 0; 
	ogg_callback_table.fopen = my_fopen;
	ogg_callback_table.fseek = my_fseek;
	ogg_callback_table.ftell = my_ftell;
	ogg_callback_table.fread = my_fread;
	ogg_callback_table.fclose = my_fclose;
	ogg_callback_table.addref = my_addref;
	ogg_callback_table.release = my_release;
	ogg_callback_table.info = ogg_info;
	ogg_callback_table.putPCM = mp3_putPCM;
	ogg_callback_table.getPCM = mp3_getPCM;
	ogg_callback_table.getOGG = ogg_getOGG;

	if( pOGGDecoder == NULL ) {
		pOGGDecoder = new OGGDecoder;
	}
	
	init_buffer_pool (&BPogg, ogg_buffer, OGG_BLOCK_SIZE, OGG_NBLOCKS);
	init_buffer_pool(&BPpcm, pcm_buffer, PCM_OGG_BLOCK_SIZE, PCM_OGG_NBLOCKS);
	pOGGDecoder->Init ();
	pOGGDecoder->InitCallbackTable (&ogg_callback_table);
	//pMpegDec->XEN_SetPower( false );				// eq off
	pMpegDec->XEN_SetPower( true );	
	pMpegDec->XEN_ClearSpectrumData();			// clear spectrum
	if (pOGGDecoder->DecodeFile( (RMint8*)szPath ) != OGG_DECODER_ERROR_NO_ERROR) {
		DEBUGMSG("pOGGDecoder->DecodeFile (%s) failed\n", szPath);
		delete pOGGDecoder;
		pOGGDecoder = 0;
		deinit_buffer_pool (&BPogg);
		deinit_buffer_pool (&BPpcm);
		
		return false;
	}

	return true;
}

static bool WMAPlayInit( const char* szPath )
{
        init_buffer_pool (&BPwma, wma_buffer, WMA_BLOCK_SIZE, WMA_NBLOCKS);
        init_buffer_pool (&BPpcm, pcm_buffer, PCM_BLOCK_SIZE, PCM_NBLOCKS);

	 if( pWMADecoder == NULL ) {
	 	DEBUGMSG( "WMAPLAYINIT\n" );
	 	pWMADecoder = new WMADecoder;
	 }
        pWMADecoder->Init ();
	wma_callback_table.context = &BPwma;        // saved context information for application
        wma_callback_table.fopen = my_fopen;        // fopen for file decoding
        wma_callback_table.fseek = my_fseek;        // fseek for file decoding
        wma_callback_table.fread = my_fread;        // fread for file decoding
        wma_callback_table.fclose = my_fclose;      // fclose for file decoding
        wma_callback_table.addref = my_addref;      // addref a buffer
        wma_callback_table.release = my_release;    // release a buffer
        wma_callback_table.info = wma_info;         // inform app of some mp3 information
        wma_callback_table.putPCM = mp3_putPCM;      // output PCM bytes
        wma_callback_table.getPCM = mp3_getPCM;      // get a buffer for PCM data
        wma_callback_table.getWMA = wma_getWMA;     // get a buffer for reading MP3 data

	pWMADecoder->InitCallbackTable (&wma_callback_table);
	//pMpegDec->XEN_SetPower( false );			// eq off
	pMpegDec->XEN_SetPower( true );	
	pMpegDec->XEN_ClearSpectrumData();		// clear spectrum
	if (pWMADecoder->DecodeFile( (RMint8*)szPath) != WMA_DECODER_ERROR_NO_ERROR)
	{
		DEBUGMSG("pWMADecoder->DecodeFile (%s) failed\n", szPath);
		delete pWMADecoder;
		pWMADecoder = 0;
		deinit_buffer_pool (&BPwma);
		deinit_buffer_pool (&BPpcm);
		return false;
	}

	return true;
}

static bool DCFPlayInit( const char* szPath )
{
	if( pMP3Decoder == NULL ) {
		pMP3Decoder = (MP3Decoder *) new MP3Decoder;
	}
	init_buffer_pool( &BPmp3, mp3_buffer, MP3_BLOCK_SIZE, MP3_NBLOCKS );
	init_buffer_pool( &BPpcm, pcm_buffer, PCM_MP3_BLOCK_SIZE, PCM_MP3_NBLOCKS );
	
	mp3_callback_table.context = &BPmp3;		// saved context information for application
	mp3_callback_table.fopen = Melon_fopen;		// fopen for file decoding
	mp3_callback_table.fseek = Melon_fseek;		// fseek for file decoding
	mp3_callback_table.ftell = Melon_ftell;		// tell for file decoding
	mp3_callback_table.fread = Melon_fread;		// fread for file decoding
	mp3_callback_table.fclose = Melon_fclose;		// fclose for file decoding
	mp3_callback_table.addref = my_addref;		// addref a buffer
	mp3_callback_table.release = my_release;	// release a buffer
	mp3_callback_table.info = dcf_info;			// inform app of some mp3 information
	mp3_callback_table.putPCM = mp3_putPCM;		// output PCM bytes
	mp3_callback_table.getPCM = mp3_getPCM;		// get a buffer for PCM data
	mp3_callback_table.getMP3 = mp3_getMP3;		// get a buffer for reading MP3 data
	mp3_callback_table.reachEof = Reach_Eof;		// talk reaching eof	power save
	
	pMP3Decoder->Init ();
	pMP3Decoder->InitCallbackTable (&mp3_callback_table);
	
	pMP3Decoder->EnableMP3BufferQ ( true );		// power save
//	Clear_Eof();									// power save

	pMpegDec->XEN_SetPower( true );				// eq on
	pMpegDec->XEN_ClearSpectrumData();			// clear spectrum

	if (pMP3Decoder->DecodeFile( (char*)szPath ) != MP3_DECODER_ERROR_NO_ERROR)
	{
		DEBUGMSG( "failed to decode\n" );
 		//prebuffering = 1;
		//prop.playtime = 0;
		delete pMP3Decoder;
		pMP3Decoder = 0;
		deinit_buffer_pool (&BPmp3);
		deinit_buffer_pool (&BPpcm);
	
		return false;
	}
	return true;
}


// PlayInit
// Initialize MP3/OGG/WMA decoder and playlist



static int StopAB( struct ABInfo* ab )
{
	prop.ab = 0;

	return 0;
}

bool CMD_STOP( void )
{
	DEBUGMSG( "CMD_STOP\n" );

	switch( prop.media ) {
	case MEDIA_MP3:
	case MEDIA_DCF_MP3:
		MP3Stop();
		break;
	case MEDIA_OGG:
		OGGStop();
		break;
	case MEDIA_WMA:
		WMAStop();
		break;
	}

	if( pMpegDec ) {
        pMpegDec->Stop();
    }

	prop.playstate = AUDIO_STATE_STOP;
	g_msgFlag = 0;

	NumberOfBitsPerSample = 0;
	NumberOfChannels = 0;
	SamplesPerSecond = 0;
	Bitrate = 0;

	// clear playinfo
	prop.playtime = 0;
	prop.mpegtime = 0;
	prop.totaltime = 0;
	
	// clear AB
	prop.ab = 0;
	prop.ab_stime = 0;
	prop.ab_etime = 0;

	return true;
}

bool CMD_PLAY( struct PlayInfo* pinfo )
{
	bool success = false;

    DEBUGMSG( "CMD_PLAY\n" );
    
	CMD_STOP();
	
	// WAKE UP HDD.
	// WakeUp();
	
	prop.start = true;
	prop.error = AUDIO_ERR_NOERROR;
	prop.fatfserr = FATFS_ERROR_NO_ERROR;

	NumberOfBitsPerSample = 0;
	NumberOfChannels = 0;
	SamplesPerSecond = 0;
	Bitrate = 0;
	prop.playtime = 0;
	pMpegDec->SetSTC( 0, 0 );
	prop.media = CheckMedia( (char*)pinfo->szFilename, true );
	switch( prop.media ) {
	case MEDIA_MP3:
		success = MP3PlayInit( pinfo->szFilename );
		break;
	case MEDIA_OGG:
		success = OGGPlayInit( pinfo->szFilename );
		break;
	case MEDIA_WMA:
		success = WMAPlayInit( pinfo->szFilename );
		break;
	case MEDIA_DCF_MP3:
		success = DCFPlayInit( pinfo->szFilename );
		break;
	default:
		prop.error = AUDIO_ERR_NOTSUPPORT;
	}

	if( success ) {
		prop.playstate = AUDIO_STATE_PLAYING;
		g_msgFlag = IPC_NOWAIT;
		s_ackinfo.media = prop.media;
		ParseTag( (uchar_t*)pinfo->szFilename );
		ParseTotalTime( (uchar_t*)pinfo->szFilename );
		memcpy( cmd.szArg, &s_ackinfo, sizeof(s_ackinfo) );
	}

	int a_cmd;
	if( prop.error ) {
		int* suberr = ( int* )cmd.szArg;
		*suberr = prop.fatfserr;
	}

	return success;
}

bool CMD_PAUSE( void ) 
{
    DEBUGMSG( "CMD_PAUSE(0x%x)\n", prop.playstate );
    
	if( prop.playstate != AUDIO_STATE_PLAYING ) {
		prop.error = AUDIO_ERR_PAUSE;
		return false;
	}
	
	pMpegDec->Pause();
	g_msgFlag = 0;
	prop.playstate = AUDIO_STATE_PAUSE;

	return true;
}

bool CMD_RESUME( void )
{
    DEBUGMSG( "CMD_RESUME(0x%x)\n", prop.playstate );
    
	if( prop.playstate != AUDIO_STATE_PAUSE ) {
		prop.error = AUDIO_ERR_RESUME;
		return false;
	}
	
	prop.playstate = AUDIO_STATE_PLAYING;
	g_msgFlag = IPC_NOWAIT;
	prebuffering = 1;

	return true;
}

bool CMD_REMOUNT( void )
{
	my_fat_exit();
	if( my_fat_mount() < 0 ) {
		printf( "failed to mount\n" );
		return false;
	}

	return true;
}

bool CMD_AB( struct ABInfo* ab )
{
	prop.ab_stime = ab->lStartTime;
	prop.ab_etime = ab->lEndTime;
	prop.ab = 1;

	prop.playtime = prop.ab_stime;
	switch( prop.media ) {
	case MEDIA_MP3:
	case MEDIA_DCF_MP3:
		pMP3Decoder->Seek( prop.playtime );
		break;
	case MEDIA_OGG:
		pOGGDecoder->Seek( prop.playtime, NULL );
		break;
	case MEDIA_WMA:
		pWMADecoder->Seek( prop.playtime, NULL );
		break;
	}
	pMpegDec->Stop();
	pMpegDec->Pause();
	prebuffering = 1;

	return true;
}

bool CMD_SEEK( struct PlayInfo* pinfo )
{
	int seektime;
	int rtime;
	int err;
	
	if( prop.playstate == AUDIO_STATE_STOP ) {
		prop.error = AUDIO_ERR_SEEK;
		return false;
	}
	if( prop.totaltime == 0 ) {
		prop.error = AUDIO_ERR_SEEK;
		return false;
	}
	seektime = ((struct PlayInfo*)cmd.szArg)->lStartTime;
	if( prop.playtime < 0 ) {
		prop.playtime = 0;
	}
	else if( prop.playtime >= prop.totaltime-2 ) {
		CMD_STOP();
		SendMessage( AUDIO_CMD_INFO, AUDIO_STATE_STOP, AUDIO_ERR_NOERROR );
	}
	else {
		pMpegDec->Stop();
		switch( prop.media ) {
		case MEDIA_MP3:
		case MEDIA_DCF_MP3:
			err = pMP3Decoder->Seek( seektime );
			ClearBufferPool(&BPmp3);	// power save
			break;
		case MEDIA_OGG:
			err = pOGGDecoder->Seek( seektime, (RMuint32*)&rtime );
			break;
		case MEDIA_WMA:
			err = pWMADecoder->Seek( seektime, (RMuint32*)&rtime );
			break;
		}
		// if success
		if( !err ) 	prop.playtime = seektime;

		pMpegDec->Pause();
		prebuffering = 1;
	}

	if( err ) prop.error = AUDIO_ERR_SEEK;
	
	return( err ? false : true );
}


bool CMD_HDD_WAKEUP( void )
{
    my_powerup();
    
    return true;
}

bool SEND_ACK( int command, bool success )
{
	switch( command ) {
	case AUDIO_CMD_PLAY:
		break;
	case AUDIO_CMD_PAUSE:
		cmd.lError = AUDIO_ERR_PAUSE;
		break;
	case AUDIO_CMD_RESUME:
		break;
	case AUDIO_CMD_AB_START:
		break;
	case AUDIO_CMD_SEEK:
		break;
	}

	s_ackinfo.cmd = command;
	s_ackinfo.done = success;
	s_ackinfo.suberr = prop.fatfserr;
	memcpy( (void*)cmd.szArg, (void*)&s_ackinfo, sizeof(s_ackinfo) );
	SendMessage( AUDIO_CMD_ACK, prop.playstate, prop.error );
	
	return true;
}

void MP3_Schedule( void )
{
	if (pMP3Decoder->Schedule () == MP3_DECODER_ERROR_FILE_DONE) {
		if (prebuffering) {
			prebuffering = 0;
			pMpegDec->Play ();
		}
		if (pMpegDec->EndOfStream (-1, 0) == MPEG_DECODER_ERROR_EOF) {
			prop.playstate = AUDIO_STATE_STOP;
		}
	}
	else {
		if( prop.ab ) {
			if( prop.playtime == prop.ab_etime ) {
				WakeUp();						// power save
				usleep(100000);					// power save
				prop.playtime = prop.ab_stime;
				if( pMP3Decoder->Seek( prop.ab_stime ) == 0 ) {
					ClearBufferPool(&BPmp3);	// power save
					prebuffering = 1;
					pMpegDec->Stop();
				}
			}
			return;
		}
	}
}

static void OGG_Schedule( void )
{
	if( pOGGDecoder->Schedule() == OGG_DECODER_ERROR_FILE_DONE ) {
		if (prebuffering) {
			prebuffering = 0;
			pMpegDec->Play();
		}
		if (pMpegDec->EndOfStream (-1, 0) == MPEG_DECODER_ERROR_EOF) {
			prop.playstate = AUDIO_STATE_STOP;
		}
	}
	else {
		if( prop.ab ) {
			if( prop.playtime == prop.ab_etime ) {
				prop.playtime = prop.ab_stime;
				if( pOGGDecoder->Seek( prop.ab_stime, NULL ) == 0 ) {
                                ClearBufferPool(&BPogg);
					prebuffering = 1;
					pMpegDec->Stop();
				}
			}
			return;
		}
	}
}

static void WMA_Schedule( void )
{
    if (pWMADecoder->Schedule () == WMA_DECODER_ERROR_FILE_DONE)
    {
        if (prebuffering)
        {
            prebuffering = 0;
            pMpegDec->Play ();
        }
        if (pMpegDec->EndOfStream (-1, 0) == MPEG_DECODER_ERROR_EOF) {
		prop.playstate = AUDIO_STATE_STOP;
        }
    }
    else {
        if( prop.ab ) {
            if( prop.playtime == prop.ab_etime ) {
                prop.playtime = prop.ab_stime;
                if( pWMADecoder->Seek( prop.ab_stime, NULL ) == 0 ) {
                    prebuffering = 1;
                    pMpegDec->Stop();
                }
            }
            return;
        }
    }
}

static void Schedule( void ) 
{	
	RMint64 stc;

	if( prop.playstate == AUDIO_STATE_PAUSE || 
	    prop.playstate == AUDIO_STATE_STOP ) return;

	pMpegDec->GetSTC( &stc, 1 );
	if( prop.mpegtime != stc ) {
		prop.mpegtime = stc;
		prop.playtime++;
		SendPlaybackInfo();
	}

	switch( prop.media ) {
	case MEDIA_MP3:
	case MEDIA_DCF_MP3:
		MP3_Schedule();
		break;
	case MEDIA_OGG:
		OGG_Schedule();
		break;
	case MEDIA_WMA:
		WMA_Schedule();
		break;
	}
	
	if( prop.playstate == AUDIO_STATE_STOP ) {
		prop.noinfo = false;
		CMD_STOP();
		DEBUGMSG("--- 1100th :: Schedule : playstate=0x%x, error=0x%x\n", prop.playstate, prop.error );

		if( prop.error == AUDIO_ERR_MELON ) {
			s_ackinfo.cmd = AUDIO_CMD_MELONPLAYINFO;
			s_ackinfo.done = false;
			s_ackinfo.suberr = prop.fatfserr;
			memcpy( (void*)cmd.szArg, (void*)&s_ackinfo, sizeof(s_ackinfo) );
			SendMessage( AUDIO_CMD_MELONPLAYINFO, prop.playstate, prop.error );
			}
		else {
			SendMessage( AUDIO_CMD_INFO, AUDIO_STATE_STOP, AUDIO_ERR_NOERROR );
			}
	}
	
}

static void Init( void )
{
	if( prop.init ) return;
	prop.init = true;
	prop.noinfo = false;

     // Initialize Msg queue
    qid = SysMSGInit( AUDIO_MSG_KEY );
    if( qid < 0 ) {
    	DEBUGMSG( "MusicPlayer: failed to SysMSGInit()\n" );
    	return;
    }

	// create an instance of the mpeg decoder
	pMpegDec = new MpegDecoder;
	pMpegDec->Init( false );
	
	my_fat_mount();

#if SHM_SCHEME
	if(( prop.MBshmid= shmget(MB_SHMKEY, MB_DATABUFFER_SIZE+MB_PCMBUFFER_SIZE, MB_SHMPERM |IPC_CREAT)) < 0) {
		DEBUGMSG( "MusicPlayer: failed shmget for Media Buffer\n" );
	}
	if(( databuffers = (RMuint32*)shmat( prop.MBshmid, (char *)0, 0)) == (RMuint32*)(-1) ) {
		DEBUGMSG( "MusicPlayer: failed shmgat for Media Buffer\n" );
	}
	else
	{
		mp3_buffer = databuffers;
		ogg_buffer = databuffers;
		wma_buffer = databuffers;
		pcm_buffer = (databuffers+(MB_DATABUFFER_SIZE)/4 );
		printf("-----------databuffers=0x%x\n",databuffers);
		DEBUGMSG( "MusicPlayer : shared memory databuffers= 0x%lx\n", (int)databuffers);
	}
#endif	// SHM_SCHEME

	// insert following for resolving inital write error
	RMuint32 fd;
	fatfs_dopen( (RMuint8*)"/hd1", &fd );
	fatfs_dcreate_dir( fd, (RMuint8*)"Music" );
	fatfs_dclose( fd );
	
	pMP3Decoder = 0;
	pWMADecoder = 0;
	pOGGDecoder = 0;

   
}

// entry point
int main ( int argc, char *argv[] )
{
	struct PlayInfo* pinfo;
	bool ret;
	int command;
	int iNeedadustTime;
	int iUpdateParam;
	
	::buf = (QBUF_t*)&cmd;
	
	prop.playstate = AUDIO_STATE_READY;
	g_msgFlag = 0;

	Init();
	MELON_Init(argv[1]);
	
	while( 1 )
	{
		cmd.lLength = sizeof( struct AudioCmd ) -4;
		if( SysMSGRecv( qid, ::buf, MSG_RECV_ID, g_msgFlag ) != QError_NOERROR ) goto __skip;
		ret = true;
		command = cmd.lCmd;
		memset( (void*)&s_ackinfo, 0, sizeof(AckInfo) );
		switch( command ) {
		case AUDIO_CMD_REMOUNT:
			ret = CMD_REMOUNT();
			DEBUGMSG("--- 12th :: MusicPlayer : REMOUNT : DeleteDID, MoveCluster\n");
			MELON_DeleteDID();
			iNeedadustTime = MELON_MoveCluster();
			if ( iNeedadustTime == 1 ) {
				MELON_SaveNonce();
				if (MELON_TimeAdjust() == 0 ) {
					MELON_DrmTimeInfo( 1 );
					}
				}
			break;
		case AUDIO_CMD_INIT:
			break;
		case AUDIO_CMD_AB_START:
			ret = CMD_AB( (struct ABInfo*)cmd.szArg );
			break;
		case AUDIO_CMD_AB_STOP:
			StopAB( (struct ABInfo*)cmd.szArg );
			break;
		case AUDIO_CMD_PLAY:
			ret = CMD_PLAY( (struct PlayInfo*)cmd.szArg );
			break;
		case AUDIO_CMD_PAUSE:
			ret = CMD_PAUSE();
			break;
		case AUDIO_CMD_RESUME:
			ret = CMD_RESUME();
			break;
		case AUDIO_CMD_STOP:
			ret = CMD_STOP();
			break;
		case AUDIO_CMD_SEEK:
			ret = CMD_SEEK( (struct PlayInfo*)cmd.szArg );
			break;
		case AUDIO_CMD_NOINFO:
			prop.noinfo = true;
			break;
	    	case AUDIO_CMD_HDD_WAKEUP:
		// This command is not used on playing music.
		// So, we needn't send an ack.
	       	CMD_HDD_WAKEUP();
			continue;
		case AUDIO_CMD_MELONUPDATEINFO:
			DEBUGMSG("--- 90th :: MusicPlayer : AUDIO_CMD_MELONUPDATEINFO received\n" );
			MELON_DrmTimeInfo( 0 );
			continue;
		case AUDIO_CMD_MELONCREATEDID:
			DEBUGMSG("--- 91th :: MusicPlayer : AUDIO_CMD_MELONCREATEDID received \n");
			ret = MELON_CreateDID();
			ret = ( ret==0?1:0);
			break;		// for send ack msg
		}
		SEND_ACK( command, ret );
__skip:
		Schedule();
	}

	delete pMP3Decoder;
	delete pMpegDec;

	MELON_Destroy();
	
	SysMSGCtrl( qid, Q_REMOVE, NULL );
	
	return 0;
}



