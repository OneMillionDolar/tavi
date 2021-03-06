//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : PlayerWnd class declaration
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/PlayerWnd.h,v $
// $Revision: 1.4 $
// $Date: 2006/06/12 04:31:00 $
// $Author: ywkim $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: PlayerWnd.h,v $
// Revision 1.4  2006/06/12 04:31:00  ywkim
// Add function of Process_Melon()
//
// Revision 1.3  2006/06/12 03:06:51  zstein
// - delay on add-to-playlist.
//
// Revision 1.2  2006/06/07 08:47:40  zstein
// *** empty log message ***
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.42  2005/12/16 09:58:12  zstein
// *** empty log message ***
//
// Revision 1.41  2005/12/10 06:40:58  zstein
// *** empty log message ***
//
// Revision 1.40  2005/12/07 09:34:54  zstein
// *** empty log message ***
//
// Revision 1.39  2005/12/01 01:04:08  zstein
// *** empty log message ***
//
// Revision 1.38  2005/11/28 00:50:18  zstein
// *** empty log message ***
//
// Revision 1.37  2005/11/25 10:21:58  zstein
// *** empty log message ***
//
// Revision 1.36  2005/11/22 09:06:54  zstein
// *** empty log message ***
//
// Revision 1.35  2005/11/17 04:24:50  zstein
// *** empty log message ***
//
// Revision 1.34  2005/11/08 07:53:20  zstein
// *** empty log message ***
//
// Revision 1.33  2005/11/08 01:20:34  zstein
// *** empty log message ***
//
// Revision 1.30  2005/10/19 02:11:45  ywkim
// Change member funcitons for 3Dsound
//
//*******************************************************************************
#ifndef __PLAYER_WINDOW_H__
#define __PLAYER_WINDOW_H__

#include <Component/window.h>
#include <Component/Pixmap.h>
#include <Component/Label.h>
#include <Component/String.h>
#include <Component/ProgressBar.h>
#include <Component/ContextMenu.h>
#include <Component/ModalWnd.h>
#include <tavi_lib.h>
#include <Task/tavi_mq.h>
#include "PlayBottomWnd.h"
#include "AudioCmd.h"
#include "AudioPlayer.h"
#include "FolderPL.h"
#include "MusicDBPL.h"

#define ON_ADDTOPLAYLIST	ON_MESSAGE( 0x200<<16 )

typedef void( *PFN_Switch )( void );

#define AB_NONE			0
#define AB_A				1
#define AB_AB			2

class CPlayerWnd : public CWindow
{
friend class CPlayerBottomWnd;
private:
	void Init( void );
	void ProcessEvent( const TMsgEntry& msg );
	void DrawTouchMenu( void );
protected:
	//CPlayerBottomWnd* m_puzBottomWnd;
	CPixmap* m_puzImage;
	CPixmap* m_pmAlbum;
	CPixmap* m_pmArtist;
	CPixmap* m_pmTitle;
	CLabel* m_puzTitle; 
	CLabel* m_puzArtist;
	CLabel* m_puzAlbum;
	CLabel* m_puzInfo;
	//CLabel* m_puzCount;
	CModalWnd* m_pModalWnd;
	CMsgBox* m_pMsgBox;

	int m_nStart;
	int m_nCurrent;
	int m_nTotal;
	int m_nTotalTime;
	int m_playindex;
	CString m_szPlayfile;
	CString m_szPlaypath;
	int m_nAB;
	int m_ab_stime;
	int m_ab_etime;
	int m_nType;
	unsigned long m_nKey;
	int m_nErrCnt;
	int m_vbr;
	int m_media;
	int m_samplingrate;
	int m_bitrate;
	int m_eq_save;
	bool m_eq_enable;
	bool m_eq_block;
	bool m_cmd_ready;

	unsigned long Process_Melon( int err );
	void Process_ACK( AudioCmd* pcmd );
	void Process_Err( AudioCmd* pcmd, int error );
	void ProcessQueue( QBUF_t* pBuf );
	
	void OnPaint( const CRect& rc );
	void OnKeypressed( const KeyEvent& evt );
	void OnKeyReleased( const KeyEvent& evt );
	void OnMouseEvent( const MouseEvent& evt );
	void OnWindowEvent( const WindowEvent& evt );

	void SendPlayCmd( const uchar_t* filename );
public:
	CPlaylist* m_pPL;
	int m_state;
	int m_nPlayType;
	AudioCmd m_uzCmd;
	QID_t m_uzQID;
	CContextMenu* m_pCtxMenuWnd;
	CPlayerBottomWnd* m_puzBottomWnd;
	typedef enum {
		MUSIC_ERROR_NO_ERROR,
		MUSIC_ERROR_PLAYLIST_OPEN,
		MUSIC_ERROR_INVALID_MEDIA
	} MUSIC_ERROR_t;
	CPlayerWnd( CWindow* parent );
	~CPlayerWnd( void );

	void SetFileCount( int nCurrent, int nTotal, bool fUpdate );
	void SetMediaInfo( int nSamplingRate, int nBitrate, bool vbr, bool fUpdate );
	//void SetMP3Info( AckInfo* pAck, bool visible );
	MUSIC_ERROR_t SetPlaylist( const char* szPlaylistPath, int nStart, int nTotal );
	void Play( int type, const CString& path, CDeviceMusicDB* handle, const CString& filename );
	bool Prev( void );
	bool Next( void );
	void Seek( int sec, int dir );
	void Pause( void );
	void Resume();
	QID_t GetQID( void ) { return m_uzQID; };
	void Show( void );
	void Hide( void );
	int GetCurrentTime( void ) { return m_puzBottomWnd->GetCurrentTime(); }
	int GetPlayState( void ) { return m_state; }
	void IncVol( void );
	void DecVol( void );
	void SetEQ( int nEQ=-1 );
	void CreateContextMenu( void );
	void SendCmd( int cmd );
	void ShowPreset( void );
	void Set2BandEQ( int row, int sel );
	void Set5BandEQ( int sel );
	void SetFocus( void );
	void EnableEQ( bool enable, int eq=SOUND_NORMAL );
 	void OnAddToPlaylist( void );

	static int MakeFolderTree( const CString& szRoot );
};

#endif /* __PLAYER_WINDOW_H__ */

