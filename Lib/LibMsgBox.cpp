//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : LibMsgBox Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibMsgBox.cpp,v $
// $Revision: 1.4 $
// $Date: 2006/06/07 08:15:55 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibMsgBox.cpp,v $
// Revision 1.4  2006/06/07 08:15:55  zstein
// - removed unused codes.
//
// Revision 1.3  2006/05/09 10:46:26  ywkim
// Set choice = 2 in MsgProc() for cancel
//
// Revision 1.2  2006/04/07 08:15:57  ywkim
// Change many function for Host
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.30  2006/01/02 06:54:40  etnlwind
// Update for New Font Bug-Fix
//
// Revision 1.29  2005/12/30 04:24:41  etnlwind
// Update for New Font Adatio
//
// Revision 1.28  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.27  2005/12/19 06:40:56  etnlwind
// Update for Reset parameters
//
// Revision 1.26  2005/12/05 08:34:29  etnlwind
// Update for VIdeoPlayback
//
// Revision 1.25  2005/11/23 09:33:53  etnlwind
// Update for TAVI-EQ
//
// Revision 1.24  2005/11/21 12:00:29  etnlwind
// Update for AlarmClock & Sound Palette UI
//
// Revision 1.23  2005/11/16 08:48:49  etnlwind
// Update for AlarmSet
//
// Revision 1.22  2005/10/28 11:48:40  etnlwind
// Update for Dbx Btn BugFix
//
// Revision 1.21  2005/10/28 11:15:15  etnlwind
// Update for Palette
//
// Revision 1.20  2005/10/25 07:09:58  etnlwind
// Update for Palette Adjust
//
// Revision 1.19  2005/10/21 05:35:34  etnlwind
// Update for Booting Progress
// Update for Booting Progress, Alarm MsgBox
//
// Revision 1.18  2005/10/18 10:21:36  etnlwind
// Update for Dbx Auto Limiter
//
// Revision 1.17  2005/10/18 09:05:13  etnlwind
// Update for New DialgBox, MessageBox UI
//
// Revision 1.16  2005/09/27 09:11:07  etnlwind
// Update for VideoPlayer, FileManager
//
// Revision 1.15  2005/08/29 10:38:25  etnlwind
// Update for FileManager
//
// Revision 1.14  2005/08/24 13:07:55  etnlwind
// Update for Virtual Keyboard
//
// Revision 1.13  2005/08/22 07:16:11  etnlwind
// Update for
// 1. Video Player Clock Added.
// 2. Clock Draw Func. Added on TopPannel Class.
// 3. DrawUC16Text DT_TOP_PANNEL option Added.
//
// Revision 1.12  2005/08/22 01:22:37  etnlwind
// Update for File Browser Long File Name Scroll And Sub Menu
//
// Revision 1.11  2005/08/19 11:20:04  etnlwind
// Update for __ATTRIBUTE_PACKED__ and FileBrowser Delete Func.
//
// Revision 1.10  2005/08/18 09:34:32  etnlwind
// Update for VideoPlayer UserEQ Setup Related
//
// Revision 1.9  2005/08/17 03:22:22  etnlwind
// Update for FileManager
//
// Revision 1.8  2005/08/08 10:49:41  etnlwind
// Update for FileManger, VideoPlayer
//
// Revision 1.7  2005/07/13 06:02:31  etnlwind
// Update for LibSubMenu Bug Fix And VideoPlayer Brightness-Contrast Dlg Added
//
// Revision 1.6  2005/07/04 21:03:17  etnlwind
// Update for SubMenu Class
//
//
//*****************************************************************************


#include <string.h>
#include <Component/object.h>
#include <Component/image.h>

#include <Lib/LibImage.h>
#include <Lib/LibScrollBar.h>
#include <Lib/LibDialogBox.h>
#include <Lib/LibBookmarkDialogBox.h>
#include <Lib/LibSetupDialogBox.h>
#include <Lib/LibControlPannel.h>
#include <Lib/LibListBox.h>
#include <Lib/LibFileBrowser.h>


