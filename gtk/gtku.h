# ifndef GTKU_H_
# define GTKU_H_

# include <stdlib.h>
# include <stdio.h>
# include <err.h>
# include <SDL/SDL.h>
# include <SDL/SDL_image.h>
# include <gtk/gtk.h>


typedef struct 
{
	gpointer user_data;
	GtkBuilder* builder;
}Data;

#define UNUSED(x) (void)(x)

void create_window(int argc, char* argv[]);	

int traiNN();


#endif