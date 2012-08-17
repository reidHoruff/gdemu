#include <gtk/gtk.h>
#include "gui.h"
#include "dcpu.h"
#include "keyboard.h"
#include "clock.h"

udcpu_t screen_pix_w, screen_pix_h, screen_offset_x, screen_offset_y;
gboolean emulator_running, start_pressed;

GtkBuilder *builder;
GtkEntry **value_sbuttons;

GtkWidget *mem_view, *live_disassemble_view;
GtkTreeModel *mem_model;
GtkListStore *mem_tree_store;

GtkWidget *screen_view;

enum{
	MEM_ADDR = 0,
	MEM_VALUE,
	MEM_CHAR,
	NUM_COLS
};

void draw_char(cairo_t *cr, udcpu_t xp, udcpu_t yp, udcpu_t w0, udcpu_t w1, udcpu_t fc, udcpu_t bc)
{
	/* cell coords to pix coords */
	xp *= LEM_CHAR_WIDTH;
	yp *= LEM_CHAR_HEIGHT;
	
	//printf("font %x %x\n", w0, w1);
	
	udcpu_t x, y;
	
	for(x=0; x < 16; x++){
		if( (w0>>(15-x))&1 ){ //fore ground
			gui_put_pixel( cr, xp+(x/8), yp+(7-(x%8)), fc );
		}
		else{ //background
			gui_put_pixel( cr, xp+(x/8), yp+(7-(x%8)), bc );
		}
		
		if( (w1>>(15-x))&1 ){ //fore ground
			gui_put_pixel( cr, xp+(x/8)+2, yp+(7-(x%8)), fc );
		}
		else{ //background
			gui_put_pixel( cr, xp+(x/8)+2, yp+(7-(x%8)), bc );
		}
	}
}

void gui_put_pixel(cairo_t *cr, udcpu_t x_pos, udcpu_t y_pos, udcpu_t color )
{	
	//printf("%d %d\n", x_pos, y_pos);
	cairo_set_source_rgb(cr, RED(color)/15.0, GREEN(color)/15.0, BLUE(color)/15.0);
	cairo_rectangle(cr, x_pos*screen_pix_w + screen_offset_x, y_pos*screen_pix_h + screen_offset_y, screen_pix_w, screen_pix_h);
	cairo_fill(cr);
}

void on_window_destroy (GtkObject *object, gpointer user_data)
{
	gtk_main_quit();
}

/* redraw/render screen */
gboolean on_screen_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{   
	screen_pix_w = (screen_view->allocation.width)/SCREEN_WIDTH;
	screen_pix_h = (screen_view->allocation.height)/SCREEN_HEIGHT;
	screen_offset_x = (screen_view->allocation.width - screen_pix_w*SCREEN_WIDTH)/2;
	screen_offset_y = (screen_view->allocation.height - screen_pix_h*SCREEN_HEIGHT)/2;
	//printf("%d %d %d %d\n", screen_pix_w, screen_pix_h, screen_offset_x, screen_offset_y);
	
	if(lem_video_starting_addr)//lem enabled
	{		
		//printf("display at %x\n", lem_video_starting_addr);
		 
		cairo_t *cr = gdk_cairo_create(screen_view->window);
		
		/* point to the video mem block */
		udcpu_t *video = &dcpu_mem[lem_video_starting_addr];
		
		/* point to default font if no addr is assigned */
		udcpu_t *font = lem_font_starting_addr ? &dcpu_mem[lem_font_starting_addr] : lem_font;
		
		/* point to default palette if no addr is assigned */
		udcpu_t *palette = lem_palette_starting_addr ? &dcpu_mem[lem_palette_starting_addr] : lem_palette;
		
		udcpu_t i;
		for(i=0; i < LEM_GRID_WIDTH*LEM_GRID_HEIGHT; i++){
			//printf("%04x  %08x\n", i + lem_video_starting_addr, video[i] );
			draw_char(cr, i%LEM_GRID_WIDTH, i/LEM_GRID_WIDTH, 
				font[ LEM_CHARACTER( video[i] )*2 ],      //font word0
				font[ LEM_CHARACTER( video[i] )*2 + 1 ],  //font word1
				palette[ LEM_FG_COLOR( video[i] ) ],      //foreground color
				palette[ LEM_BG_COLOR( video[i] ) ]       //background color
			);
		}
		
		cairo_destroy(cr);
	}
	
	return FALSE;
}