#include "LibDefines.h"
#include "LibMsgBox.h"

extern CTextEngine_BDF* pT;

#define COLORINDEX_MSGBOX_TITLE_BG 217

U16 TopLeftRound[4][4] = 
{
0,0,0,0,
0,0,1,1,
0,1,COLORINDEX_MSGBOX_TITLE_BG,COLORINDEX_MSGBOX_TITLE_BG,
0,1,COLORINDEX_MSGBOX_TITLE_BG,COLORINDEX_MSGBOX_TITLE_BG
};

U16 TopRightRound[4][4] = 
{
0,0,0,0,
1,1,0,0,
COLORINDEX_MSGBOX_TITLE_BG,COLORINDEX_MSGBOX_TITLE_BG,1,0,
COLORINDEX_MSGBOX_TITLE_BG,COLORINDEX_MSGBOX_TITLE_BG,1,0,
};

U16 BottomLeftRound[4][4] = 
{
0,1,_WHITE,_WHITE,
0,1,_WHITE,_WHITE,
0,0,1,1,
0,0,0,0
};

U16 BottomRightRound[4][4] = 
{
_WHITE,_WHITE,1,0,
_WHITE,_WHITE,1,0,
1,1,0,0,
0,0,0,0,
};


U16 SelectedBtnRound[] = 
{
_WHITE,_WHITE,_WHITE,_WHITE,
_WHITE,_WHITE,MSGBTN_BG_SELECTED,MSGBTN_BG_SELECTED,
_WHITE,MSGBTN_BG_SELECTED,MSGBTN_BG_SELECTED,MSGBTN_BG_SELECTED,
_WHITE,MSGBTN_BG_SELECTED,MSGBTN_BG_SELECTED,MSGBTN_BG_SELECTED
};

U16 UnselectedBtnRound[] = 
{
_WHITE,_WHITE,_WHITE,_WHITE,
_WHITE,_WHITE,MSGBTN_BG_UNSELECTED,MSGBTN_BG_UNSELECTED,
_WHITE,MSGBTN_BG_UNSELECTED,MSGBTN_BG_UNSELECTED,MSGBTN_BG_UNSELECTED,
_WHITE,MSGBTN_BG_UNSELECTED,MSGBTN_BG_UNSELECTED,MSGBTN_BG_UNSELECTED
};



//////////////////////////////////////////////////////////////////////////////////////////////////
//
// CMsgBox Class Figure
//
//
CMsgBox::CMsgBox( 
					U32 riTitle, 
					U16 TitleBackgroundColor, 
					U32 riTextContents, 
					int w,
					int h,
					MSG_PROCESSOR pfpMsgProc,
					U32 option
					)
{
	TextResBuffer = NULL;
	Reset( riTitle, TitleBackgroundColor, pT->GetTextRes( riTextContents ), w, h, pfpMsgProc, option );
}

CMsgBox::CMsgBox( 
					U32 riTitle, 
					U16 TitleBackgroundColor, 
					uchar_t* wTextContents,
					int w,
					int h,
					MSG_PROCESSOR pfpMsgProc,
					U32 option
					)
{
	TextResBuffer = NULL;
	Reset( riTitle, TitleBackgroundColor, wTextContents, w, h, pfpMsgProc, option );
}

CMsgBox::CMsgBox( 
					U32 riTitle, 
					U16 TitleBackgroundColor, 
					char* cTextContents,
					int w,
					int h,
					MSG_PROCESSOR pfpMsgProc,
					U32 option
					)
{

	TextResBuffer = NULL;
	Reset( riTitle, TitleBackgroundColor, cTextContents, w, h, pfpMsgProc, option );
}

CMsgBox::~CMsgBox()
{
	if(TextResBuffer)
		free(TextResBuffer);
	
	pMpegDecoder->RenderText( 0, 0, 0, 30, 320, 210 );
}


