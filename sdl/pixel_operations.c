// Simple get/put pixel for SDL
// Inspired by code from SDL documentation
// (http://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html)

#include "pixel_operations.h"

#define TEMPS       30 //Time between angles

void init_sdl()
{
    // Init only the video part.
    // If it fails, die with an error message.
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
        errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
}


SDL_Surface* load_image(char *path)
{
    SDL_Surface *img;

    // Load an image using SDL_image with format detection.
    // If it fails, die with an error message.
    img = IMG_Load(path);
    if (!img)
        errx(3, "can't load %s: %s", path, IMG_GetError());

    return img;
}


SDL_Surface* display_image(SDL_Surface *img)
{
    SDL_Surface *screen;

    // Set the window to the same size as the image
    screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
    if (screen == NULL)
    {
        // error management
        errx(1, "Couldn't set %dx%d video mode: %s\n",
                img->w, img->h, SDL_GetError());
    }

    // Blit onto the screen surface
    if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
        warnx("BlitSurface error: %s\n", SDL_GetError());

    // Update the screen
    SDL_UpdateRect(screen, 0, 0, img->w, img->h);

    // return the screen for further uses
    return screen;
}


void wait_for_keypressed()
{
    SDL_Event event;

    // Wait for a key to be down.
    do
    {
        SDL_PollEvent(&event);
    } while(event.type != SDL_KEYDOWN);

    // Wait for a key to be up.
    do
    {
        SDL_PollEvent(&event);
    } while(event.type != SDL_KEYUP);
}


void SDL_FreeSurface(SDL_Surface *surface);


static inline
Uint8* pixel_ref(SDL_Surface *surf, unsigned x, unsigned y)
{
    int bpp = surf->format->BytesPerPixel;
    return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y)
{
    Uint8 *p = pixel_ref(surface, x, y);

    switch (surface->format->BytesPerPixel)
    {
        case 1:
            return *p;

        case 2:
            return *(Uint16 *)p;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32 *)p;
    }

    return 0;
}

