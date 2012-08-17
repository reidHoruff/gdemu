#ifndef _GUI_H_
#define _GUI_H_

#include "dcpu.h"
#include <gtk/gtk.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 92

void init_gui(int , char**);
void init_value_spin_buttons(void);
void init_tree_view(void);
GtkTreeModel* create_and_fill_mem_model(void);

void update_all_value_regs(void);
void update_visible_mem_tree(void);
void update_mem_location(udcpu_t, udcpu_t);

void draw_char(cairo_t*, udcpu_t, udcpu_t, udcpu_t, udcpu_t, udcpu_t, udcpu_t);
void gui_put_pixel( cairo_t *cr, udcpu_t, udcpu_t, udcpu_t );

#define GTK_GET_OBJ(x) gtk_builder_get_object(builder, x)

#include "gui.c"
#endif
