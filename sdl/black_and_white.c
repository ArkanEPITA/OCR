#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"
#include <math.h>
#include <errno.h>

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



void black_and_white(SDL_Surface* image_surface)
{
    for(int i = 0; i < image_surface->w;i++)
    {
	    for(int j = 0; j < image_surface->h; j++)
	    {
		    Uint32 pixel = get_pixel(image_surface, i, j);
        	Uint8 r, g, b;
		    SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		    Uint8 average = 0.3*r + 0.59*g + 0.11*b;
		    Uint32 pixel2;

            if(average > 150)
            {
                pixel2 = SDL_MapRGB(image_surface->format, 255, 255, 255);
            }
            else
            {
                pixel2 = SDL_MapRGB(image_surface->format, 0, 0, 0);
            }
            


		    put_pixel(image_surface, i, j, pixel2);
	    }
    }
}


void filtre_gaussien(SDL_Surface* image_surface)
{
    double masque[3][3] = {{1, 2, 1},{2, 4, 2}, {1, 2, 1}};
    //double sigma = 0.8;
    //double k = 2.0 * sigma * sigma;
    double S = 16.0;
    //int p;

/*
    //calcul du masque
    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; i++)
        {
            p = -(i*i + j*j);
            masque[(i+1) * 3 + j+1] = (exp(p/k))/(k*M_PI);
            S += masque[(i+1) * 3 + j+1];
        }
    }

*/

    //normalisation du masque
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            masque[i][j] /= S;
        }
    };

    //application du masque
    int calcul = 0;
    int height = image_surface->h;
    int weight = image_surface->w;
    for(int i = 1; i < weight-2; i++)
    {
        for(int j = 1; j < height-2; j++)
        {
            for(int k = -1; k <=1; k++)
            {
                for(int l = -1; l <= 1; l++)
                {
                    Uint32 pixel = get_pixel(image_surface, k + i, l + j);
                    calcul += masque[(k + 1)][l + 1] * pixel;
                }
            }

            Uint8 r, g, b;
            Uint32 pixel = get_pixel(image_surface, i, j);
		    SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            int abs_calcul = abs(calcul);
            Uint32 pixel2 = SDL_MapRGB(image_surface->format, abs_calcul*r, abs_calcul*g, abs_calcul*b);
            put_pixel(image_surface, i, j, pixel2);
            calcul = 0;
        }
    }
}




void dilatation(SDL_Surface* image_surface)
{
    double masque[3][3] = {{0, 1, 0},{0, 1, 0}, {0, 1, 0}};
    

    int calculr = 0;
    int calculg = 0;
    int calculb = 0;

    int height = image_surface->h;
    int weight = image_surface->w;

    //int matrix[weight][height];

    for(int i = 1; i < weight-2; i++)
    {
        for(int j = 1; j < height-2; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
		    SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            for(int k = -1; k <= 1; k++)
            {
                for(int l = -1; l <= 1; l++)
                {
                    Uint32 pixel_suivant = get_pixel(image_surface, i+k, j+l);
                    Uint8 r1, g1, b1;
		            SDL_GetRGB(pixel_suivant, image_surface->format, &r1, &g1, &b1);

                    calculr = fmax(calculr,  r - masque[k + 1][l + 1] * r1);
                    calculg = fmax(calculg,  g - masque[k + 1][l + 1] * g1);
                    calculb = fmax(calculb,  b - masque[k + 1][l + 1] * b1);
                }
            }
            if(calculr < 0)
            {
                calculr = 0;
            }
            if(calculg < 0)
            {
                calculg = 0;
            }
            if(calculb < 0)
            {
                calculb = 0;
            }

            //matrix[i][j] = calcul;
            Uint32 pixel2 = SDL_MapRGB(image_surface->format, calculr+r, calculg+g, calculb+b);
            put_pixel(image_surface, i, j, pixel2);

            calculr = 0;
            calculg = 0;
            calculb = 0;
        }
    }
/*
    for(int i = 1; i < weight-2; i++)
    {
        for(int j = 1; j < height-2; j++)
        {
            Uint32 pixel2 = SDL_MapRGB(image_surface->format, matrix[i][j]+r, matrix[i][j]+g, matrix[i][j]+b);
            put_pixel(image_surface, i, j, pixel2);
        }
    }*/
}


void erosion(SDL_Surface* image_surface)
{
    double masque[3][3] = {{0, 1, 0},{1, 1, 1}, {0, 1, 0}};

    //SDL_Surface* image_surface2 = image_surface;

    //application du masque
    int calcul = 0;
    int height = image_surface->h;
    int weight = image_surface->w;
    for(int i = 1; i < weight-2; i++)
    {
        for(int j = 1; j < height-2; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            for(int k = -1; k <= 1; k++)
            {
                for(int l = -1; l <= 1; l++)
                {
                    calcul = fmin(calcul, pixel - masque[k + 1][l + 1] * get_pixel(image_surface, i+k, j+l));
                }
            }

            Uint8 r, g, b;
		    SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            int abs_calcul = abs(calcul);
            Uint32 pixel2 = SDL_MapRGB(image_surface->format, abs_calcul + r, abs_calcul + g, abs_calcul + b);
            put_pixel(image_surface, i, j, pixel2);
            calcul = 0;
        }
    }
    //image_surface = image_surface2;
}

/*
void trkl(SDL_Surface* image_surface)
{
    SDL_Surface = ???
}


*/

int main()
{
    SDL_Surface* image_surface;
    SDL_Surface* screen_surface;

    init_sdl();

    image_surface = load_image("girl.png");
    
    screen_surface = display_image(image_surface);

    wait_for_keypressed();


    //INSERER ICI FONCTIONS ANNEXES

    /*black_and_white(image_surface);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    filtre_gaussien(image_surface);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();*/

    dilatation(image_surface);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    SDL_FreeSurface(image_surface);
    SDL_FreeSurface(screen_surface);



    return 0;
}
