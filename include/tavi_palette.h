//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : TAVI Palette Define
// 
// $Source: /home/cvsroot/TAVI020/tavi/include/tavi_palette.h,v $
// $Revision: 1.2 $
// $Date: 2006/04/15 02:19:45 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: tavi_palette.h,v $
// Revision 1.2  2006/04/15 02:19:45  zstein
// - added yellowish green
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.17  2006/01/13 08:40:56  ywkim
// Increase value of _LEVELMETER_BASE to 240 from 239
//
// Revision 1.16  2005/11/17 11:18:16  zstein
// - LEVEL METER PALLETE
//
// Revision 1.15  2005/11/17 04:26:32  zstein
// *** empty log message ***
//
// Revision 1.14  2005/10/21 09:43:56  etnlwind
// Update for TextViwer Scroll, Booting Progress
//
// Revision 1.13  2005/10/21 05:37:16  etnlwind
// Update for Booting Progress
// Update for Booting Progress, Alarm MsgBox, Palette Management
//
// Revision 1.12  2005/10/13 01:25:25  etnlwind
// Update for Header Info
//
//*****************************************************************************

/*
 * TAVI-PMP Version 1.0
 * new media LIFE CO., LTD.
 * tavi_palette.h
 * Sang-jin Ryu (ew@newmedialife.com)
 * 
 */

#ifndef __TAVI_PALLETTE_H__
#define __TAVI_PALLETTE_H__

#define _TRANSPARENT	0
#define _BLACK			1
#define _WHITE			2
#define _YELLOW			3
#define _BLUE			4
#define _RED				5
#define _GREEN			6
#define _ORANGE			7
#define _GRAY			8
#define _LIGHTGREEN		9
#define _DARKGREEN		10

#define _GRAY0			18
#define _GRAY20			19
#define _GRAY40			20
#define _GRAY60			21
#define _GRAY80			22

#define _YELLOWISH_GREEN     211

#define _LEVELMETER_BASE		240
#define _LEVELMETER_BG			201
#define _LEVELMETER_UNIT_BG	202

//#define _ORANGE		100
//#define _GREEN			101

#define MAX_PALLETTE_USE 106

