/* Font data for Cooper Black 14pt */
#include <stdint.h>
#define uint_8 uint8_t

#ifndef __HQFONT_H__
#define __HQFONT_H__
typedef struct 
{
	uint8_t  Width;
	uint8_t  Height;
	uint16_t Offset;
}FONT_CHAR_INFO;

typedef struct 
{
	uint8_t               CHeight;          /*  Character height */
	uint8_t               CStart;           /*  Start character */
	uint8_t               CEnd;             /*  End character */
	const FONT_CHAR_INFO* Descriptors;      /*  Character descriptor array */
	const uint8_t*        FontBitmaps;      /*  Character bitmap array */
}FONT_INFO;

extern const FONT_INFO* FontList[];
//extern const FONT_INFO FontInfo;

//extern const uint8_t FontBitmaps[];
//extern const FONT_CHAR_INFO FontDescriptors[];

#endif