void CMsgBox::Reset( 
					U32 riTitle, 
					U16 TitleBackgroundColor, 
					U32 riTextContents, 
					int w,
					int h,
					MSG_PROCESSOR pfpMsgProc,
					U32 option
					)
{
	Reset( riTitle, TitleBackgroundColor, pT->GetTextRes( riTextContents ), w, h, pfpMsgProc, option );
}

void CMsgBox::Reset( 
					U32 riTitle, 
					U16 TitleBackgroundColor, 
					uchar_t* wTextContents,
					int w,
					int h,
					MSG_PROCESSOR pfpMsgProc,
					U32 option
					)
{

	TitleRes			= riTitle;
	TitleBG			= TitleBackgroundColor;
	TextRes			= wTextContents;
	pText			= NULL;

	if( w == -1 )	width = DL_DEFAULT_W;
	else			width = w;

	if( h == -1 )	height = DL_DEFAULT_H;
	else			height = h;
	
	fpMsgBoxMsgProc	= pfpMsgProc;
	BoxOptions 		= option;
	
	sx				= (160 - (width/2)) -1;
	sy 				= (120 - (height/2)) -1;
	
}
void CMsgBox::Reset( 
					U32 riTitle, 
					U16 TitleBackgroundColor, 
					char* cTextContents,
					int w,
					int h,
					MSG_PROCESSOR pfpMsgProc,
					U32 option
					)
{

	TitleRes			= riTitle;
	TitleBG			= TitleBackgroundColor;
	TextRes			= NULL;
	pText 			= cTextContents;

//	sprintf( pText, "%s", cTextContents );

	if( w == -1 )	width = DL_DEFAULT_W;
	else			width = w;

	if( h == -1 )	height = DL_DEFAULT_H;
	else			height = h;
	
	fpMsgBoxMsgProc	= pfpMsgProc;
	BoxOptions 		= option;
	
	sx				= (160 - (width/2)) -1;
	sy 				= (120 - (height/2)) -1;
	
}

void CMsgBox::BuildDbxBackground()
{
	// Background
	int sy1;

	if( (DL_ALARM_MBX_MODE&BoxOptions) == DL_ALARM_MBX_MODE  )
		{
		sy1 = sy+8;
		}
	else
		{
		sy1 = sy;
		}
	
	DrawBoxWithoutRefresh( _BLACK, sx+4, sy1, width-8, 1); 
	DrawBoxWithoutRefresh( COLORINDEX_MSGBOX_TITLE_BG, sx+4, sy1+1, width-8, 3);
	DrawBoxWithoutRefresh( COLORINDEX_MSGBOX_TITLE_BG, sx+1, sy1+2, width-2, 24 );
	DrawBoxWithoutRefresh( _BLACK, sx, sy1+4, 1, height-8);
	DrawBoxWithoutRefresh( _BLACK, sx+width-1, sy1+4, 1, height-8);

	for(int y=0; y<4; y++)
		for(int x=0; x<4; x++)
			DrawPixelWithoutRefresh( TopLeftRound[y][x] , sx+x, sy1+y);

	for(int y=0; y<4; y++)
		for(int x=0; x<4; x++)
			DrawPixelWithoutRefresh( TopRightRound[y][x] , sx+width-4+x, sy1+y);

	csx = sx+1;
	csy = sy1+1+24;
		
	DrawBoxWithoutRefresh( _WHITE, csx, csy, width-2, height-1-24-4 );
	DrawBoxWithoutRefresh( _WHITE, sx+4, csy+height-1-24-4, width-8, 3 );
	DrawBoxWithoutRefresh( _BLACK, sx+4, csy+height-2-24, width-8, 1 );


	for(int y=0; y<4; y++)
		for(int x=0; x<4; x++)
			DrawPixelWithoutRefresh( BottomLeftRound[y][x] , sx+x, sy1+height-4+y);

	for(int y=0; y<4; y++)
		for(int x=0; x<4; x++)
			DrawPixelWithoutRefresh( BottomRightRound[y][x] , sx+width-4+x, sy1+height-4+y);

	if( (DL_BG_ICON&BoxOptions) == DL_BG_ICON )
		DrawItemWithoutRefresh( &TAVI_IMAGES[ICON_DITTO], 0, csx + (width/2-31), csy + ((height-24)/2-31), 60, 60 );

	else if( (DL_BG_ETHERNET&BoxOptions) == DL_BG_ETHERNET  )
		DrawItemWithoutRefresh( &TAVI_IMAGES[BG_ETHERNET_CONNECT], 0, csx + (width/2-67), csy + 15, 132, 43 );

	else if( (DL_BG_HOST&BoxOptions) == DL_BG_HOST  )
		DrawItemWithoutRefresh( &TAVI_IMAGES[BG_HOST_CONNECT], 0, csx + (width/2-67), csy + 15, 132, 43 );

	else if( (DL_ALARM_MBX_MODE&BoxOptions) == DL_ALARM_MBX_MODE  )
		DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[IMAGE_CLOCK], 0, CSX_CLOCK, CSY_CLOCK, W_CLOCK, H_CLOCK );

}