#if 1
/*
static unsigned char aubTaviPaletteTransparent[256];
static unsigned char aubVideoPlayerPaletteTransparent[256];
static unsigned char aubAudioPlayerPaletteTransparent[256];
static unsigned char aubRadioPaletteTransparent[256];
*/
#else
// MAIN PALETTE
static const unsigned char aubTaviPaletteTransparent[256] = {
///////////////////////////////////////////////////
// System Pallette
0x0,		// 0		transparent color
0xff,	// 1		white
0xff,	// 2		black
0x0,	// 3
0x0,	// 4
0x0,	// 5
0x0,	// 6
0x0,	// 7
0x0,	// 8
0x0,	// 9
0xB0,	// 10
0x0,	// 11
0x0,	// 12
0x0,	// 13
0x0,	// 14
0x0,	// 15
0x0,	// 16


///////////////////////////////////////////////////
// Start of NML-TAVI Pallette

// * Selected State

/*
// TEST PALETTE START
0x72,		//17
0x72,		//18
0x72,		//19
0x72,		//20
0x72,		//21
0x72,		//22
0x72,		//23
0x72,		//24
0x72,		//25
0xb2,		//26

0x99,		//27
0x99,		//28
0x99,		//29
0x99,		//30
0x99,		//31
0x99,		//32
0x99,		//33
0x99,		//34
0x99,		//35
0xff,		//36
// TEST PALETTE END

*/
// * Selected State
0xff,		//17
0xff,		//18
0xff,		//19
0xff,		//20
0xff,		//21
0xff,		//22
0xff,		//23
0xff,		//24
0xff,		//25
0xff,		//26

0xff,		//27
0x66,		//28
0x66,		//29
0x66,		//30
0x66,		//31
0x66,		//32
0x66,		//33
0x66,		//34
0x66,		//35
0x66,		//36
0x66,		//37
0x66,		//38
0x66,		//39
0x66,		//40
0x66,		//41
0x66,		//42
0x66,		//43
0x66,		//44
0x66,		//45
0x66,		//46
0x66,		//47



0x66,		//48
0x66,		//49
0x66,		//50
0x66,		//51
0x66,		//52
0x66,		//53
0x66,		//54
0x66,		//55
0x66,		//56
0x66,		//57
0x66,		//58
0x66,		//59
0x66,		//60
0x66,		//61
0x66,		//62
0x66,		//63
0x66,		//64
0x66,		//65
0x66,		//66
0x66,		//67
0x66,		//68
0x66,		//69
0x66,		//70
0x66,		//71
0x66,		//72
0x66,		//73




0x9A,		//74
0xff,		//75
0x9A,		//76
0xff,		//77
0x9A,		//78
0xff,		//79
0xff,		//80
0x9A,		//81
0x9A,		//82
0x9A,		//83
0x9A,		//84
0x9A,		//85
0x9A,		//86
0x9A,		//87
0x9A,		//88
0x9A,		//89
0x9A,		//90
0x9A,		//91
0x9A,		//92
0x9A,		//93
0x28,		//94
0x28,		//95



0x0A,		//96
0x0A,		//97
0x0A,		//98
0x0A,		//99
0xff,		//100	//0x0A,		//100
0xff,		//101	//0x14,		//101
0x14,		//102
0x28,		//103
0x2D,		//104
0x32,		//105
0x2D,		//106
0x3A,		//107
0x3E,		//108


0x0B,		//109
0x0E,		//110
0x11,		//111
0x17,		//112
0x1D,		//113
0x17,		//114
0x1F,		//115
0x1F,		//116
0x1F,		//117
0x1F,		//118
0x1F,		//119
0x1F,		//120
0x1F,		//121
0x02,		//122
0x05,		//123
0x08,		//124
0x0B,		//125
0x14,		//126
0x1A,		//127
0x1A,		//128
0x1F,		//129
0x1F,		//130
0x1F,		//131
0x1F,		//132
0x1A,		//133
0x1F,		//134
0x1F,		//135
0x1F,		//136
0x1F,		//137
0x2D,		//138

/*
0xB0,		//17
0xB0,		//18
0xB0,		//19
0xB0,		//20
0xB0,		//21
0xB0,		//22
0xB0,		//23
0xB0,		//24
0xB0,		//25
0xB0,		//26
0xB0,		//27
0xb2,		//28
0xff,		//29
0xb2,		//30
0xB0,		//31
0xB0,		//32
0xB0,		//33
0xB0,		//34
0xB0,		//35
0xB0,		//36
0xB0,		//37
0xB0,		//38
0xB0,		//39
0xB0,		//40
0xb2,		//41
0xB0,		//42
0xB0,		//43
0xB0,		//44
0xff,		//45
0xb2,		//46
0xB0,		//47
0xb2,		//48
0xff,		//49
0xff,		//50
0xff,		//51
0xff,		//52
0xff,		//53
0xff,		//54
0xff,		//55
0xff,		//56
0xff,		//57
0xff,		//58
0xff,		//59
0xff,		//60
0xff,		//61
0xff,		//62

// Middle of Select and Degree-1 State 
0xcc,		//63
0xcc,		//64
0xcc,		//65
0xcc,		//66
0xcc,		//67
0xcc,		//68
0xcc,		//69
0xcc,		//70
0xcc,		//71
0xcc,		//72
0x66,		//73

// Degree-1 State 
0x99,		//74
0x99,		//75
0x99,		//76
0x99,		//77
0x99,		//78
0x99,		//79
0x99,		//80
0x99,		//81
0x99,		//82
0x99,		//83
0x4c,		//84

// Middle of Select and Degree-1 State 
0x66,		//85
0x66,		//86
0x66,		//87
0x66,		//88
0x66,		//89
0x66,		//90
0x66,		//91
0x66,		//92
0x66,		//93
0x66,		//94
0x33,		//95

// Degree-2 State 
0x33,		//96
0x33,		//97
0x33,		//98
0x33,		//99
0x33,		//100
0x33,		//101
0x33,		//102
0x33,		//103
0x33,		//104
0x33,		//105
0x19,		//106

///////////////////////////////////////////////////
// Reserved Area
0xff,	//107
0xb2,	//108
0xB0,	//109
0xff,	//110
0xB0,	//111
0xB0,	//112
0x66,	//113
0xff,	//114
0xB0,	//115
0xff,	//116
0xff,	//117
0xff,	//118
0xff,	//119

0xff,	//120
0xff,	//121
0xff,	//122
0xff,	//123
0xff,	//124
0xff,	//125
0xff,	//126
0xff,	//127
0xff,	//128
0xff,	//129
0xff,	//130
0xff,	//131
0xff,	//132
0xff,	//133
0xff,	//134
0xff,	//135
0xff,	//136
0xff,	//137
0xff,	//138
*/
0x73,	//139
0xff,	//140
0xff,	//141
0xff,	//142
0xff,	//143
0xff,	//144
0xff,	//145
0xff,	//146
0xff,	//147
0xff,	//148
0xff,	//149
0xff,	//150
0xff,	//151
0xff,	//152
0xff,	//153
0xff,	//154
0xff,	//155
0xff,	//156
0xff,	//157
0xff,	//158
0xff,	//159
0xff,	//160
0xff,	//161
0xff,	//162
0xff,	//163
0xff,	//164
0xff,	//165
0xff,	//166
0xff,	//167
0xff,	//168
0xff,	//169
0xff,	//170
0xff,	//171
0xff,	//172
0xff,	//173
0xff,	//174
0xff,	//175
0xff,	//176
0xff,	//177
0xff,	//178
0xff,	//179
0xff,	//180
0xff,	//181
0xff,	//182
0xff,	//183
0xff,	//184
0xff,	//185
0xff,	//186
0xff,	//187
0xff,	//188
0xff,	//189
0xff,	//190
0xff,	//191
0xff,	//192
0xff,	//193
0xff,	//194
0xff,	//195
0xff,	//196
0xff,	//197
0xff,	//198
0xff,	//199
0xff,	//200
0xff,	//201
0xff,	//202
0xff,	//203
0xff,	//204
0xff,	//205
0xff,	//206
0xff,	//207
0xff,	//208
0xff,	//209
0xff,	//210
0xff,	//211
0xff,	//212
0xff,	//213
0xff,	//214
0xff,	//215
0xff,	//216
0xff,	//217
0xff,	//218
0xff,	//219
0xff,	//220
0xff,	//221
0xff,	//222
0xff,	//223
0xff,	//224
0xff,	//225
0xff,	//226
0xff,	//227
0xff,	//228
0xff,	//229
0xff,	//230
0xff,	//231
0xff,	//232
0xff,	//233
0xff,	//234
0xff,	//235
0xff,	//236
0xff,	//237
0xff,	//238
0xff,	//239
0xff,	//240
0xff,	//241
0xff,	//242
0xff,	//243
0xff,	//244
0xff,	//245
0xff,	//246
0xff,	//247
0xff,	//248
0xff,	//249
0xff,	//250
0xff,	//251
0xff,	//252
0xff,	//253
0xff,	//254
0xff	//255

};