void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel)
{
    Uint8 *p = pixel_ref(surface, x, y);

    switch(surface->format->BytesPerPixel)
    {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}

void update_surface(SDL_Surface* screen, SDL_Surface* image)
{
    if (SDL_BlitSurface(image, NULL, screen, NULL) < 0)
        warnx("BlitSurface error: %s\n", SDL_GetError());

    SDL_UpdateRect(screen, 0, 0, image->w, image->h);
}


/* Copy a SDL_Surface */
SDL_Surface* copy_image(SDL_Surface* image_surface, int imin, int imax, int jmin, int jmax)
{
  Uint32 pixel;
  SDL_Surface* image2;
  image2 = SDL_CreateRGBSurface(SDL_HWSURFACE, jmax - jmin, imax - imin, image_surface -> format -> BitsPerPixel,0,0,0,0);
  int j2 = 0;
  for(int i = jmin; i < jmax; i++)
  {
    int i2 = 0;
    for(int j = imin; j < imax; j++)
    {
      pixel = get_pixel(image_surface, i, j);
      put_pixel(image2, j2, i2, pixel);
      i2 += 1;
    }
    j2 += 1;
  }
  return(image2);
}

//Create the .txt file with the values of pixels
void create_matrix_file_txt(SDL_Surface* image_surface, char* filename)
{
    //Variables
    Uint8 r;
    Uint8 g;
    Uint8 b;

    strtok(filename, ".");
    strcat(filename, ".txt");
    FILE* file = fopen(filename, "w");
    printf("%s\n",filename);

    for(int i = 0; i < image_surface->h; i++)
    {
        for(int j = 0;j < image_surface->w; j++)
        {
            Uint32 pixel = get_pixel(image_surface, j, i);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            if(r == 0 && g == 0 && b == 0)
            {
                fputs("1", file);
            }
            else
            {
                fputs("0", file);
            }
        }
        fputs("\n", file);
    }
    fclose(file);
}

void print_matrix(double matrix[], size_t lines, size_t cols)
{
    for(size_t i = 0; i < lines; i++)
    {
        for(size_t j = 0; j < cols; j++)
        {
            printf("%d", (int)matrix[j + i * cols]);
        }
        printf("\n");
    }
}

//Create & return the double* pixels values from filename
double* matrixFromFile(char* filename)
{
    double* matrix = malloc(sizeof(double) * 28 * 28);
    FILE* file = fopen(filename, "r");

    if(file == NULL)
    {
        printf("File is NULL \n");
    }

    for(int i = 0; i <= 28; i++)
    {
        for(int j = 0; j <= 28; j++)
        {
            int c = fgetc(file);
            if(c == 49)
            matrix[j+i*28] = 1;
            if(c == 48)
            matrix[j+i*28] = 0;
        }
    }
    fclose(file);
    return matrix;
}


/*
//Create & return the double* wanted pixels values from filename
double* wantedArray(char letter)
{
    double* wanted = calloc(52, sizeof(double));

    if(letter >= 'A' && letter <= 'Z')
    {
        wanted[(int)(letter) - 65] = 1;
    }
    else if(letter >= 'a' && letter <= 'z')
    {
        wanted[((int)(letter) - 97) + 26] = 1;
    }
    return wanted;
}

//Create & return all the wanted matrixes (for all letters)
double** wantedMatrix()
{
  double** wantedMatrix = malloc(sizeof(double*) * 52);
  char maj = 'A';
  char min = 'a';

  for(int i = 0; i < 52; i++)
  {
    if(i < 26)
    {
      wantedMatrix[i] = wantedArray(maj);
      maj++;
    }
    else
    {
      wantedMatrix[i] = wantedArray(min);
      min++;
    }
  }
  return wantedMatrix;
}
*/

//Create & return all the letters matrixes (for all letters)
double** lettersMatrix()
{
  //Variables
  char uppercase_path[19] = "uppercase/0/00.txt\0";
  char lowercase_path[19] = "lowercase/0/00.txt\0";
  double** lettersMatrix = malloc(sizeof(double *) * 52);
  char uppercase = 'A';
  char lowercase = 'a';
  char count = '4';

  for(int i = 0; i < 52; i++)
  {

    if(i < 26)
    {
      uppercase_path[10] = uppercase;
      uppercase_path[12] = uppercase;
      uppercase_path[13] = count;
      lettersMatrix[i] = matrixFromFile(uppercase_path);
      uppercase++;

    }
    else if(i >= 26)
    {
      count = '3';
      lowercase_path[10] = lowercase;
      lowercase_path[12] = lowercase;
      lowercase_path[13] = count;
      lettersMatrix[i] = matrixFromFile(lowercase_path);
      lowercase++;

    }
  }
  return lettersMatrix;
}

//Create & return the letters matrix binarisation
double* create_matrix_letter(SDL_Surface *image_surface)
{
    //Variables
    double* letterMatrix = malloc(sizeof(double) * 28 * 28);
    Uint8 r;
    Uint8 g;
    Uint8 b;

    for(int i = 0; i < image_surface->h; i++)
    {
        for(int j = 0; j < image_surface->w; j++)
        {
            Uint32 pixel = get_pixel(image_surface, j, i);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            if(r == 0 && g == 0 && b == 0)
            {
                letterMatrix[j + i * image_surface->w] = 1;
            }
            else
            {
                letterMatrix[j + i * image_surface->w] = 0;
            }
        }
    }
    return letterMatrix;
}

SDL_Surface* Resize_letter(SDL_Surface *image_surface)
{
  SDL_Surface* destination = SDL_CreateRGBSurface(SDL_HWSURFACE, 28, 28, image_surface->format->BitsPerPixel,0,0,0,0);
  SDL_SoftStretch(image_surface, NULL, destination, NULL);
  return destination;
}

SDL_Surface* Resize(SDL_Surface *image_surface)
{
  SDL_Surface* destination = SDL_CreateRGBSurface(SDL_HWSURFACE, 576, 460, image_surface->format->BitsPerPixel,0,0,0,0);
  SDL_SoftStretch(image_surface, NULL, destination, NULL);
  return destination;
}
 
int rotation_angles(int argc, char *argv[])
{
    SDL_Surface *ecran = NULL, *image = NULL, *rotation = NULL;
    SDL_Rect rect;
    SDL_Event event;
    double angle = 0;
 
    int continuer = 1;
    int tempsPrecedent = 0, tempsActuel = 0;
 
    SDL_Init(SDL_INIT_VIDEO);
 
    ecran = SDL_SetVideoMode(500, 500, 32, SDL_HWSURFACE);
    SDL_WM_SetCaption("Faire des rotations avec SDL_gfx", NULL);
 
    image = SDL_LoadBMP("image.bmp");
 
    while(continuer)
    {
        SDL_PollEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
                break;
        }
 
        tempsActuel = SDL_GetTicks();
        if (tempsActuel - tempsPrecedent > TEMPS)
        {
            angle += 2; //On augmente l'angle pour que l'image tourne sur elle-même.
 
            tempsPrecedent = tempsActuel;
        }
        else
        {
            SDL_Delay(TEMPS - (tempsActuel - tempsPrecedent));
        }
 
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
 
        rotation = rotozoomSurface(image, angle, 1.0, 1); //On transforme la surface image.
 
        //On positionne l'image en fonction de sa taille.
        rect.x =  200 - rotation->w / 2;
        rect.y =  200 - rotation->h / 2;
 
        SDL_BlitSurface(rotation , NULL, ecran, &rect); //On affiche la rotation de la surface image.
        SDL_FreeSurface(rotation); //On efface rotation car on va la redéfinir dans la prochaine boucle. Si on ne le fait pas, cela crée une fuite de mémoire. 
 
        SDL_Flip(ecran);
    }
 
    SDL_FreeSurface(ecran);
    SDL_FreeSurface(image);
    SDL_Quit();
 
    return EXIT_SUCCESS;
}