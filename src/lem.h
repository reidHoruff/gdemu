#ifndef _lem_h_
#define _lem_h_

#include "hardware.h"
#include "dcpu.h"

#define LEM_VERSION          0x1802
#define LEM_ID               0x7349f615
#define LEM_MANUFACTURER     0x1c6c8b36

#define LEM_VERSION_OLD      0x1801
#define LEM_ID_OLD           0x7349f615

#define LEM_GRID_WIDTH       32
#define LEM_GRID_HEIGHT      12
#define LEM_CHAR_WIDTH       4
#define LEM_CHAR_HEIGHT      8

#define INT_MEM_MAP_SCREEN   0
#define INT_MEM_MAP_FONT     1
#define INT_MEM_MAP_PALETTE  2
#define INT_SET_BORDER_COLOR 3
#define INT_DUMP_FONT        4
#define INT_DUMP_PALETTE     5

#define LEM_CHARACTER(x) (x&0b1111111)
#define LEM_BLINK(x) ((x>>7)&1)
#define LEM_FG_COLOR(x) ((x>>12)&0xF)
#define LEM_BG_COLOR(x) ((x>>8)&0xF)

#define RED(x) ((x>>8)&0xF)
#define GREEN(x) ((x>>4) & 0xF)
#define BLUE(x) (x & 0xF)

extern hardware_device *lem, *lem_old;
void init_lem(void);
void lem_handle_int(udcpu_t*);
void lem_reset(void);

extern udcpu_t lem_video_starting_addr;
extern udcpu_t lem_font_starting_addr;
extern udcpu_t lem_palette_starting_addr;
extern udcpu_t lem_border_color;

#endif
