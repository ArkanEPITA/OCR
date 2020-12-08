# include "gtku.h"


gchar* file = "";
char* sentence = "";
GtkWidget* parent_window;


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
			file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog));
      		gtk_label_set_text(GTK_LABEL(label), file);
			break;
		}
		default:
			break;
	}
	gtk_widget_destroy(dialog);
}

void load_image_gtk(GtkButton* button, GtkImage* image)
{
    if(strcmp(file,"") == 0)
        return;

    UNUSED(button);
	SDL_Surface* image_surface = IMG_Load((char*)file);
	if(image_surface->w > 576 && image_surface->h > 460)
	{
		printf("Need Resize\n");
		SDL_Surface* resize = Resize(image_surface);

		SDL_SaveBMP(resize, "image_resize");
		gtk_image_set_from_file(GTK_IMAGE(image), "image_resize");
	}
	else
	{
    	gtk_image_set_from_file(GTK_IMAGE(image), file);
	}
}


int launchOCR(GtkButton* button, GtkTextBuffer* buffer)
{
  if(strcmp(file,"") == 0)
  {
    return 1;
  }
	UNUSED(button);
	SDL_Init(SDL_INIT_VIDEO);

	printf("%s\n ",file);
	SDL_Surface* image = IMG_Load((char*)file);
	
	greyscale(image);
	printf("Greyscale ! \n");
	
	binarisation(image);
	printf("Binarization ! \n");
	
	//SDL_Surface* image_cut = lineCut(img);
	//printf("Line Cuts\n");
	
	//printf("Character cuts\n");
  	//struct Neural_Network* network = ExtractData();
	//printf("Extract Data Done \n");

	//isolateLine(image_cut,net);
	//printf("Isolate Line Done \n");

  	//gtk_text_buffer_set_text (buffer, network->str, strlen(network->str));

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

  	gtk_window_set_title(GTK_WINDOW(main_window), "prOCRAstination");
  	gtk_widget_show_all(main_window);
  	gtk_main();
}
