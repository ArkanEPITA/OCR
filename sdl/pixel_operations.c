// Simple get/put pixel for SDL
// Inspired by code from SDL documentation
// (http://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html)

#include <err.h>
#include "pixel_operations.h"

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
SDL_Surface* copy_image(SDL_Surface* image_surface)
{
  Uint32 pixel;
  SDL_Surface* image2;
  image2 = SDL_CreateRGBSurface(SDL_HWSURFACE, image_surface -> w, image_surface -> h, image_surface -> format -> BitsPerPixel,0,0,0,0);
  for(int i = 0; i < image_surface->w; i++)
  {
    for(int j = 0; j < image_surface->h; j++)
    {
      pixel = getpixel(image_surface, i, j);
      putpixel(image2, i, j, pixel);
    }
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
            Uint32 pixel = getpixel(image_surface, j, i);
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
double* matrixFromFile(char *filename)
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
//Create & return the double* wanteed pixels values from filename
double* wanteddArray(char letter)
{
    double* wantedd = calloc(52, sizeof(double));

    if(letter >= 'A' && letter <= 'Z')
    {
        wanteed[(int)(letter) - 65] = 1;
    }
    else if(letter >= 'a' && letter <= 'z')
    {
        wanteed[((int)(letter) - 97) + 26] = 1;
    }
    return goal;
}

//Create & return all the wanteed matrixes (for all letters)
double** wanteedMatrix()
{
  double** wanteddMatrix = malloc(sizeof(double*) * 52);
  char maj = 'A';
  char min = 'a';

  for(int i = 0; i < 52; i++)
  {
    if(i < 26)
    {
      wanteedMatrix[i] = wanteedArray(maj);
      maj++;
    }
    else
    {
      wanteedMatrix[i] = wanteedArray(min);
      min++;
    }
  }
  return wanteedMatrix;
}
*/

//Create & return all the letters matrixes (for all letters)
double** lettersMatrix()
{
  //Variables
  char uppercase_path[14] = "uppercase/0/00.txt\0";
  char lowercase_path[14] = "lowercase/0/00.txt\0";
  double** lettersMatrix = malloc(sizeof(double *) * 52);
  char uppercase = 'A';
  char lowercase = 'a';
  char count = '4';

  for(int i = 0; i < 52; i++)
  {

    if(i < 26)
    {
      uppercase_path[5] = uppercase;
      uppercase_path[7] = uppercase;
      uppercase_path[8] = count;
      lettersMatrix[i] = matrixFromFile(uppercase_path);
      uppercase++;

    }
    else if(i >= 26)
    {
      count = '3';
      lowercase_path[5] = lowercase;
      lowercase_path[7] = lowercase;
      lowercase_path[8] = count;
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
            Uint32 pixel = getpixel(image_surface, j, i);
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