gboolean draw_screen(gpointer *user_data)
{
	gtk_widget_queue_draw_area(screen_view, 0, 0, screen_view->allocation.width, screen_view->allocation.height);
	return emulator_running;
}

/* assumed to be called every 100th of a second */
gboolean run_emulator(gpointer *user_data)
{	
	clock_tick();
	
	udcpu_t i = 1000;
	while(i-->0){
		dcpu_cycle();
	}
	
	return emulator_running;
}

gboolean poll_events(gpointer *user_data)
{	
	if(start_pressed && !emulator_running){
		emulator_running = TRUE;
		start_pressed = FALSE;
		
		g_timeout_add(10, (GSourceFunc)run_emulator, NULL);
		g_timeout_add(1000/10, (GSourceFunc)draw_screen, NULL);
	}
	
	return TRUE;
}

gboolean on_start_button_clicked(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	start_pressed = TRUE;
	return FALSE;
}

gboolean on_reset_button_clicked(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	reset_dcpu();
	reset_hardware();
	update_all_value_regs();
	update_visible_mem_tree();
	gtk_widget_queue_draw_area(screen_view, 0, 0, screen_view->allocation.width, screen_view->allocation.height);
	return FALSE;
}

gboolean on_refresh_button_clicked(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	update_all_value_regs();
	update_visible_mem_tree();
	gtk_widget_queue_draw_area(screen_view, 0, 0, screen_view->allocation.width, screen_view->allocation.height);
	return FALSE;
}

gboolean on_stop_button_clicked (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	emulator_running = FALSE;
	update_all_value_regs();
	update_visible_mem_tree();
	gtk_widget_queue_draw_area(screen_view, 0, 0, screen_view->allocation.width, screen_view->allocation.height);
	return FALSE;
}

gboolean on_step_button_clicked (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	//clock_tick();
	
	udcpu_t steps = 1;
	
	while(steps-->0){
		dcpu_step();
	}
	
	update_all_value_regs();
	update_visible_mem_tree();
	gtk_widget_queue_draw_area(screen_view, 0, 0, screen_view->allocation.width, screen_view->allocation.height);
	return FALSE;
}

gboolean on_mem_view_scroll_scroll_event (GtkScrolledWindow *scrolledwindow, GtkScrollType arg1, gboolean arg2, gpointer user_data) 
{
	printf("scroll\n");
	return FALSE;
}

gboolean key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	//printf("key press %d\n", event->keyval);
	keyboard_key_pressed(event->keyval);
	return FALSE;
}

gboolean key_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	//printf("key press %d\n", event->keyval);
	keyboard_key_released(event->keyval);
	return FALSE;
}

void update_all_value_regs()
{
	udcpu_t x;
	char text[10];
	
	for(x = REGISTER_A; x <= VALUE_IA; x++){
		sprintf(text, "%04X", register_literals[x] );
		gtk_entry_set_text( value_sbuttons[x], text);
	}
}

void update_visible_mem_tree()
{	
	GdkRectangle mem_view_pos, cell;
	GtkTreeIter iter_tmp;
	
	/* get coords of visible window */
	gtk_tree_view_get_visible_rect( GTK_TREE_VIEW(mem_view), &mem_view_pos );
	
	/* get dimensions of individual cell */
	gtk_tree_model_iter_nth_child( mem_model, &iter_tmp, NULL, 0 );
	GtkTreePath *path = gtk_tree_model_get_path(mem_model, &iter_tmp );
	gtk_tree_view_get_background_area( GTK_TREE_VIEW(mem_view), path, NULL, &cell);
	
	/*calculate address' to update */
	ldcpu_t first_cell_addr = (mem_view_pos.y / cell.height);
	ldcpu_t last_cell_addr = (mem_view_pos.height + mem_view_pos.y) / cell.height; 
	
	for( ; first_cell_addr <= last_cell_addr; first_cell_addr++ ){
		update_mem_location(first_cell_addr, dcpu_mem[first_cell_addr] );
	}
}