void CMsgBox::BuildTitle( )
{

	int sy1;
	
	if( (DL_ALARM_MBX_MODE&BoxOptions) == DL_ALARM_MBX_MODE  )
		{
		sy1 = sy+8;
		}
	else
		{
		sy1 = sy;
		}

	// Title
	if( TitleRes != 0 )
		{	
		pT->DrawResText( TitleRes, 0, sy1+6, _WHITE, 12, DT_TRANSPARENCY|DT_BOLD|DT_ALIGN_CENTER );
		}
}

#define PROGRESSBAR_WIDTH 200
#define PROGRESSBAR_HEIGHT 15
void CMsgBox::BuildProgressBar( int percentage )
{
	char szNum[4];
	
	DrawBoxWithoutRefresh( _BLACK, psx-1, psy-1, 202, 17 );
	DrawBoxWithoutRefresh( _GRAY, psx, psy, 200, 15 );

	if(percentage>0)
		{
		if( percentage > 100 )
			percentage = 100;
		DrawBoxWithoutRefresh( _GREEN, psx, psy, percentage*2, 15 );

		}
	sprintf(szNum, "%d%%", percentage );
	pT->DrawEUC8Text(szNum, 0, psy+3, _WHITE, 12, DT_TRANSPARENCY|DT_BOLD|DT_ALIGN_CENTER);

	pMpegDecoder->RefreshOsd();
}

void CMsgBox::BuildProgressContents( int mode, uchar_t* uFilename, uchar_t* uSrcPath, uchar_t* uDesPath )
{
	uchar_t FolderName[128];
	int a_psy = csy+8;
	DrawBoxWithoutRefresh( _WHITE, csx, a_psy, width-2, psy-a_psy-1 );
	GetLastItem_In_UC16FullDirString_WithUserDefineSplitter( FolderName, uSrcPath, 0x002F );
	pT->DrawUC16Text( FolderName, psx, a_psy, _BLACK, _SHADOW, 12, psx+200, DT_TRANSPARENCY|DT_PROGRESS_DBX );
	pT->DrawUC16Text( uFilename, psx, a_psy+14, _BLACK, _SHADOW, 12, psx+200, DT_TRANSPARENCY|DT_PROGRESS_DBX );
}