static const unsigned char aubAudioPlayerPaletteTransparent[256] = {
	///////////////////////////////////////////////////
	// System Pallette
	0x0,		// 0		transparent color
	0x0,	// 1		white
	0x0,	// 2		black
	0x0,	// 3
	0x0,	// 4
	0x0,	// 5
	0x0,	// 6
	0x0,	// 7
	0x0,	// 8
	0x0,	// 9
	0x0,	// 10
	0x0,	// 11
	0x0,	// 12
	0x0,	// 13
	0x0,	// 14
	0x0,	// 15
	0x0,	// 16


	///////////////////////////////////////////////////
	// Start of NML-TAVI Pallette

	// * Selected State
	255,		//17
	255,		//18
	255,		//19
	255,		//20
	255,		//21
	255,		//22
	255,		//23
	77,		//24
	38,		//25
	13,		//26

	115,		//27
	115,		//28
	115,		//29
	255,		//30
	153,		//31
	115,		//32
	115,		//33
	255,		//34
	0xB0,		//35
	0xB0,		//36
	0xB0,		//37
	0xB0,		//38
	0xB0,		//39
	0xB0,		//40
	0xb2,		//41
	0xB0,		//42
	0xB0,		//43
	0xB0,		//44
	0xff,		//45
	0xb2,		//46
	0xB0,		//47
	0xb2,		//48
	0xff,		//49
	0xff,		//50
	0xff,		//51
	0xff,		//52
	0xff,		//53
	0xff,		//54
	0xff,		//55
	0xff,		//56
	0xff,		//57
	0xff,		//58
	0xff,		//59
	0xff,		//60
	0xff,		//61
	0xff,		//62

	// Middle of Select and Degree-1 State 
	0xcc,		//63
	0xcc,		//64
	0xcc,		//65
	0xcc,		//66
	0xcc,		//67
	0xcc,		//68
	0xcc,		//69
	0xcc,		//70
	0xcc,		//71
	0xcc,		//72
	0x66,		//73

	// Degree-1 State 
	0x99,		//74
	0x99,		//75
	0x99,		//76
	0x99,		//77
	0x99,		//78
	0x99,		//79
	0x99,		//80
	0x99,		//81
	0x99,		//82
	0x99,		//83
	0x4c,		//84

	// Middle of Select and Degree-1 State 
	0x66,		//85
	0x66,		//86
	0x66,		//87
	0x66,		//88
	0x66,		//89
	0x66,		//90
	0x66,		//91
	0x66,		//92
	0x66,		//93
	0x66,		//94
	0x33,		//95

	// Degree-2 State 
	0x33,		//96
	0x33,		//97
	0x33,		//98
	0x33,		//99
	0xff,		//100
	0xff,		//101
	0x33,		//102
	0x33,		//103
	0x33,		//104
	0x33,		//105
	0x19,		//106

	///////////////////////////////////////////////////
	// Reserved Area
	0xff,	//107
	0xb2,	//108
	0xB0,	//109
	0xff,	//110
	0xB0,	//111
	0xB0,	//112
	0x66,	//113
	0xff,	//114
	0xB0,	//115
	0xff,	//116
	0xff,	//117
	0xff,	//118
	0xff,	//119




	0xff,	//120

	0xff,	//121
	0xff,	//122
	0xff,	//123
	0xff,	//124
	0xff,	//125
	0xff,	//126
	0xff,	//127
	0xff,	//128
	0xff,	//129
	0xff,	//130
	0xff,	//131

	0xff,	//132
	0xff,	//133
	0xff,	//134
	0xff,	//135
	0xff,	//136
	0xff,	//137
	0xff,	//138
	0x73,	//139
	0xff,	//140

	0xff,	//141
	0xff,	//142
	0xff,	//143
	0xff,	//144
	0xff,	//145
	0xff,	//146
	0xff,	//147
	0xff,	//148
	0xff,	//149
	0xff,	//150

	0xff,	//151
	0xff,	//152
	0xff,	//153
	0xff,	//154
	0xff,	//155
	0xff,	//156
	0xff,	//157
	0xff,	//158
	0xff,	//159
	0xff,	//160

	0xff,	//161
	0xff,	//162
	0xff,	//163
	0xff,	//164
	0xff,	//165
	0xff,	//166
	0xff,	//167
	0xff,	//168
	0xff,	//169
	0xff,	//170

	0xff,	//171
	0xff,	//172
	0xff,	//173
	0xff,	//174
	0xff,	//175
	0xff,	//176
	0xff,	//177
	0xff,	//178
	0xff,	//179
	0xff,	//180

	0xff,	//181
	0xff,	//182
	0xff,	//183
	0xff,	//184
	0xff,	//185
	0xff,	//186
	0xff,	//187
	0xff,	//188
	0xff,	//189
	0xff,	//190
	0xff,	//191
	0xff,	//192
	0xff,	//193
	0xff,	//194
	0xff,	//195
	0xff,	//196
	0xff,	//197
	0xff,	//198
	0xff,	//199
	0xff,	//200
	0xff,	//201
	0xff,	//202
	0xff,	//203
	0xff,	//204
	0xff,	//205
	0xff,	//206
	0xff,	//207
	0xff,	//208
	0xff,	//209
	0xff,	//210
	0xff,	//211
	0xff,	//212
	0xff,	//213
	0xff,	//214
	0xff,	//215
	0xff,	//216
	0xff,	//217
	0xff,	//218
	0xff,	//219
	0xff,	//220
	0xff,	//221
	0xff,	//222
	0xff,	//223
	0xff,	//224
	0xff,	//225
	0xff,	//226
	0xff,	//227
	0xff,	//228
	0xff,	//229
	0xff,	//230
	0xff,	//231
	0xff,	//232
	0xff,	//233
	0xff,	//234
	0xff,	//235
	0xff,	//236
	0xff,	//237
	0xff,	//238
	0xff,	//239
	0xff,	//240
	0xff,	//241
	0xff,	//242
	0xff,	//243
	0xff,	//244
	0xff,	//245
	0xff,	//246
	0xff,	//247
	0xff,	//248
	0xff,	//249
	0xff,	//250
	0xff,	//251
	0xff,	//252
	0xff,	//253
	0xff,	//254
	0xff	//255

	
};




