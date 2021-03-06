/******************************************************************************
 * Name         : rectable.h
 * Title        : this is the reciprocal table to be loaded into texas.
 * Author       : Peter Leaback
 * Created      : 20/10/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  :
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: rectable.h,v $
 * Revision 1.2  1997/05/02  07:26:29  erf
 * Removed UINT32's etc and replaced with sgl_uint32 etc.
 *
 * Revision 1.1  1996/06/10  11:51:12  jop
 * Initial revision
 *
 * Revision 1.1  1996/06/10  11:39:13  jop
 * Initial revision
 *
 * Revision 1.1  1995/10/20  15:20:04  pdl
 * Initial revision
 *

 *****************************************************************************/

static sgl_uint32 ReciprocalTable[]={
 		0x7FFFFFFF,
 		0x00000000,
  		0x7FFFFFFF,
  		0x00000FE0,
  		0x3FFFFFF7,
  		0x00000FC0,
  		0x7FFF7FFF,
  		0x00000FA1,
  		0xDFFFBFFE,
  		0x00000F81,
  		0x7FF7FF7F,
  		0x00000F63,
  		0x6FFBFF7F,
  		0x00000F44,
  		0xBFEFEFF7,
  		0x00000F25,
  		0xFBFBFBF7,
  		0x00000F07,
  		0xF7EFDFBF,
  		0x00000EE9,
  		0x7EFBF7DF,
  		0x00000ECC,
  		0xFEFBEF7D,
  		0x00000EAE,
  		0x77DEFBDE,
  		0x00000E91,
  		0x7BDEF7BD,
  		0x00000E74,
  		0x6F7BDDEF,
  		0x00000E57,
        0xF7BDDEEE,
        0x00000E3A,
        0x7777BBBB,
        0x00000E1E,
        0xDDDDDDDD,
        0x00000E01,
        0xDDDBBBBB,
        0x00000DE5,
        0x7776EEDD,
        0x00000DCA,
        0x3B776EDB,
        0x00000DAE,
        0xB76DDB76,
        0x00000D92,
        0xDDB6EDB6,
        0x00000D77,
        0xEDB6DB6D,
        0x00000D5C,
        0xDB6DB6DA,
        0x00000D41,
        0x5B6DB6D6,
        0x00000D27,
        0xDB6B6DAD,
        0x00000D0C,
        0x35B6B6B6,
        0x00000CF2,
        0x36B6B5B5,
        0x00000CD8,
        0x2DAD6B5A,
        0x00000CBE,
        0xDAD6AD6B,
        0x00000CA4,
        0x5AD5AD5A,
        0x00000C8B,
        0xD5AB56AD,
        0x00000C71,
        0xDAB56AB5,
        0x00000C58,
        0xDAB556AB,
        0x00000C3F,
        0xD56AAD55,
        0x00000C26,
        0x6AAAD555,
        0x00000C0E,
        0xEAAAAAD5,
        0x00000BF5,
        0x55555556,
        0x00000BDD,
        0xAAAAAAAA,
        0x00000BC4,
        0xAA955555,
        0x00000BAC,
        0x55552AAA,
        0x00000B95,
        0x2A55552A,
        0x00000B7D,
        0xAA5552AA,
        0x00000B65,
        0xD54AA954,
        0x00000B4E,
        0x2A552A95,
        0x00000B37,
        0x4A952A54,
        0x00000B20,
        0x2952A54A,
        0x00000B09,
        0xD4A54A52,
        0x00000AF2,
        0xA5294A52,
        0x00000ADB,
        0x14A52949,
        0x00000AC5,
        0x4A525292,
        0x00000AAF,
        0x94949494,
        0x00000A98,
        0x94949292,
        0x00000A82,
        0x524A4929,
        0x00000A6D,
        0x24A4924A,
        0x00000A57,
        0xC9249249,
        0x00000A41,
        0x49249249,
        0x00000A2C,
        0xA2492492,
        0x00000A16,
        0xC8924922,
        0x00000A01,
        0xC9124912,
        0x000009EC,
        0xC4912449,
        0x000009D7,
        0x92448912,
        0x000009C2,
        0x24891122,
        0x000009AE,
        0xC4889112,
        0x00000999,
        0x22444888,
        0x00000985,
        0x91111112,
        0x00000970,
        0xA2222222,
        0x0000095C,
        0xA2222211,
        0x00000948,
        0x91110888,
        0x00000934,
        0x84442221,
        0x00000920,
        0x10884422,
        0x0000090D,
        0x91084421,
        0x000008F9,
        0x08842108,
        0x000008E6,
        0x44210841,
        0x000008D3,
        0x88421082,
        0x000008BF,
        0x90821082,
        0x000008AC,
        0x90420821,
        0x00000899,
        0x84104108,
        0x00000886,
        0x20820810,
        0x00000874,
        0xC1040820,
        0x00000861,
        0x41020810,
        0x0000084F,
        0xA0410202,
        0x0000083C,
        0x04081020,
        0x0000082A,
        0x20404040,
        0x00000818,
        0x40404040,
        0x00000806,
        0x40402020,
        0x000007F4,
        0x10080402,
        0x000007E2,
        0x01004010,
        0x000007D0,
        0x88010040,
        0x000007BE,
        0x10020040,
        0x000007AD,
        0x88008008,
        0x0000079B,
        0x00800800,
        0x0000078A,
        0x40010004,
        0x00000779,
        0x80100020,
        0x00000767,
        0x80200020,
        0x00000756,
        0x80080002,
        0x00000745,
        0x80002000,
        0x00000734,
        0x01000001,
        0x00000724,
        0x00000040,
        0x00000713,
        0x80000080,
        0x00000702,
        0x00000002,
        0x000006F2,
        0x80000000,
        0x000006E1,
        0x80000800,
        0x000006D1,
        0x7FFFFFFF,
        0x000006C1,
        0x7FFFFFFF,
        0x000006B1,
        0x7FFFFFFF,
        0x000006A1,
        0x7FFFFFFF,
        0x00000691,
        0x7FFF7FFF,
        0x00000681,
        0xFFFFFFFF,
        0x00000671,
        0xFEFFFFFF,
        0x00000661,
        0x7FFDFFFF,
        0x00000652,
        0xFFFDFFFF,
        0x00000642,
        0x7FBFFFFF,
        0x00000633,
        0xDFFFFF7F,
        0x00000623,
        0xFFFBFFFF,
        0x00000614,
        0xFFFFEFFF,
        0x00000605,
        0x77FFFBFF,
        0x000005F6,
        0x7BFFF7FF,
        0x000005E7,
        0x6FFFDFFF,
        0x000005D8,
        0xFFFBFFDF,
        0x000005C9,
        0xFEFFF7FF,
        0x000005BA,
        0x7FF7FF7F,
        0x000005AC,
        0x6FFDFFDF,
        0x0000059D,
        0xF7FEFFDF,
        0x0000058E,
        0x77FDFF7F,
        0x00000580,
        0xDFF7FBFE,
        0x00000571,
        0xFF7FBFDF,
        0x00000563,
        0x6FF7FBFD,
        0x00000555,
        0x7EFEFF7F,
        0x00000547,
        0x7F7F7F7F,
        0x00000539,
        0xBFBF7F7F,
        0x0000052A,
        0x7F7EFEFD,
        0x0000051D,
        0x7DFBFBF7,
        0x0000050F,
        0x6FDFDFBF,
        0x00000501,
        0xFEFDF7EF,
        0x000004F3,
        0xDFBF7DFB,
        0x000004E5,
        0x77DFBEFD,
        0x000004D8,
        0xF7DFBEFB,
        0x000004CA,
        0x77DF7DF7,
        0x000004BD,
        0xDFBEFBEF,
        0x000004AF,
        0x3DF7DF7D,
        0x000004A2,
        0x77BEFBEF,
        0x00000495,
        0x7DF7BEF7,
        0x00000488,
        0xDEFBDF7B,
        0x0000047A,
        0xEF7BEF7B,
        0x0000046D,
        0xEF7BDF7B,
        0x00000460,
        0xDEF7BEF7,
        0x00000453,
        0xBDEF7BDE,
        0x00000446,
        0x77BDEF7B,
        0x0000043A,
        0x3DEF7BDE,
        0x0000042D,
        0xEF7BDDEF,
        0x00000420,
        0x77BDDEF7,
        0x00000414,
        0xFBDDEEF7,
        0x00000407,
        0x7BDDEEEF,
        0x000003FB,
        0xF7BBDDEE,
        0x000003EE,
        0x6F777BBB,
        0x000003E2,
        0xDDDEEEF7,
        0x000003D5,
        0xF777BBBB,
        0x000003C9,
        0x5DDDDDDE,
        0x000003BD,
        0x6EEEEEEE,
        0x000003B1,
        0x6EF77777,
        0x000003A5,
        0x7777776E,
        0x00000399,
        0x6EEEEEEE,
        0x0000038D,
        0x6EDDDDDD,
        0x00000381,
        0x5BBBBB77,
        0x00000375,
        0xF76EEEDD,
        0x00000369,
        0xDDBBBB77,
        0x0000035D,
        0x6EEDDDBB,
        0x00000352,
        0x3776EDDD,
        0x00000346,
        0xBB776EDD,
        0x0000033A,
        0x3BB76EDD,
        0x0000032F,
        0xBB76EDDB,
        0x00000323,
        0x376EDDB7,
        0x00000318,
        0x6EDDB76E,
        0x0000030D,
        0xDBB76DDB,
        0x00000301,
        0xF6DDB76D,
        0x000002F6,
        0x5B76DDB7,
        0x000002EB,
        0x6DBB6DDB,
        0x000002E0,
        0x6EDB76DB,
        0x000002D5,
        0xB6DBB6DB,
        0x000002C9,
        0xB6DBB6DB,
        0x000002BE,
        0x76DB6DDB,
        0x000002B4,
        0x6DB6EDB6,
        0x000002A9,
        0x5B6DBB6D,
        0x0000029E,
        0x36DB6DB6,
        0x00000293,
        0xDB6EDB6D,
        0x00000288,
        0xB6DB6DB6,
        0x0000027D,
        0x5B6D6DB6,
        0x00000273,
        0xDB6DB6DB,
        0x00000268,
        0x6D6DB6DB,
        0x0000025E,
        0xEDADB6DB,
        0x00000253,
        0x6B6DB6D6,
        0x00000249,
        0xDB6D6DB6,
        0x0000023E,
        0x36DB5B6D,
        0x00000234,
        0x6DB6B6D6,
        0x0000022A,
        0xDB5B6D6D,
        0x0000021F,
        0x2DB5B6B6,
        0x00000215,
        0x56DADB5B,
        0x0000020B,
        0x6B6B6D6D,
        0x00000201,
        0x6DADADAD,
        0x000001F7,
        0xADADB5B5,
        0x000001EC,
        0xB5ADADAD,
        0x000001E2,
        0xADADAD6D,
        0x000001D8,
        0x6D6B6B6B,
        0x000001CF,
        0x5B5ADAD6,
        0x000001C5,
        0x56B6B5AD,
        0x000001BB,
        0x2D6B6B5A,
        0x000001B1,
        0xD6D6B5AD,
        0x000001A7,
        0x6D6B5AD6,
        0x0000019E,
        0x35AD6D6B,
        0x00000194,
        0xDAD6B5AD,
        0x0000018A,
        0x6AD6B5AD,
        0x00000181,
        0xEB5AD6AD,
        0x00000177,
        0x6B5AD5AD,
        0x0000016E,
        0xEB56B5AB,
        0x00000164,
        0x5AD5AD6A,
        0x0000015B,
        0xD6AD6B56,
        0x00000151,
        0x356AD6AD,
        0x00000148,
        0x6AD6AD5A,
        0x0000013F,
        0xD5AB56B5,
        0x00000135,
        0xEAD5AB5A,
        0x0000012C,
        0x356AD5AB,
        0x00000123,
        0x56AD5AB5,
        0x0000011A,
        0x6AB56AD5,
        0x00000111,
        0xAAD5AB55,
        0x00000107,
        0xAB56AB56,
        0x000000FE,
        0xAB55AB55,
        0x000000F5,
        0xAAD5AAD5,
        0x000000EC,
        0x6AB55AAD,
        0x000000E4,
        0x56AB55AA,
        0x000000DB,
        0x555AAD56,
        0x000000D2,
        0x2AD56AAD,
        0x000000C9,
        0xD5AAB55A,
        0x000000C0,
        0xAB556AAB,
        0x000000B7,
        0x556AAD55,
        0x000000AF,
        0xEAAD556A,
        0x000000A6,
        0xAB555AAA,
        0x0000009D,
        0x5556AAAD,
        0x00000095,
        0xD56AAAD5,
        0x0000008C,
        0x55AAAAD5,
        0x00000084,
        0xD56AAAD5,
        0x0000007B,
        0x555AAAAD,
        0x00000073,
        0xD5556AAA,
        0x0000006A,
        0x2D55556A,
        0x00000062,
        0xAAAAD555,
        0x00000059,
        0xD56AAAAA,
        0x00000051,
        0x2D555555,
        0x00000049,
        0x6AAAAAAA,
        0x00000041,
        0xAD555555,
        0x00000038,
        0xD55AAAAA,
        0x00000030,
        0x2AAAAAAA,
        0x00000028,
        0x55555555,
        0x00000020,
        0x55555555,
        0x00000018,
        0x55555555,
        0x00000010,
        0x55555555,
        0x00000008};