void CMsgBox::BuildContents()
{
	// message
	if( (BoxOptions&DL_BTN_OK_ONLY) == DL_BTN_OK_ONLY ||
		(BoxOptions&DL_BTN_CANCEL_ONLY) == DL_BTN_CANCEL_ONLY ||
		(BoxOptions&DL_BTN_OK_CANCEL) == DL_BTN_OK_CANCEL )
		RelativeHeight = height - DBBTN_H - 12;
	else
		RelativeHeight = height;


	if( (BoxOptions&DL_PROGRESS_MODE) == DL_PROGRESS_MODE )
		{
		psx = sx+(width/2 - (PROGRESSBAR_WIDTH+2)/2);
		psy = sy+(height/2 - (PROGRESSBAR_HEIGHT+2)/2)+14;

		// 프로그래스바 프레임 출력
		//BuildProgressBar(0);

		}

	else if( (BoxOptions&DL_BG_HOST) == DL_BG_HOST 
		|| (BoxOptions&DL_BG_ETHERNET) == DL_BG_ETHERNET)
		{
/*
		printf("((((( D_BG_.... ))))\n");
		printf(" pT->GetStringWidth(TextRes, 12, DT_TRANSPARENCY )  : %d\n",  pT->GetStringWidth(TextRes, 12, DT_TRANSPARENCY ));
		printf("width: %d  csx: %d\n", width, csx );
*/
		pT->DrawUC16Text( TextRes, csx+(width/2 - pT->GetStringWidth(TextRes, 12, DT_TRANSPARENCY )/2), sy+height-24, _BLACK, 12, DT_TRANSPARENCY );
		}

	else if( (BoxOptions&DL_ALARM_MBX_MODE) == DL_ALARM_MBX_MODE )
		{
		DrawASCIITextWithoutRefresh( pText, W_CLOCKDIGIT, H_CLOCKDIGIT, 
			&TAVI_IMAGES[ALARM_DIGIT], CSX_CLOCKDIGIT, CSY_CLOCKDIGIT, ALARM_DIGIT );
		}
	
	else
		{

		int sl, lsp;
		uchar_t* sp;
		sdc = pT->GetStringDividerCount( TextRes );
		sl = ustrlen( TextRes );
		lsp =  csy+((RelativeHeight-24)/2 - 12/2) - ((sdc*14)/2);
		
//		if( tcw < (width-20) )
		if( sdc )
			{
			uchar_t buffer[128];

			// Splited Long Text
			//printf(" (((((((( SPLIT LONG TEXT ))))))))\n");

			for( int loop=0; loop <= sdc ; loop++ )
				{

				sp = pT->SetNthStringWithDivider( buffer, TextRes, 0x007c, loop );
				//pT->DrawUC16Text( sp, csx+(240/2 - pT->GetStringWidth(sp, 12, DT_TRANSPARENCY )/2), lsp+(loop*14) , _BLACK, 12, DT_TRANSPARENCY );
				pT->DrawUC16Text( sp, 0, lsp+(loop*14) , _BLACK, 12, (DT_TRANSPARENCY|DT_ALIGN_CENTER) );
/*
				int temp; pT->DrawEUC8LineText("Do you really want to delete", 78, 61, _BLACK, _WHITE, 12, (DT_TRANSPARENCY), &temp);
				pT->DrawEUC8LineText("Do you really want to delete", 82, 73, _BLACK, _WHITE, 12, (DT_TRANSPARENCY), &temp);
				pT->DrawEUC8LineText("Do you really want to delete", 87, 85, _BLACK, _WHITE, 12, (DT_TRANSPARENCY), &temp);

				DrawBoxWithoutRefresh(_GREEN, (320-142)/2, 170, 142, 20);
				DrawBoxWithoutRefresh(_GREEN, (320-180)/2, 190, 180, 20);
				DrawBoxWithoutRefresh(_GREEN, (320-210)/2, 210, 210, 20);
*/

				/*
				uchar_t usc[256];
				char sc[256];
				str2ustr( usc, "Do you really want to delete" );
				CSize szc;
				int cx;
				for(int l=0; l<strlen("Do you really want to delete"); l++ )
					{
					pT->GetCharSize(usc[l], 12, &szc);
					sprintf( sc, "%s+%d", sc, szc.width );
					}

				pT->DrawEUC8LineText(sc, 0, 65, _BLACK, _WHITE, 12, (DT_TRANSPARENCY), &temp);
				*/

#if 0
				for(int i=0; i<sl; i++)
					{
					if( buffer[i] == 0x007c || buffer[i] == 0x0000 )
						{
						printf("|\n");
						break;
						}
					else
						printf("%c ", buffer[i]);
					}
#endif				
				}
			
			}
		else
			{

			tcw = pT->GetStringWidth(TextRes, 12, DT_TRANSPARENCY );

			//printf("((((( SINGLE LINE TEXT ))))\n");
			if( width > tcw )
				pT->DrawUC16Text( TextRes, csx+(width/2 - tcw/2), csy+((RelativeHeight-24)/2 - 12/2), _BLACK,  12, DT_TRANSPARENCY );
			else
				pT->DrawUC16Text( TextRes, csx+10, csy+((RelativeHeight-24)/2 - 12/2), _BLACK, 12, DT_TRANSPARENCY );
			
			}

		}

}

