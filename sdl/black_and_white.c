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



void grayscale(SDL_Surface* image_surface)
{
    for(int i = 0; i < image_surface->w;i++)
    {
	    for(int j = 0; j < image_surface->h; j++)
	    {
		    Uint32 pixel = get_pixel(image_surface, i, j);
        	Uint8 r, g, b;
		    SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		    Uint8 average = 0.299*r + 0.587*g + 0.114*b;
		    Uint32 pixel2;

            pixel2 = SDL_MapRGB(image_surface->format, average , average, average);

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




    //normalisation du masque
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            masque[i][j] /= S;
        }
    }

    //application du masque
    int calculr = 0;
    int calculg = 0;
    int calculb = 0;
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
                    Uint8 r, g, b;
                    SDL_GetRGB(get_pixel(image_surface, k + i, l + j), image_surface->format, &r, &g, &b);
                    calculr += masque[(k + 1)][l + 1] * r;
                    calculg += masque[(k + 1)][l + 1] * g;
                    calculb += masque[(k + 1)][l + 1] * b;
                }
            }

            Uint8 r, g, b;
            Uint32 pixel3 = get_pixel(image_surface, i, j);
		    SDL_GetRGB(pixel3, image_surface->format, &r, &g, &b);

            //int abs_calcul = abs(calcul);
            //printf("%d\n", abs_calcul);

            int abs_calculr = abs(calculr);
            int abs_calculg = abs(calculg);
            int abs_calculb = abs(calculb);

            Uint32 pixel2 = SDL_MapRGB(image_surface->format, abs_calculr, abs_calculg, abs_calculb);
            put_pixel(image_surface, i, j, pixel2);

            calculr = 0;
            calculg = 0;
            calculb = 0;
        }
    }
}




void dilatation(SDL_Surface* image_surface)
{
    double masque[3] = {1, 1, 1};
    
    int height = image_surface->h;
    int weight = image_surface->w;



    for(int i = 1; i < weight-2; i++)
    {
        for(int j = 1; j < height-2; j++)
        {

            int calculr = 0;
            int calculg = 0;
            int calculb = 0;

            Uint32 pixel = get_pixel(image_surface, i+1, j);
            Uint8 r, g, b;
		    SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            for(int k = -1; k <= 1; k++)
            {
                Uint32 pixel_suivant = get_pixel(image_surface, i+k+1, j);
                Uint8 r1, g1, b1;
		        SDL_GetRGB(pixel_suivant, image_surface->format, &r1, &g1, &b1);

                calculr = fmax(calculr, - r + masque[k+1] * r1);
                calculg = fmax(calculg, - g + masque[k+1] * g1);
                calculb = fmax(calculb, - b + masque[k+1] * b1);
            }

            //printf("%d, %d, %d\n", (calculr + r), (calculg + g), (calculb + b));
            
            int rfin = calculr + r;
            int gfin = calculg + g;
            int bfin = calculb + b;

            if(rfin < 0)
            {
                rfin = 0;
            }
            else if(rfin > 255)
            {
                rfin = 255;
            }

            if(gfin < 0)
            {
                gfin = 0;
            }
            else if(gfin > 255)
            {
                gfin = 255;
            }

            if(bfin < 0)
            {
                bfin = 0;
            }
            else if (bfin > 255)
            {
                bfin = 255;
            }

            Uint32 pixel2 = SDL_MapRGB(image_surface->format, rfin, gfin, bfin);
            put_pixel(image_surface, i, j, pixel2);
        }
    }
}


void erosion(SDL_Surface* image_surface)
{
    double masque[3] = {1, 1, 1};
    
    int height = image_surface->h;
    int weight = image_surface->w;

    for(int i = 1; i < weight-2; i++)
    {
        for(int j = 1; j < height-2; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
		    SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            int calculr = r;
            int calculg = g;
            int calculb = b;

            for(int k = -1; k <= 1; k++)
            {
                for(int l = -1; l <= 1; l++)
                {
                    Uint32 pixel_suivant = get_pixel(image_surface, i+k, j);
                    Uint8 r1, g1, b1;
		            SDL_GetRGB(pixel_suivant, image_surface->format, &r1, &g1, &b1);

                    calculr = fmin(calculr, r - masque[k+1] * r1);
                    calculg = fmin(calculg, g - masque[k+1] * g1);
                    calculb = fmin(calculb, b - masque[k+1] * b1);
                }
            }

            //printf("%d, %d, %d\n", (calculr + r), (calculg + g), (calculb + b));
            
            int rfin = calculr + r;
            int gfin = calculg + g;
            int bfin = calculb + b;

            if(rfin < 0)
            {
                rfin = 0;
            }
            else if(rfin > 255)
            {
                rfin = 255;
            }

            if(gfin < 0)
            {
                gfin = 0;
            }
            else if(gfin > 255)
            {
                gfin = 255;
            }

            if(bfin < 0)
            {
                bfin = 0;
            }
            else if (bfin > 255)
            {
                bfin = 255;
            }

            Uint32 pixel2 = SDL_MapRGB(image_surface->format, rfin, gfin, bfin);
            put_pixel(image_surface, i, j, pixel2);

            //calculr = 255;
            //calculg = 255;
            //calculb = 255;
        }
    }
}


