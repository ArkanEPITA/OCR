# ifndef GTK_H_
# define GTK_H_

# include <stdlib.h>
# include <stdio.h>
# include <err.h>
# include <SDL/SDL.h>
# include <SDL/SDL_image.h>
# include <gtk/gtk.h>
# include "../sdl/pixel_operations.h"

typedef struct
{
	gpointer user_data;
	GtkBuilder* builder;
} Data;