void CMsgBox::BuildButtonEdge( int isSelectedBtn, int BtnSx, int BtnSy, int BtnW, int BtnH )
{
	U16* Image;
	int x, y, _x, _y;

	if( isSelectedBtn )
		Image = SelectedBtnRound;
	else
		Image = UnselectedBtnRound;

	// TopLeft
	for(y=0; y<4; y++)
		for( x=0; x<4; x++)
			DrawPixelWithoutRefresh( Image[(y*4)+x] , BtnSx+x, BtnSy+y);

	// TopRight
	for(y=0; y<4; y++)
		for(x=0, _x=3; x<4; x++, _x--)
			DrawPixelWithoutRefresh( Image[(y*4)+_x] , BtnSx+BtnW-4+x, BtnSy+y);

	// BottomLeft
	for(y=0,_y=3; y<4; y++, _y--)
		for(int x=0; x<4; x++)
			DrawPixelWithoutRefresh( Image[(_y*4)+x] , BtnSx+x, BtnSy+BtnH-4+y);

	// BottomRight
	for(y=0,_y=3; y<4; y++, _y--)
		for(x=0, _x=3; x<4; x++, _x--)
			DrawPixelWithoutRefresh( Image[(_y*4)+_x] , BtnSx+BtnW-4+x, BtnSy+BtnH-4+y);

}