void gradient_vertical(SDL_Surface* image_surface)
{
    double masque[3] = {0, 0, 1};
    
    int height = image_surface->h;
    int weight = image_surface->w;
    
    int calcul_r_dil = 0;
    int calcul_g_dil = 0;
    int calcul_b_dil = 0;

    for(int i = 1; i < weight-2; i++)
    {
        for(int j = 1; j < height-2; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
		    SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            int calcul_r_ero = r;
            int calcul_g_ero = g;
            int calcul_b_ero = b;

            for(int k = -1; k <= 1; k++)
            {
                Uint32 pixel_dilatation = get_pixel(image_surface, i+k+1, j+1);
                Uint8 r1, g1, b1;
                SDL_GetRGB(pixel_dilatation, image_surface->format, &r1, &g1, &b1);
/*
                Uint32 pixel_erosion = get_pixel(image_surface, i+k, j+l);
                Uint8 r2, g2, b2;
		        SDL_GetRGB(pixel_erosion, image_surface->format, &r2, &g2, &b2);
*/
                calcul_r_ero = fmin(calcul_r_ero, r - masque[k+1] * r1);
                calcul_g_ero = fmin(calcul_g_ero, g - masque[k+1] * g1);
                calcul_b_ero = fmin(calcul_b_ero, b - masque[k+1] * b1);

                calcul_r_dil = fmax(calcul_r_dil, - r + masque[k+1] * r1);
                calcul_g_dil = fmax(calcul_g_dil, - g + masque[k+1] * g1);
                calcul_b_dil = fmax(calcul_b_dil, - b + masque[k+1] * b1);
            }
/*
            printf("ERO");
            printf("\nr : %d, g : %d, b : %d\n\n", calcul_r_ero, calcul_g_ero, calcul_b_ero);

            printf("DIL");
            printf("\nr : %d, g : %d, b : %d\n\n", calcul_r_dil, calcul_g_dil, calcul_b_dil);
*/

            int rfin = calcul_r_dil - calcul_r_ero;
            int gfin = calcul_g_dil - calcul_g_ero;
            int bfin = calcul_b_dil - calcul_b_ero;


            if(rfin < 0)
            {
                rfin = 0;
            }
            else if(rfin > 255)
            {
                rfin = 255;
            }

            if(gfin < 0)
            {
                gfin = 0;
            }
            else if(gfin > 255)
            {
                gfin = 255;
            }

            if(bfin < 0)
            {
                bfin = 0;
            }
            else if (bfin > 255)
            {
                bfin = 255;
            }

            Uint32 pixel2 = SDL_MapRGB(image_surface->format, rfin, gfin, bfin);
            put_pixel(image_surface, i, j, pixel2);


            calcul_r_dil = 0;
            calcul_g_dil = 0;
            calcul_b_dil = 0;
        }
    }
}


void binarisation(SDL_Surface* image_surface)
{
    for(int i = 0; i < image_surface->w;i++)
    {
	    for(int j = 0; j < image_surface->h; j++)
	    {
		    Uint32 pixel = get_pixel(image_surface, i, j);
        	Uint8 r, g, b;
		    SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		    Uint8 average = 0.299*r + 0.587*g + 0.114*b;
		    Uint32 pixel2;
            

            if(average > 100)
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



int main()
{
    SDL_Surface* image_surface;
    SDL_Surface* screen_surface;

    init_sdl();

    image_surface = load_image("gaussien.png");
    
    screen_surface = display_image(image_surface);

    wait_for_keypressed();


    //INSERER ICI FONCTIONS ANNEXES

    grayscale(image_surface);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    filtre_gaussien(image_surface);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();


    gradient_vertical(image_surface);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    binarisation(image_surface);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    SDL_FreeSurface(image_surface);
    SDL_FreeSurface(screen_surface);



    return 0;
}
