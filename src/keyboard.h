#ifndef _keyb_h_
#define _keyb_h_

#include "hardware.h"
#include "dcpu.h"

#define KEYB_VERSION          0x1
#define KEYB_ID               0x30cf7406
#define KEYB_MANUFACTURER     0x0

#define ASCII_MIN             0x20
#define ASCII_MAX             0x7E

#define UP_KEY_CODE           65362
#define DOWN_KEY_CODE         65364
#define LEFT_KEY_CODE         65361
#define RIGHT_KEY_CODE        65363

#define CTRL_KEY_CODE         65507

#define LEFT_ALT_KEY_CODE     65513
#define RIGHT_ALT_KEY_CODE    65514

#define LEFT_SHIFT_KEY_CODE   65505
#define RIGHT_SHIFT_KEY_CODE  65506

#define BACK_SPACE_KEY_CODE   65288
#define ENTER_KEY_CODE        65293
#define DELETE_KEY_CODE       65535
#define INSERT_KEY_CODE       65379

/* */

#define BACKSPACE      0x10
#define RETURN         0x11
#define INSERT         0x12
#define DELETE         0x13
#define ARROW_UP       0x80
#define ARROW_DOWN     0x81
#define ARROW_LEFT     0x82
#define ARROW_RIGHT    0x83
#define SHIFT          0x90
#define CONTROL        0x91

#define KEYB_BUFFER_SIZE 0xFFFF

void init_keyboard(void);
void keyboard_handle_int(udcpu_t*);
void keyboard_reset(void);

udcpu_t dequeue_code(void);
void keyboard_key_pressed(uldcpu_t);
void keyboard_key_released(uldcpu_t);
void keyboard_trigger_int(void);
udcpu_t get_key_code(udcpu_t);

#include "keyboard.c"
#endif