void CMsgBox::BuildButtons( int newchoice )
{
	U16 BtnOKBgColor;
	U16 BtnOKTextColor;
	U16 BtnCancelBgColor;
	U16 BtnCancelTextColor;
	U16 BtnOKEdgeColor;
	U16 BtnCancelEdgeColor;
	U32 BtnOkTextOpt;
	U32 BtnCancelTextOpt;
	U16 Btn1Res;
	U16 Btn2Res;

	int btn_tsx = sx + width/2 -DBBTN_W/2;
	int btn_tsy = sy + height-DBBTN_H -12;

	if( (BoxOptions&DL_BTN_OK_ONLY) == DL_BTN_OK_ONLY )
		{
		BtnOKBgColor = MSGBTN_BG_SELECTED;
		BtnOKTextColor = _WHITE;
		DrawBoxWithoutRefresh( BtnOKBgColor, btn_tsx, btn_tsy, DBBTN_W, DBBTN_H );
		BuildButtonEdge( TRUE, btn_tsx, btn_tsy, DBBTN_W, DBBTN_H );
		pT->DrawResText(ML_OK, btn_tsx+(DBBTN_W/2)-(pT->GetStringWidth(ML_OK, 12, 0)/2), sy+height-DBBTN_H -12+6, BtnOKTextColor, 12, DT_TRANSPARENCY );
		}

	else	 if( (BoxOptions&DL_BTN_CANCEL_ONLY) == DL_BTN_CANCEL_ONLY )
		{
		BtnOKBgColor = MSGBTN_BG_SELECTED;
		BtnOKTextColor = _WHITE;
		DrawBoxWithoutRefresh( BtnOKBgColor, btn_tsx, btn_tsy, DBBTN_W, DBBTN_H );
		BuildButtonEdge( TRUE, btn_tsx, btn_tsy, DBBTN_W, DBBTN_H );
		pT->DrawResText(ML_CANCEL, btn_tsx+(DBBTN_W/2)-(pT->GetStringWidth(ML_CANCEL, 12, 0)/2), sy+height-DBBTN_H -12+6, BtnOKTextColor, 12, DT_TRANSPARENCY );
		}
	
	else if( (BoxOptions&DL_BTN_OK_CANCEL) == DL_BTN_OK_CANCEL )
		{
		
		btn_tsx 	= sx + width/2 -(DBBTN_W*2+10)/2;
		choice 	= newchoice;

		if( choice == 1 ) // LEFT
			{
			BtnOKBgColor = MSGBTN_BG_SELECTED;			//_BLACK;
			BtnOKTextColor = _WHITE;
			BtnOKEdgeColor = TRUE;
			BtnOkTextOpt = (DT_TRANSPARENCY|DT_BOLD);
			
			BtnCancelBgColor = MSGBTN_BG_UNSELECTED;		//_GRAY;
			BtnCancelTextColor = _BLACK;
			BtnCancelEdgeColor = FALSE;
			BtnCancelTextOpt = (DT_TRANSPARENCY);			
			}

		else if( choice == 100 ) // No Swapping
			{
			BtnOKBgColor = MSGBTN_BG_SELECTED;			//_BLACK;
			BtnOKTextColor = _WHITE;
			BtnOKEdgeColor = TRUE;
			BtnOkTextOpt = (DT_TRANSPARENCY|DT_BOLD);
			
			BtnCancelBgColor = MSGBTN_BG_SELECTED;		//_BLACK;
			BtnCancelTextColor = _WHITE;
			BtnCancelEdgeColor = TRUE;
			BtnCancelTextOpt = (DT_TRANSPARENCY|DT_BOLD);
			}

		else
			{
			BtnOKBgColor = MSGBTN_BG_UNSELECTED;			//_GRAY;
			BtnOKTextColor = _BLACK;
			BtnOKEdgeColor = FALSE;
			BtnOkTextOpt = (DT_TRANSPARENCY);			
			
			BtnCancelBgColor = MSGBTN_BG_SELECTED;		// _BLACK;
			BtnCancelTextColor = _WHITE;
			BtnCancelEdgeColor = TRUE;
			BtnCancelTextOpt = (DT_TRANSPARENCY|DT_BOLD);
			}
		
		DrawBoxWithoutRefresh( BtnOKBgColor, btn_tsx, btn_tsy, DBBTN_W, DBBTN_H );
		BuildButtonEdge( BtnOKEdgeColor, btn_tsx, btn_tsy, DBBTN_W, DBBTN_H );
		DrawBoxWithoutRefresh( BtnCancelBgColor, btn_tsx+DBBTN_W+10, btn_tsy, DBBTN_W, DBBTN_H );
		BuildButtonEdge( BtnCancelEdgeColor, btn_tsx+DBBTN_W+10, btn_tsy, DBBTN_W, DBBTN_H );

		if( (BoxOptions&DL_ALARM_MBX_MODE) == DL_ALARM_MBX_MODE)
			{
			Btn1Res = ML_SNOOZE;
			Btn2Res = ML_CLOSE;
			}
		else
			{
			Btn1Res = ML_OK;
			Btn2Res = ML_CANCEL;
			}

		pT->DrawResText(Btn1Res, btn_tsx+(DBBTN_W/2)-(pT->GetStringWidth(Btn1Res, 12, BtnOkTextOpt)/2), sy+height-DBBTN_H -12+6, BtnOKTextColor, 12, BtnOkTextOpt );
		pT->DrawResText(Btn2Res , (btn_tsx+DBBTN_W+10)+(DBBTN_W/2)-(pT->GetStringWidth(Btn2Res, 12, BtnCancelTextOpt)/2), sy+height-DBBTN_H -12+6, BtnCancelTextColor, 12, BtnCancelTextOpt ); 
#if 0
		char temp[256];
		sprintf( temp, "btn_tsx:%d sw:%d  swby2:%d", 
			btn_tsx,
			pT->GetStringWidth(Btn2Res, 12, BtnCancelTextOpt), 
			(pT->GetStringWidth(Btn2Res, 12, BtnCancelTextOpt)/2) );
		pT->DrawEUC8Text(temp, 10, 200, _BLACK, _SHADOW, 12, 0 );



			CSize cxz;
			for( int x=0; x<ustrlen(usString); x++ )
				{
				GetCharSize( usString[x], fontsize, &cxz);
				printf("-0x%x : %d\n", usString[x], cxz.width );
				}
#endif


		
		}

}

