#ifndef PIXEL_OPERATIONS_H_
#define PIXEL_OPERATIONS_H_

#include <stdlib.h>
#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <math.h>
#include <errno.h>
#include <string.h>
#include <time.h>

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
    int weigth;
};

typedef struct Block Block;

struct Block{
    SDL_Surface ** images;
    int* left;
    int* up;
    int* down;
    int nb_block;
};

Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y);

void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);

void wait_for_keypressed(void);

SDL_Surface* display_image(SDL_Surface* image_surface);

SDL_Surface* copy_image(SDL_Surface* image_surface, int imin, int imax, int jmin, int jmax);

void create_matrix_file_txt(SDL_Surface* image_surface, char* filename);

void print_matrix(double matrix[], size_t lines, size_t cols);

double* create_matrix(SDL_Surface* image_surface);

void greyscale(SDL_Surface* image_surface);

SDL_Surface* Resize(SDL_Surface *image_surface, size_t l, size_t c);

void binarisation(SDL_Surface* image_surface);

void init_sdl();

SDL_Surface* load_image(char *path);

SDL_Surface* display_image(SDL_Surface *img);

void wait_for_keypressed();

void SDL_FreeSurface(SDL_Surface *surface);

void update_surface(SDL_Surface* screen, SDL_Surface* image);

SDL_Surface* Resize_letter(SDL_Surface *image_surface);

double* create_matrix_letter(SDL_Surface *image_surface);

char* final(SDL_Surface* image_surface, SDL_Surface* true_surface);

char* print_line(SDL_Surface* image_surface, int alinea, int begin);

struct Neural_Network
{
    int nbInput;
    int nbHidden;
    int nbOutput;

    //Arrays
    double InputValue[52][784];
    double Goal[52][52];
    //Weight Arrays
    double WeightIH[64][784];
    double WeightHO[52][64];
    //Bias Array
    double BiasH[64];
    //Bias output
    double BiasO[52];
    //Output Hidden
    double OutputH[64];
    //Output of output
    double OutputO[52];
    //Array of delta bias for hidden
    double dBiasH[64];
    //delta bias for output
    double dBiasO[52];
    //delta weight
    double dWeightIH[64][784];
    double dWeightHO[52][64];
    //delta of output
    double dOutputO[52];
    //delta of hidden
    double dHiddenH[64];

    char act;
    
    double ErrorRate;
    double eta;
    double alpha;
};

double Random();

double sigmoid(double x);

double dSigmoid(double z);   //derivative of the activating sigmoid function

void SaveData(struct Neural_Network *net);

void LoadData(struct Neural_Network *net);

void free_array(struct Neural_Network *net);

void InitalizeValue(struct Neural_Network *net);

void ForwardPass(struct Neural_Network *net, int p, int epoch);

void BackwardPass(struct Neural_Network *net, int p);

void train();

char run(double letter[784]);

struct Neural_Network *InitalizeNetwork();



#endif