// APPLICATION PALETTE

static const unsigned char aubVideoPlayerPaletteTransparent[256] = {
///////////////////////////////////////////////////
// System Pallette
0x0,		// 0		transparent color
0xff,	// 1		white
0xff,	// 2		black
0x0,	// 3
0x0,	// 4
0x0,	// 5
0x0,	// 6
0x0,	// 7
0x0,	// 8
0x0,	// 9
0x0,	// 10
0x0,	// 11
0x0,	// 12
0x0,	// 13
0x0,	// 14
0x0,	// 15
0x0,	// 16


///////////////////////////////////////////////////
// Start of NML-TAVI Pallette
// 0x66 = 40%
// 0xB0 = 45%
// 0xFF = 100%
// 0xB2 = 70%
// * Selected State
/*
0xB0,		//17
0xB0,		//18
0xB0,		//19
0xB0,		//20
0xB0,		//21
0xB0,		//22
0xB0,		//23
0xB0,		//24
0xB0,		//25
0xB0,		//26
0xB0,		//27
0xb2,		//28
0xff,		//29
0xb2,		//30
0xB0,		//31
0xB0,		//32
0xB0,		//33
0xB0,		//34
*/

// * Selected State

//from Designer
0xff,		//17
0xff,		//18
0xff,		//19
0xff,		//20
0xB2,		//21
0xff,		//22
0xff,		//23
0x4C,		//24
0x26,		//25
0x0C,		//26
0x73,		//27
0x73,		//28
0xB3,		//29
0xff,		//30
0x99,		//31
0x73,		//32
0x73,		//33
0xff,		//34

/*
0xff,		//17
0xff,		//18
0xff,		//19
0xff,		//20
0xff,		//21
0xff,		//22
0xff,		//23
0xff,		//24
0xff,		//25
0xff,		//26

0xff,		//27
0xff,		//28
0xff,		//29
0xff,		//30
0xff,		//31
0xff,		//32
0xff,		//33
0xff,		//34
*/

0xff,		//35
0xff,		//36
0xff,		//37
0xff,		//38
0xff,		//39
0xff,		//40
0xff,		//41
0xff,		//42
0xff,		//43
0xff,		//44
0xff,		//45
0xff,		//46
0xff,		//47
0xff,		//48
0xff,		//49
0xff,		//50
0xB2,		//51
0xff,		//52
0xff,		//53
0xff,		//54
0xff,		//55
0xff,		//56
0xff,		//57
0xff,		//58
0xff,		//59
0xff,		//60
0xff,		//61
0xff,		//62

// Middle of Select and Degree-1 State 
0xcc,		//63
0xcc,		//64
0xcc,		//65
0xcc,		//66
0xcc,		//67
0xcc,		//68
0xcc,		//69
0xcc,		//70
0xcc,		//71
0xcc,		//72
0x66,		//73

// Degree-1 State 
0x99,		//74
0x99,		//75
0x99,		//76
0x99,		//77
0x99,		//78
0x99,		//79
0x99,		//80
0x99,		//81
0x99,		//82
0x99,		//83
0x4c,		//84

// Middle of Select and Degree-1 State 
0x66,		//85
0x66,		//86
0x66,		//87
0x66,		//88
0x66,		//89
0x66,		//90
0x66,		//91
0x66,		//92
0x66,		//93
0x66,		//94
0x33,		//95

// Degree-2 State 
0x33,		//96
0x33,		//97
0x33,		//98
0x33,		//99
0xff,		//100
0xff,		//101
0x33,		//102
0x33,		//103
0x33,		//104
0x33,		//105
0x19,		//106

///////////////////////////////////////////////////
// Reserved Area
0xff,	//107
0xb2,	//108
0xB0,	//109
0xff,	//110
0xB0,	//111
0xB0,	//112
0x66,	//113
0xff,	//114
0xB0,	//115
0xff,	//116
0xff,	//117
0xff,	//118
0xff,	//119




0xff,	//120

0xff,	//121
0xff,	//122
0xff,	//123
0xff,	//124
0xff,	//125
0xff,	//126
0xff,	//127
0xff,	//128
0xff,	//129
0xff,	//130
0xff,	//131

0xff,	//132
0xff,	//133
0xff,	//134
0xff,	//135
0xff,	//136
0xff,	//137
0xff,	//138
0x73,	//139
0xff,	//140

0xff,	//141
0xff,	//142
0xff,	//143
0xff,	//144
0xff,	//145
0xff,	//146
0xff,	//147
0xff,	//148
0xff,	//149
0xff,	//150

0xff,	//151
0xff,	//152
0xff,	//153
0xff,	//154
0xff,	//155
0xff,	//156
0xff,	//157
0xff,	//158
0xff,	//159
0xff,	//160

0xff,	//161
0xff,	//162
0xff,	//163
0xff,	//164
0xff,	//165
0xff,	//166
0xff,	//167
0xff,	//168
0xff,	//169
0xff,	//170

0xff,	//171
0xff,	//172
0xff,	//173
0xff,	//174
0xff,	//175
0xff,	//176
0xff,	//177
0xff,	//178
0xff,	//179
0xff,	//180

0xff,	//181
0xff,	//182
0xff,	//183
0xff,	//184
0xff,	//185
0xff,	//186
0xff,	//187
0xff,	//188
0xff,	//189
0xff,	//190
0xff,	//191
0xff,	//192
0xff,	//193
0xff,	//194
0xff,	//195
0xff,	//196
0xff,	//197
0xff,	//198
0xff,	//199
0xff,	//200
0xff,	//201
0xff,	//202
0xff,	//203
0xff,	//204
0xff,	//205
0xff,	//206
0xff,	//207
0xff,	//208
0xff,	//209
0xff,	//210
0xff,	//211
0xff,	//212
0xff,	//213
0xff,	//214
0xff,	//215
0xff,	//216
0xff,	//217
0xff,	//218
0xff,	//219
0xff,	//220
0xff,	//221
0xff,	//222
0xff,	//223
0xff,	//224
0xff,	//225
0xff,	//226
0xff,	//227
0xff,	//228
0xff,	//229
0xff,	//230
0xff,	//231
0xff,	//232
0xff,	//233
0xff,	//234
0xff,	//235
0xff,	//236
0xff,	//237
0xff,	//238
0xff,	//239
0xff,	//240
0xff,	//241
0xff,	//242
0xff,	//243
0xff,	//244
0xff,	//245
0xff,	//246
0xff,	//247
0xff,	//248
0xff,	//249
0xff,	//250
0xff,	//251
0xff,	//252
0xff,	//253
0xff,	//254
0xff	//255

	
};

