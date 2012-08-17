#include "keyboard.h"
#include "dcpu.h"

void init_keyboard()
{	
	keyboard = (hardware_device*)malloc( sizeof(hardware_device) );
	keyboard->id = KEYB_ID;
	keyboard->version = KEYB_VERSION;
	keyboard->manufacturer = KEYB_MANUFACTURER;
	keyboard->call_int = &keyboard_handle_int;
	keyboard->reset = &keyboard_reset;
	add_hardware_device( keyboard );
	
	keyb_buffer = (udcpu_t*)malloc( sizeof(udcpu_t) * KEYB_BUFFER_SIZE );
	keyb_buffer_ptr = 0;
	
	keyboard_key_is_pressed = (udcpu_t*)malloc( sizeof(udcpu_t) * 0xFF );
	
	keyboard_interrupt_message = 0;
}

void keyboard_handle_int(udcpu_t *a)
{	
	switch( register_literals[REGISTER_A] )
	{
		case 0: /* clear keyboard buffer */
			keyb_buffer_ptr = 0; 
			break;
			
		case 1: /* store next key typed in register C */
			register_literals[REGISTER_C] = dequeue_code();
			break;
			
		case 2: /* set C reg to 1 if key specified by B reg is pressed, 0 else */
			register_literals[REGISTER_C] = keyboard_key_is_pressed[ register_literals[REGISTER_B] ];
			break;
			
		case 3: /* set interrupt message to reg B */
			keyboard_interrupt_message = register_literals[REGISTER_B];
			break;
			
		default:
			printf("Undefined Keyboard Interrupt message\n");
			break;
	}
}

void keyboard_key_pressed(uldcpu_t code)
{
	udcpu_t c = get_key_code(code);
	
	if(c && !keyboard_key_is_pressed[c]){
		printf("press: %x\n", c);
		if(keyb_buffer_ptr < KEYB_BUFFER_SIZE){
			keyb_buffer[keyb_buffer_ptr++] = code;
		}
		
		keyboard_key_is_pressed[c] = true;
		keyboard_trigger_int();
	}
}

void keyboard_key_released(uldcpu_t code)
{
	udcpu_t c = get_key_code(code);
	
	if(c){
		printf("release: %x\n", c);
		keyboard_key_is_pressed[c] = false;
		keyboard_trigger_int();
	}
	
}

void keyboard_trigger_int()
{
	if(keyboard_interrupt_message){
		queue_interrupt(keyboard_interrupt_message);
	}
}

udcpu_t get_key_code(udcpu_t code)
{
	switch(code)
	{
		case BACK_SPACE_KEY_CODE: return BACKSPACE;
		case ENTER_KEY_CODE:        return RETURN;
		case INSERT_KEY_CODE:       return INSERT;
		case DELETE_KEY_CODE:       return DELETE;
				
		case UP_KEY_CODE:           return ARROW_UP;
		case DOWN_KEY_CODE:         return ARROW_DOWN;
		case LEFT_KEY_CODE:         return ARROW_LEFT;
		case RIGHT_KEY_CODE:        return ARROW_RIGHT;
		
		case LEFT_SHIFT_KEY_CODE:
		case RIGHT_SHIFT_KEY_CODE:  return SHIFT;
		case CTRL_KEY_CODE:         return CONTROL;
		
		default:
			if(code >= ASCII_MIN && code <= ASCII_MAX)
				return code;
				
			else
				return 0;
	}
}

udcpu_t dequeue_code()
{
	if(keyb_buffer_ptr <= 0){
		return 0;
	}
	
	udcpu_t key = keyb_buffer[0];
	
	udcpu_t i;
	
	for(i=0; i < keyb_buffer_ptr-1; i++){
		keyb_buffer[i] = keyb_buffer[i+1];
	}
	
	keyb_buffer_ptr--;
	
	return key;
}

void keyboard_reset()
{
	keyb_buffer_ptr = 0;
	
	udcpu_t i;
	
	for(i = 0; i < 0xFF; i++){
		keyboard_key_is_pressed[i] = 0;
	}
}