int CMsgBox::CursorLeft()
{
	if( (BoxOptions&DL_BTN_OK_CANCEL) == DL_BTN_OK_CANCEL )
		{
		BuildButtons(1);
		pMpegDecoder->RefreshOsd();
		return choice;
		}
	else
		return -1;

}

int CMsgBox::CursorRight()
{
	if( (BoxOptions&DL_BTN_OK_CANCEL) == DL_BTN_OK_CANCEL )
		{
		BuildButtons(2);
		pMpegDecoder->RefreshOsd();
		return choice;
		}
	else
		return -1;
}

void CMsgBox::Show()
{
	ReDraw();
}


void CMsgBox::ReDraw( eOSD_REFRESH_OPTION refresh )
{
	BuildDbxBackground();
	BuildTitle();
	BuildContents();
	if( (BoxOptions&DL_OK) == DL_OK ) BuildButtons(1);
	else BuildButtons(2);
	if(refresh==OSD_REFRESH)
		pMpegDecoder->RefreshOsd();
	isVisible = TRUE;
}

int CMsgBox::MsgBtnUp( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	return ( fpMsgBoxMsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
}

int CMsgBox::MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	DEBUGMSG(1,("CMsgBox::MsgProc( %d, %d, %d, %d )\n", ssTaskId, ulMsgId, ssParam1, ssParam2 ));

	if( fpMsgBoxMsgProc != NULL )
		{
		DEBUGMSG(1,("-External MsgProc\n"));
		return ( fpMsgBoxMsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
		}
	else
		{
		DEBUGMSG(1,("-Internal MsgProc\n"));

		//////////////////////////////////////////////////////////////////////
		//	MsgBtnUp
		//	Key Message Process
		
		if( ulMsgId == MSG_BUTTON_UP )
			{

			switch( ssParam1 )
				{
				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_UP:
					break;

				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_DOWN:
					break;
					
				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_LEFT:
					if( (BoxOptions&DL_BTN_OK_CANCEL) == DL_BTN_OK_CANCEL )
						CursorLeft();
					break;

				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_EXIT:
					if( (BoxOptions&DL_BTN_OK_CANCEL) == DL_BTN_OK_CANCEL ) {
						choice = 2;
						return -1;
						}
					break;

				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_RIGHT:
					if( (BoxOptions&DL_BTN_OK_CANCEL) == DL_BTN_OK_CANCEL )
						CursorRight();
					break;

				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_ENTER:
					if( 	(BoxOptions&DL_BTN_OK_ONLY) == DL_BTN_OK_ONLY ||
						(BoxOptions&DL_BTN_CANCEL_ONLY) == DL_BTN_CANCEL_ONLY ||
						(BoxOptions&DL_BTN_OK_CANCEL) == DL_BTN_OK_CANCEL )
						return -1;
					break;
		
				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_MENU:
					break;
					
				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_LEXIT:				
				case TAVI_KEY_MODE:
				case TAVI_KEY_LMODE:
					choice = 2;
					return -1;
					break;

				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_SHUTDOWN:
					choice = 2;
					return -1;
					break;
				}
			}


		//////////////////////////////////////////////////////////////////////
		//	MsgBtnUp
		//	Touch Pad Message Process

		else if( ulMsgId == MSG_TOUCHPAD )
			{

			if( ssParam1 == -1 )
				{
				CursorLeft();
				}
			else if( ssParam1 == 1 )
				{
				CursorRight();
				}
			else if( ssParam2 == 1 )
				{
				CursorRight();
				}
			}

		return 0;
	}

}