static const unsigned char aubRecoderPaletteTransparent[256] = {
///////////////////////////////////////////////////
// System Pallette
0x0,		// 0		transparent color
0xff,	// 1		white
0xff,	// 2		black
0x0,	// 3
0x0,	// 4
0x0,	// 5
0x0,	// 6
0x0,	// 7
0x0,	// 8
0x0,	// 9
0x0,	// 10
0x0,	// 11
0x0,	// 12
0x0,	// 13
0x0,	// 14
0x0,	// 15
0x0,	// 16


///////////////////////////////////////////////////
// Start of NML-TAVI Pallette
// 0x66 = 40%
// 0xB0 = 45%
// 0xFF = 100%
// 0xB2 = 70%
// * Selected State
/*
0xB0,		//17
0xB0,		//18
0xB0,		//19
0xB0,		//20
0xB0,		//21
0xB0,		//22
0xB0,		//23
0xB0,		//24
0xB0,		//25
0xB0,		//26
0xB0,		//27
0xb2,		//28
0xff,		//29
0xb2,		//30
0xB0,		//31
0xB0,		//32
0xB0,		//33
0xB0,		//34
0xB0,		//35
0xB0,		//36
0xB0,		//37
0xB0,		//38
0xB0,		//39
0xB0,		//40
0xb2,		//41
0xB0,		//42
0xB0,		//43
0xB0,		//44
0xff,		//45
0xb2,		//46
0xB0,		//47
0xb2,		//48
0xff,		//49
0xff,		//50
0xff,		//51
0xff,		//52
0xff,		//53
0xff,		//54

*/

// * Selected State
0xff,		//17
0xff,		//18
0xff,		//19
0xff,		//20
0xff,		//21
0xff,		//22
0xff,		//23
0x1E,		//24
0x0F,		//25
0x05,		//26

0x2D,		//27
0x2D,		//28
0x2D,		//29
0xff,		//30
0x3C,		//31
0x2D,		//32
0x2D,		//33
0xff,		//34
0xff,		//35
0xff,		//36
0xff,		//37
0xff,		//38
0xff,		//39
0xff,		//40
0xff,		//41
0xff,		//42
0xff,		//43
0xff,		//44
0xff,		//45
0xff,		//46
0xff,		//47
0xff,		//48
0xff,		//49
0xff,		//50
0xff,		//51
0xff,		//52
0x2D,		//53
0xff,		//54



0xff,		//55
0xff,		//56
0xff,		//57
0xff,		//58
0xff,		//59
0xff,		//60
0xff,		//61
0xff,		//62

// Middle of Select and Degree-1 State 
0xcc,		//63
0xcc,		//64
0xcc,		//65
0xcc,		//66
0xcc,		//67
0xcc,		//68
0xcc,		//69
0xcc,		//70
0xcc,		//71
0xcc,		//72
0x66,		//73

// Degree-1 State 
0x99,		//74
0x99,		//75
0x99,		//76
0x99,		//77
0x99,		//78
0x99,		//79
0x99,		//80
0x99,		//81
0x99,		//82
0x99,		//83
0x4c,		//84

// Middle of Select and Degree-1 State 
0x66,		//85
0x66,		//86
0x66,		//87
0x66,		//88
0x66,		//89
0x66,		//90
0x66,		//91
0x66,		//92
0x66,		//93
0x66,		//94
0x33,		//95

// Degree-2 State 
0x33,		//96
0x33,		//97
0x33,		//98
0x33,		//99
0x33,		//100
0x33,		//101
0x33,		//102
0x33,		//103
0x33,		//104
0x33,		//105
0x19,		//106

///////////////////////////////////////////////////
// Reserved Area
0xff,	//107
0xb2,	//108
0xB0,	//109
0xff,	//110
0xB0,	//111
0xB0,	//112
0x66,	//113
0xff,	//114
0xB0,	//115
0xff,	//116
0xff,	//117
0xff,	//118
0xff,	//119




0xff,	//120

0xff,	//121
0xff,	//122
0xff,	//123
0xff,	//124
0xff,	//125
0xff,	//126
0xff,	//127
0xff,	//128
0xff,	//129
0xff,	//130
0xff,	//131

0xff,	//132
0xff,	//133
0xff,	//134
0xff,	//135
0xff,	//136
0xff,	//137
0xff,	//138
0x73,	//139
0xff,	//140

0xff,	//141
0xff,	//142
0xff,	//143
0xff,	//144
0xff,	//145
0xff,	//146
0xff,	//147
0xff,	//148
0xff,	//149
0xff,	//150

0xff,	//151
0xff,	//152
0xff,	//153
0xff,	//154
0xff,	//155
0xff,	//156
0xff,	//157
0xff,	//158
0xff,	//159
0xff,	//160

0xff,	//161
0xff,	//162
0xff,	//163
0xff,	//164
0xff,	//165
0xff,	//166
0xff,	//167
0xff,	//168
0xff,	//169
0xff,	//170

0xff,	//171
0xff,	//172
0xff,	//173
0xff,	//174
0xff,	//175
0xff,	//176
0xff,	//177
0xff,	//178
0xff,	//179
0xff,	//180

0xff,	//181
0xff,	//182
0xff,	//183
0xff,	//184
0xff,	//185
0xff,	//186
0xff,	//187
0xff,	//188
0xff,	//189
0xff,	//190
0xff,	//191
0xff,	//192
0xff,	//193
0xff,	//194
0xff,	//195
0xff,	//196
0xff,	//197
0xff,	//198
0xff,	//199
0xff,	//200
0xff,	//201
0xff,	//202
0xff,	//203
0xff,	//204
0xff,	//205
0xff,	//206
0xff,	//207
0xff,	//208
0xff,	//209
0xff,	//210
0xff,	//211
0xff,	//212
0xff,	//213
0xff,	//214
0xff,	//215
0xff,	//216
0xff,	//217
0xff,	//218
0xff,	//219
0xff,	//220
0xff,	//221
0xff,	//222
0xff,	//223
0xff,	//224
0xff,	//225
0xff,	//226
0xff,	//227
0xff,	//228
0xff,	//229
0xff,	//230
0xff,	//231
0xff,	//232
0xff,	//233
0xff,	//234
0xff,	//235
0xff,	//236
0xff,	//237
0xff,	//238
0xff,	//239
0xff,	//240
0xff,	//241
0xff,	//242
0xff,	//243
0xff,	//244
0xff,	//245
0xff,	//246
0xff,	//247
0xff,	//248
0xff,	//249
0xff,	//250
0xff,	//251
0xff,	//252
0xff,	//253
0xff,	//254
0xff	//255

	
};

