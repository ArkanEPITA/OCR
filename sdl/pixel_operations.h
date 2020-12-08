#ifndef PIXEL_OPERATIONS_H_
#define PIXEL_OPERATIONS_H_

#include <stdlib.h>
#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <math.h>
#include <errno.h>

typedef struct Word Word;

struct Word
{
    int h_word;
    int v_word;
    int maxi_word;
    int mini_word;
    int maxj_word;
    int minj_word;
};

typedef struct Array_word Array_word;

struct Array_word
{
    Word* word;
    int length;
};

Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y);

void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);

void wait_for_keypressed(void);

SDL_Surface* display_image(SDL_Surface* image_surface);

SDL_Surface* copy_image(SDL_Surface* image_surface);

void create_matrix_file_txt(SDL_Surface* image_surface, char* filename);

void print_matrix(double matrix[], size_t lines, size_t cols);

double* matrixFromFile(char* filename);

//double* wantedArray(char letter);

//double** wantedMatrix();

double** lettersMatrix();

double* create_matrix(SDL_Surface* image_surface);

void greyscale(SDL_Surface* image_surface);

SDL_Surface* Resize(SDL_Surface *image_surface);

void binarisation(SDL_Surface* image_surface);


#endif
