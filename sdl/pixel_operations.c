
// Simple get/put pixel for SDL
// Inspired by code from SDL documentation
// (http://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html)

#include "pixel_operations.h"

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

SDL_Surface* Resize(SDL_Surface *image_surface, size_t l, size_t c)
{
  SDL_Surface* destination = SDL_CreateRGBSurface(SDL_HWSURFACE, l, c, image_surface->format->BitsPerPixel,0,0,0,0);
  SDL_SoftStretch(image_surface, NULL, destination, NULL);
  return destination;
}