static const unsigned char aubRadioPaletteTransparent[256] = {
///////////////////////////////////////////////////
// System Pallette
0x0,		// 0		transparent color
0xff,	// 1		white
0xff,	// 2		black
0x0,	// 3
0x0,	// 4
0x0,	// 5
0x0,	// 6
0x0,	// 7
0x0,	// 8
0x0,	// 9
0x0,	// 10
0x0,	// 11
0x0,	// 12
0x0,	// 13
0x0,	// 14
0x0,	// 15
0x0,	// 16


///////////////////////////////////////////////////
// Start of NML-TAVI Pallette
// 0x66 = 40%
// 0xB0 = 45%
// 0xFF = 100%
// 0xB2 = 70%
// * Selected State
/*
0xB0,		//17
0xB2,		//18
0xB0,		//19
0xB0,		//20
0xB0,		//21
0xB0,		//22
0xB0,		//23
0xB0,		//24
0xB0,		//25
0xB0,		//26
0xB0,		//27
0xb2,		//28
0xff,		//29
0xb2,		//30
0xB0,		//31
0xB0,		//32
0xB0,		//33
0xB0,		//34
0xB0,		//35
0xB0,		//36
0xB0,		//37
0xB0,		//38
0xB0,		//39
0xB0,		//40
0xb2,		//41
0xB0,		//42
0xB0,		//43
0xB0,		//44
0xff,		//45
0xb2,		//46
0xB0,		//47
0xb2,		//48
0xff,		//49
0xff,		//50
0xff,		//51
0xff,		//52
0xff,		//53
0xff,		//54
*/

// * Selected State

0xff,		//17
0xff,		//18
0xff,		//19
0xff,		//20
0xB2,		//21
0xff,		//22
0xff,		//23
0x4C,		//24
0x26,		//25
0x0C,		//26
0x73,		//27
0x73,		//28
0xD8,		//29
0xff,		//30
0x99,		//31
0x73,		//32
0x73,		//33
0xff,		//34
0xff,		//35
0xff,		//36
0xff,		//37
0xff,		//38
0xff,		//39
0xff,		//40
0xff,		//41
0xff,		//42
0xff,		//43
0xff,		//44
0xff,		//45
0xff,		//46
0xff,		//47
0xff,		//48
0xff,		//49
0xff,		//50
0xB2,		//51
0xff,		//52
0xff,		//53
0xff,		//54

0xff,		//55
0xff,		//56
0xff,		//57
0xff,		//58
0xff,		//59
0xff,		//60
0xff,		//61
0xB0,		//62

0xB0,		//63
0xB0,		//64
0xB0,		//65
0xB0,		//66
0xB0,		//67
0xB0,		//68
0xB0,		//69
0xB0,		//70
0xB0,		//71
0xB0,		//72
0xB0,		//73

0xB0,		//74
0xB0,		//75
0xB0,		//76
0xB0,		//77
0xB0,		//78
0xB0,		//79
0xB0,		//80
0xB0,		//81
0xB0,		//82



0x73,		//83
0x73,		//84
0x73,		//85
0x73,		//86
0x73,		//87
0x73,		//88
0x73,		//89
0x73,		//90
0x73,		//91
0x73,		//92
0x73,		//93
0x73,		//94
0x73,		//95

0x73,		//96
0x73,		//97
0x73,		//98
0x33,		//99
0xff,		//0x33,		//100
0xff,		//0x33,		//101
0x33,		//102
0x33,		//103
0x33,		//104
0x33,		//105
0x19,		//106

///////////////////////////////////////////////////
// Reserved Area
0xff,	//107
0xb2,	//108
0xB0,	//109
0xff,	//110
0xB0,	//111
0xB0,	//112
0x66,	//113
0xff,	//114
0xB0,	//115
0xff,	//116
0xff,	//117
0xff,	//118
0xff,	//119
0xff,	//120

0xff,	//121
0xff,	//122
0xff,	//123
0xff,	//124
0xff,	//125
0xff,	//126
0xff,	//127
0xff,	//128
0xff,	//129
0xff,	//130
0xff,	//131

0xff,	//132
0xff,	//133
0xff,	//134
0xff,	//135
0xff,	//136
0xff,	//137
0xff,	//138
0x73,	//139
0xff,	//140

0xff,	//141
0xff,	//142
0xff,	//143
0xff,	//144
0xff,	//145
0xff,	//146
0xff,	//147
0xff,	//148
0xff,	//149
0xff,	//150

0xff,	//151
0xff,	//152
0xff,	//153
0xff,	//154
0xff,	//155
0xff,	//156
0xff,	//157
0xff,	//158
0xff,	//159
0xff,	//160

0xff,	//161
0xff,	//162
0xff,	//163
0xff,	//164
0xff,	//165
0xff,	//166
0xff,	//167
0xff,	//168
0xff,	//169
0xff,	//170

0xff,	//171
0xff,	//172
0xff,	//173
0xff,	//174
0xff,	//175
0xff,	//176
0xff,	//177
0xff,	//178
0xff,	//179
0xff,	//180

0xff,	//181
0xff,	//182
0xff,	//183
0xff,	//184
0xff,	//185
0xff,	//186
0xff,	//187
0xff,	//188
0xff,	//189
0xff,	//190
0xff,	//191
0xff,	//192
0xff,	//193
0xff,	//194
0xff,	//195
0xff,	//196
0xff,	//197
0xff,	//198
0xff,	//199
0xff,	//200
0xff,	//201
0xff,	//202
0xff,	//203
0xff,	//204
0xff,	//205
0xff,	//206
0xff,	//207
0xff,	//208
0xff,	//209
0xff,	//210
0xff,	//211
0xff,	//212
0xff,	//213
0xff,	//214
0xff,	//215
0xff,	//216
0xff,	//217
0xff,	//218
0xff,	//219
0xff,	//220
0xff,	//221
0xff,	//222
0xff,	//223
0xff,	//224
0xff,	//225
0xff,	//226
0xff,	//227
0xff,	//228
0xff,	//229
0xff,	//230
0xff,	//231
0xff,	//232
0xff,	//233
0xff,	//234
0xff,	//235
0xff,	//236
0xff,	//237
0xff,	//238
0xff,	//239
0xff,	//240
0xff,	//241
0xff,	//242
0xff,	//243
0xff,	//244
0xff,	//245
0xff,	//246
0xff,	//247
0xff,	//248
0xff,	//249
0xff,	//250
0xff,	//251
0xff,	//252
0xff,	//253
0xff,	//254
0xff	//255
};
#endif

#endif



