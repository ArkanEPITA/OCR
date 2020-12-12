# include "gtku.h"
# include <SDL/SDL_rotozoom.h>
# include "../sdl/pixel_operations.h"
#include "../neural_network/neural_network.h"

#define TEMPS       30 // Le temps qu'il y a entre chaque augmentation de l'angle.


gchar* file = "";
char* sentence = "";
GtkWidget* parent_window;
int* goal = 0;	

void loading_image(GtkButton* button, GtkImage* image)
{
	if(goal == 0)
	{
		if(strcmp(file,"") == 0)
			return;
		UNUSED(button);
		SDL_Surface *img = IMG_Load((char *)file);
		if(img->w > 576 && img->h > 460)
		{

			printf("Need Resize \n");
			//SDL_Surface *new = imageResize(img,576,460);
			SDL_Surface *new = Resize(img, 576, 460);

			SDL_SaveBMP(new,"save_image/image_resize");
		//SDL_FreeSurface(new);
			gtk_image_set_from_file (GTK_IMAGE (image), "save_image/image_resize");
		}
		else
			gtk_image_set_from_file (GTK_IMAGE (image), file);
	}
	else
	{
		gtk_image_set_from_file (GTK_IMAGE (image), "save_image/out.bmp");
	}
	
}

SDL_Surface* show_image(SDL_Surface *image_surface, double angle)
{
    // Return the frame buffer surface after rotation

    float angle_radial;
    float cosinus;
    float sinus;
    double wsize;
    double hsize;
    SDL_Surface *screen;
  
    // convert angle from degrees to radians //
    angle_radial = -angle * M_PI / 180.0;
 
    // memorize the sine and cosine values ​​of the angle //
    cosinus = cos(angle_radial);
    sinus = sin(angle_radial);
 
    // destination image size calculation //
    // (smallest integer value of image width and high) //
    wsize = ceil(image_surface->w * fabs(cosinus) + image_surface->h * fabs(sinus));
    hsize = ceil(image_surface->w * fabs(sinus) + image_surface->h * fabs(cosinus));

    // Set the window to the new size
    screen = SDL_SetVideoMode(wsize, hsize, 32, SDL_HWSURFACE);
    if (screen == NULL)
    {
        // error management
        errx(1, "Couldn't set %dx%d video mode: %s\n", image_surface->w, image_surface->h, SDL_GetError());
    }
	
    // return the screen surface for further uses
    return screen;
}



int rotation_angles()
{
    SDL_Surface *screen = NULL, *image = NULL, *rotation = NULL;
    
    SDL_Rect rect;	//we put at the origin
    rect.x = 0;
    rect.y = 0;	
    
    SDL_Event event;
    
    double angle = 0;
    printf("entrez un degré de rotation :");
    if (scanf("%lf", &angle)!=1)
	{
	     printf("Failed to read integer.\n");
	}
 
    int not_finish = 1;
    SDL_Init(SDL_INIT_VIDEO);
    //image = loading("rotation.png");
	image = IMG_Load((char*)file);
    screen = show_image(image, angle);

    //makes the background of the image white
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 255));
    // do the rotation with a zoom of 1  and image smoothing
	
    rotation = rotozoomSurface(image, angle, 1.0, 1);
    // Blit onto the screen surface after rotation

	SDL_BlitSurface(rotation , NULL, screen, &rect);
	SDL_Flip(screen); //updates all screen settings
	while(not_finish) //while we do not close the screen
	{
		SDL_WaitEvent(&event);
		switch(event.type) //we close the screen
		{
			case SDL_QUIT:
				not_finish = 0;
				break;
		}
	}
    
    SDL_Quit(); //exits by closing the screen
	// Création d'une copie du pixbuf modifié
	rotation = Resize(rotation, 576, 460);
	SDL_SaveBMP(rotation, "save_image/out.bmp");
	file = "save_image/out.bmp";
	goal += 1;
	
    return EXIT_SUCCESS;
}


void save_text(GtkButton* button, GtkTextBuffer* buffer)
{
  UNUSED(button);
  UNUSED(buffer);

  GtkWidget* dialog;
  GtkWidget* toplevel = gtk_widget_get_toplevel(GTK_WIDGET(button));
  dialog = gtk_file_chooser_dialog_new ("Save Text", GTK_WINDOW (toplevel), GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, NULL);
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
  {
    char* file;
    file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

    // set the contents of the file to the sentence from the buffer 
    g_file_set_contents(file, sentence, strlen(sentence), NULL);
  }
  gtk_widget_destroy(dialog);
}


void openFile(GtkButton* button, GtkLabel* textlabel)
{
    GtkWidget* label = (GtkWidget*)textlabel;
	GtkWidget* toplevel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	GtkWidget* dialog = gtk_file_chooser_dialog_new(("Open image"), GTK_WINDOW(toplevel), GTK_FILE_CHOOSER_ACTION_OPEN, "Open", GTK_RESPONSE_ACCEPT, "Cancel", GTK_RESPONSE_CANCEL, NULL);

	switch (gtk_dialog_run(GTK_DIALOG(dialog)))
	{
		case GTK_RESPONSE_ACCEPT:
		{
			goal = 0;
			file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog));
      		gtk_label_set_text(GTK_LABEL(label), file);
			break;
		}
		default:
			break;
	}
	gtk_widget_destroy(dialog);
}

int traiNN()
{
	train();
	return EXIT_SUCCESS;
}



int launchOCR(GtkButton* button, GtkTextBuffer* buffer)
{
  if(strcmp(file,"") == 0)
  {
    return 1;
  }
	UNUSED(button);
	UNUSED(buffer);
	SDL_Init(SDL_INIT_VIDEO);

	printf("%s\n ",file);
	SDL_Surface* image = IMG_Load((char*)file);
	SDL_Surface* true_image = IMG_Load((char*)file);

	

	sentence = final(image, true_image);
	
	//SDL_Surface* image_cut = lineCut(img);
	printf("Line Cuts\n");
	
	printf("Character cuts\n");
  	//struct Neural_Network* network = ExtractData();
	printf("Extract Data Done \n");

	//isolateLine(image_cut,net);
	printf("Isolate Line Done \n");

  	gtk_text_buffer_set_text(buffer, sentence, strlen(sentence));

  	//sentence = net->str;
	printf("Finish Treatment\n");

	SDL_Quit();
	return EXIT_SUCCESS;
}




void create_window(int argc, char* argv[])
{
  	//Init variables
  	GtkWidget* main_window;
  	Data data;

  	//Init GTK
  	gtk_init(&argc, &argv);

  	//Build from .glade
  	data.builder = gtk_builder_new();
  	gtk_builder_add_from_file(data.builder, "gtk.glade", NULL);

	//Get main_window
	main_window =  GTK_WIDGET(gtk_builder_get_object(data.builder,"main_window"));
  	parent_window = main_window;

	//Connect signals
	gtk_builder_connect_signals(data.builder, &data);

  	gtk_window_set_title(GTK_WINDOW(main_window), "prOCRastination");
  	gtk_widget_show_all(main_window);
  	gtk_main();
}