void update_mem_location(udcpu_t addr, udcpu_t val)
{
	GtkTreeIter iter;
	gtk_tree_model_iter_nth_child( mem_model, &iter, NULL, addr );
	
	char b[15], c[2];
	sprintf(b, "0x%04X", val );
	
	if(val >= 0x21 && val <= 0x7E){
		sprintf(c, "%c", val );
		gtk_list_store_set(mem_tree_store, &iter, MEM_VALUE, b, MEM_CHAR, c, -1);
	}
	else{
		gtk_list_store_set(mem_tree_store, &iter, MEM_VALUE, b, -1);
	}
}

GtkTreeModel* create_and_fill_mem_model()
{
	mem_tree_store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	GtkTreeIter iter;
	
	char a[15], b[15];
	ldcpu_t i;
	
	for(i = MIN_DCPU_T; i <= MAX_DCPU_T; i++){
		sprintf(a, "0x%04X", i );
		sprintf(b, "0x0000");
		gtk_list_store_append(mem_tree_store, &iter);
		gtk_list_store_set(mem_tree_store, &iter, MEM_ADDR, a, MEM_VALUE, b, -1);
	}
	
	return GTK_TREE_MODEL(mem_tree_store);
}

void init_tree_view()
{
	mem_view = GTK_WIDGET( GTK_GET_OBJ("mem_tree_view") );
	live_disassemble_view = GTK_WIDGET( GTK_GET_OBJ("live_dissassemble_tree_view") );
	
	GtkCellRenderer *renderer;
	
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(mem_view), -1, "Address", renderer, "text", MEM_ADDR, NULL);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(mem_view), -1, "Value", renderer, "text", MEM_VALUE, NULL);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(mem_view), -1, "Character", renderer, "text", MEM_CHAR, NULL);
	
	mem_model = create_and_fill_mem_model();
	gtk_tree_view_set_model( GTK_TREE_VIEW(mem_view), mem_model );
}

void init_value_spin_buttons()
{
	/* init all spin buttons */
	value_sbuttons[REGISTER_A] = GTK_ENTRY( GTK_GET_OBJ("disp_a") );
	value_sbuttons[REGISTER_B] = GTK_ENTRY( GTK_GET_OBJ("disp_b") );
	value_sbuttons[REGISTER_C] = GTK_ENTRY( GTK_GET_OBJ("disp_c") );
	value_sbuttons[REGISTER_X] = GTK_ENTRY( GTK_GET_OBJ("disp_x") );
	value_sbuttons[REGISTER_Y] = GTK_ENTRY( GTK_GET_OBJ("disp_y") );
	value_sbuttons[REGISTER_Z] = GTK_ENTRY( GTK_GET_OBJ("disp_z") );
	value_sbuttons[REGISTER_I] = GTK_ENTRY( GTK_GET_OBJ("disp_i") );
	value_sbuttons[REGISTER_J] = GTK_ENTRY( GTK_GET_OBJ("disp_j") );
	value_sbuttons[VALUE_PC  ] = GTK_ENTRY( GTK_GET_OBJ("disp_pc") );
	value_sbuttons[VALUE_SP  ] = GTK_ENTRY( GTK_GET_OBJ("disp_sp") );
	value_sbuttons[VALUE_EX  ] = GTK_ENTRY( GTK_GET_OBJ("disp_ex") );
	value_sbuttons[VALUE_IA  ] = GTK_ENTRY( GTK_GET_OBJ("disp_ia") );
}

void init_gui(int argc, char **argv)
{
	GtkWidget *window;
	value_sbuttons = (GtkEntry**)malloc( sizeof(GtkSpinButton*) * 13 );
	
	gtk_init (&argc, &argv);
	
	/* init builder */
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "src/window.xml", NULL);
	gtk_builder_connect_signals(builder, NULL);
	
	window = GTK_WIDGET( GTK_GET_OBJ("mainWindow") );
	screen_view = GTK_WIDGET( GTK_GET_OBJ("screen") );
	
	init_value_spin_buttons();
	init_tree_view();
	
	g_object_unref( G_OBJECT(builder) );
		
	gtk_widget_show_all(window);
	
	update_all_value_regs();
	update_visible_mem_tree();
	
	emulator_running = FALSE;
	g_timeout_add(250, (GSourceFunc)poll_events, NULL);
	gtk_main();
}

