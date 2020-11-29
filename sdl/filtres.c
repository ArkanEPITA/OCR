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



//=============================
//          Filters
//=============================



void grayscale(SDL_Surface* image_surface)
{
    //Transform the image in gray scales
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
    //Reduce noise by bluring the image


    //double mask[3][3] = {{0.099999, 0.116228, 0.099999},{0.116228, 0.135092, 0.116228}, {0.099999, 0.116228, 0.099999}};

    //Initialization of the mask to apply
    double mask[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}};


    //double sigma = 0.8;
    //double k = 2.0 * sigma * sigma;
    
    //int p;




    //normalization of the mask
    double S = 16.0;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            mask[i][j] /= S;
        }
    }



    //application of the mask
    int set_r = 0;
    int set_g = 0;
    int set_b = 0;

    int height = image_surface->h;
    int weight = image_surface->w;

    //Going through the image
    for(int i = 1; i < weight-2; i++)
    {
        for(int j = 1; j < height-2; j++)
        {
            //Going through the mask
            for(int k = -1; k <=1; k++)
            {
                for(int l = -1; l <= 1; l++)
                {
                    Uint8 r, g, b;
                    SDL_GetRGB(get_pixel(image_surface, k + i, l + j), image_surface->format, &r, &g, &b);
                    set_r += mask[(k + 1)][l + 1] * r;
                    set_g += mask[(k + 1)][l + 1] * g;
                    set_b += mask[(k + 1)][l + 1] * b;
                }
            }

            //apply filter
            Uint8 r, g, b;
            Uint32 pixel3 = get_pixel(image_surface, i, j);
		    SDL_GetRGB(pixel3, image_surface->format, &r, &g, &b);

            //int abs_calcul = abs(calcul);
            //printf("%d\n", abs_calcul);

            int abs_set_r = abs(set_r);
            int abs_set_g = abs(set_g);
            int abs_set_b = abs(set_b);

            Uint32 pixel2 = SDL_MapRGB(image_surface->format, abs_set_r, abs_set_g, abs_set_b);
            put_pixel(image_surface, i, j, pixel2);

            set_r = 0;
            set_g = 0;
            set_b = 0;
        }
    }
}




void dilatation(SDL_Surface* image_surface)
{
    //Initialization of the mask
    double mask[3] = {1, 1, 1};
    
    int height = image_surface->h;
    int weight = image_surface->w;


    //Going through the image
    for(int i = 1; i < weight-2; i++)
    {
        for(int j = 1; j < height-2; j++)
        {
            int set_r = 0;
            int set_g = 0;
            int set_b = 0;

            Uint32 pixel = get_pixel(image_surface, i+1, j);
            Uint8 r, g, b;
		    SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            //Going through the mask
            for(int k = -1; k <= 1; k++)
            {
                Uint32 next_pixel = get_pixel(image_surface, i+k+1, j);
                Uint8 r1, g1, b1;
		        SDL_GetRGB(next_pixel, image_surface->format, &r1, &g1, &b1);

                set_r = fmax(set_r, - r + mask[k+1] * r1);
                set_g = fmax(set_g, - g + mask[k+1] * g1);
                set_b = fmax(set_b, - b + mask[k+1] * b1);
            }

            //printf("%d, %d, %d\n", (set_r + r), (set_g + g), (set_b + b));
            

            int res_r = set_r + r;
            int res_g = set_g + g;
            int res_b = set_b + b;


            //Normalization of the colors
            if(res_r < 0)
            {
                res_r = 0;
            }
            else if(res_r > 255)
            {
                res_r = 255;
            }

            if(res_g < 0)
            {
                res_g = 0;
            }
            else if(res_g > 255)
            {
                res_g = 255;
            }

            if(res_b < 0)
            {
                res_b = 0;
            }
            else if (res_b > 255)
            {
                res_b = 255;
            }


            //Apply filter
            Uint32 pixel2 = SDL_MapRGB(image_surface->format, res_r, res_g, res_b);
            put_pixel(image_surface, i, j, pixel2);
        }
    }
}


void erosion(SDL_Surface* image_surface)
{
    //Initialization of the image
    double mask[3] = {1, 1, 1};
    
    int height = image_surface->h;
    int weight = image_surface->w;

    //Going through the image
    for(int i = 1; i < weight-2; i++)
    {
        for(int j = 1; j < height-2; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
		    SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            int set_r = r;
            int set_g = g;
            int set_b = b;

            //Going through the mask
            for(int k = -1; k <= 1; k++)
            {
                Uint32 next_pixel = get_pixel(image_surface, i+k, j);
                Uint8 r1, g1, b1;
                SDL_GetRGB(next_pixel, image_surface->format, &r1, &g1, &b1);

                set_r = fmin(set_r, r - mask[k+1] * r1);
                set_g = fmin(set_g, g - mask[k+1] * g1);
                set_b = fmin(set_b, b - mask[k+1] * b1);
            }

            //printf("%d, %d, %d\n", (set_r + r), (set_g + g), (set_b + b));
            
            int res_r = set_r + r;
            int res_g = set_g + g;
            int res_b = set_b + b;


            //Normalization of the colors
            if(res_r < 0)
            {
                res_r = 0;
            }
            else if(res_r > 255)
            {
                res_r = 255;
            }

            if(res_g < 0)
            {
                res_g = 0;
            }
            else if(res_g > 255)
            {
                res_g = 255;
            }

            if(res_b < 0)
            {
                res_b = 0;
            }
            else if (res_b > 255)
            {
                res_b = 255;
            }

            //Apply filter
            Uint32 pixel2 = SDL_MapRGB(image_surface->format, res_r, res_g, res_b);
            put_pixel(image_surface, i, j, pixel2);

            //set_r = 255;
            //set_g = 255;
            //set_b = 255;
        }
    }
}


void gradient_vertical(SDL_Surface* image_surface)
{
    //Initialization of the mask
    double mask[3] = {1, 1, 1};
    
    int height = image_surface->h;
    int weight = image_surface->w;
    
    int set_r_dil = 0;
    int set_g_dil = 0;
    int set_b_dil = 0;

    //Going through the image
    for(int i = 1; i < weight-2; i++)
    {
        for(int j = 1; j < height-2; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
		    SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            int set_r_ero = r;
            int set_g_ero = g;
            int set_b_ero = b;

            //Going through the mask
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
                set_r_ero = fmin(set_r_ero, r - mask[k+1] * r1);
                set_g_ero = fmin(set_g_ero, g - mask[k+1] * g1);
                set_b_ero = fmin(set_b_ero, b - mask[k+1] * b1);

                set_r_dil = fmax(set_r_dil, - r + mask[k+1] * r1);
                set_g_dil = fmax(set_g_dil, - g + mask[k+1] * g1);
                set_b_dil = fmax(set_b_dil, - b + mask[k+1] * b1);
            }
/*
            printf("ERO");
            printf("\nr : %d, g : %d, b : %d\n\n", set_r_ero, set_g_ero, set_b_ero);

            printf("DIL");
            printf("\nr : %d, g : %d, b : %d\n\n", set_r_dil, set_g_dil, set_b_dil);
*/

            int res_r = set_r_dil - set_r_ero;
            int res_g = set_g_dil - set_g_ero;
            int res_b = set_b_dil - set_b_ero;

            //Normalization of the colors
            if(res_r < 0)
            {
                res_r = 0;
            }
            else if(res_r > 255)
            {
                res_r = 255;
            }

            if(res_g < 0)
            {
                res_g = 0;
            }
            else if(res_g > 255)
            {
                res_g = 255;
            }

            if(res_b < 0)
            {
                res_b = 0;
            }
            else if (res_b > 255)
            {
                res_b = 255;
            }

            //apply filter
            Uint32 pixel2 = SDL_MapRGB(image_surface->format, res_r, res_g, res_b);
            put_pixel(image_surface, i, j, pixel2);


            set_r_dil = 0;
            set_g_dil = 0;
            set_b_dil = 0;
        }
    }
}


void binarisation(SDL_Surface* image_surface)
{
    //Going through the image
    for(int i = 0; i < image_surface->w;i++)
    {
	    for(int j = 0; j < image_surface->h; j++)
	    {
		    Uint32 pixel = get_pixel(image_surface, i, j);
        	Uint8 r, g, b;
		    SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

		    Uint8 average = 0.299*r + 0.587*g + 0.114*b;

		    Uint32 pixel2;

            //Binarisation
            if(average > 120)
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



void contour_vertical(SDL_Surface* image_surface)
{
    //Going through the image
    for(int i = 0; i < image_surface->w - 1; i++)
    {
        for(int j = 0; j < image_surface->h - 1; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            //next pixel
            Uint32 next_pixel = get_pixel(image_surface, i + 1, j);
            Uint8 r2, g2, b2;
            SDL_GetRGB(next_pixel, image_surface->format, &r2, &g2, &b2);

            Uint8 average = 0.299*r + 0.587*g + 0.114*b;
            Uint8 average2 = 0.299*r2 + 0.587*g2 + 0.114*b2;

            //Detection of the borders
            if(abs(average - average2) > 20)
            {
                pixel = SDL_MapRGB(image_surface->format, 255, 255, 255);
            }
            else
            {
                pixel = SDL_MapRGB(image_surface->format, 0, 0, 0);
            }

            put_pixel(image_surface, i, j, pixel);
        }
    }
}


void fermeture_verticale(SDL_Surface* image_surface, int end)
{
    //Going through the image
    for(int i = 0; i < image_surface->w; i++)
    {
        for(int j = 0; j < image_surface->h - 1; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            //Detection of white pixels
            if (r == 255)
            {

                //Careful to borders
                int save_end = end;
                if(j > image_surface->h - save_end)
                {
                    save_end -= image_surface->h - j;
                }
                int pos = 1;
                int finder = 0;

                //Find a white pixel in the x next pixels
                while(pos <= save_end && finder == 0)
                {
                    Uint32 pixel2 = get_pixel(image_surface, i, j+pos);
                    Uint8 r2, g2, b2;
                    SDL_GetRGB(pixel2, image_surface->format, &r2, &g2, &b2);
                    
                    if (r2 == 255)
                    {
                        finder = 1;
                    }
                    pos++;
                }

                //check if a pixel was found
                if(finder == 1)
                {
                    //apply the filter
                    for(int k = 1; k < pos; k++)
                    {
                        Uint32 pixel3 = get_pixel(image_surface, i, j+k);
                        Uint8 r3, g3, b3;
                        SDL_GetRGB(pixel3, image_surface->format, &r3, &g3, &b3);
                        pixel3 = SDL_MapRGB(image_surface->format, 255, 255, 255);
                        put_pixel(image_surface, i, j+k, pixel3);
                    }
                }
            }
        }
    }
}


void fermeture_horizontale(SDL_Surface* image_surface, int end)
{
    //Going through the image
    for(int j = 0; j < image_surface->h; j++)
    {
        for(int i = 0; i < image_surface->w - 1; i++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            //Detection of white pixels
            if (r == 255)
            {

                //Careful to borders
                int save_end = end;
                if(i > image_surface->w - save_end)
                {
                    save_end -= image_surface->w - i;
                }
                int pos = 1;
                int finder = 0;

                //Find a white pixel in the x next pixels
                while(pos <= save_end && finder == 0)
                {
                    Uint32 pixel2 = get_pixel(image_surface, i+pos, j);
                    Uint8 r2, g2, b2;
                    SDL_GetRGB(pixel2, image_surface->format, &r2, &g2, &b2);
                    if (r2 == 255)
                    {
                        finder = 1;
                    }
                    pos++;
                }

                //check if a pixel was found
                if(finder == 1)
                {
                    //apply the filter
                    for(int k = 1; k < pos; k++)
                    {
                        Uint32 pixel3 = get_pixel(image_surface, i+k, j);
                        Uint8 r3, g3, b3;
                        SDL_GetRGB(pixel3, image_surface->format, &r3, &g3, &b3);
                        pixel3 = SDL_MapRGB(image_surface->format, 255, 255, 255);
                        put_pixel(image_surface, i+k, j, pixel3);
                    }
                }
            }
        }
    }
}

/*


void too_small_area(SDL_Surface* image_surface)
{
    //Detect too small areas
    int max_h = image_surface->h;
    int max_w = image_surface->w;
    int i = 0;
    int j = 0;
    int delete_range = 4;

    //Going through the image
    while(j < max_w)
    {
        while(i < max_h - delete_range)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            int search = 1;

            if(r == 255)
            {
                while(search <= delete_range)
                {
                    Uint32 next_pixel = get_pixel(image_surface, i+search, j);
                    Uint8 next_r, next_g, next_b;
                    SDL_GetRGB(next_pixel, image_surface->format, &next_r, &next_g, &next_b);
                    if(next_r != 255)
                    {
                        for(int k = 0; k <= search; k++)
                        {
                            Uint32 black_pixel = get_pixel(image_surface, i+k, j);
                            Uint8 r2, g2, b2;
                            SDL_GetRGB(black_pixel, image_surface->format, &r2, &g2, &b2);
                            black_pixel = SDL_MapRGB(image_surface->format, 0, 0, 0);
                            put_pixel(image_surface, i+k, j, black_pixel);
                        }
                    }
                    search++;
                }
                i += search + 1;
            }
            else
            {
                i++;
            }
        }
        j++;
    }
}



void rec_count_area(SDL_Surface* image_surface, int i, int j, int *count, int **test_mask)
{
    Uint8 r, g, b;
    if(i < image_surface->h - 1 && i >= 0 && j >= 0 && j <= image_surface->w -1)
    { 
        
        Uint32 pixel = get_pixel(image_surface, i+1, j);
        SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

        if(r == 255 && test_mask[i+1][j] != 1)
        {
            printf("i = %d   j = %d\n", i+1, j);
            (*count)++;
            test_mask[i+1][j] = 1;
            rec_count_area(image_surface, i+1, j, count, test_mask);
        }
    }

    if(i <= image_surface->h - 1 && i > 0 && j >= 0 && j <= image_surface->w -1)
    {
        
        Uint32 pixel = get_pixel(image_surface, i-1, j);
        SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
        if(r == 255 && test_mask[i-1][j] != 1)
        {
            printf("i = %d   j = %d\n", i-1, j);
            (*count)++;
            test_mask[i-1][j] = 1;
            rec_count_area(image_surface, i-1, j, count, test_mask);
        }
    }

    if(i <= image_surface->h - 1 && i >= 0 && j >= 0 && j < image_surface->w -1)
    {
        
        Uint32 pixel = get_pixel(image_surface, i, j+1);
        SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
        if(r == 255 && test_mask[i][j+1] != 1)
        {
            printf("i = %d   j = %d\n", i, j+1);
            (*count)++;
            test_mask[i][j+1] = 1;
            rec_count_area(image_surface, i, j+1, count, test_mask);
        }
    }

    if(i <= image_surface->h - 1 && i >= 0 && j > 0 && j <= image_surface->w -1)
    {
        
        Uint32 pixel = get_pixel(image_surface, i, j-1);
        SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
        if(r == 255 && test_mask[i][j-1] != 1)
        {
            printf("i = %d   j = %d\n", i, j-1);
            (*count)++;
            test_mask[i][j-1] = 1;
            rec_count_area(image_surface, i, j-1, count, test_mask);
        }
    }
}



void too_small_surface(SDL_Surface* image_surface)
{
    //creates the mask
    int **mask = NULL;
    
    //allow memory for h int* (array size h of arrays of int)
    mask = malloc(sizeof(int*) * image_surface->w); 

    
    for (int i = 0; i < image_surface->w; i++)
    {
        //allow memory for w int (array size w of int)
        mask[i] = malloc(sizeof(int) * image_surface->h);
    }
    printf("%d\n", image_surface->w);

    //Going through the image
    for(int i = 0; i < image_surface->w; i++)
    {
        for(int j = 0; j < image_surface->h; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            int count1 = 1;
            int *count = &count1;

            
            //creates the mask
            int **test_mask = NULL;
            
            //allow memory for h int* (array size h of arrays of int)
            test_mask = malloc(sizeof(int*) * image_surface->w); 

            
            for (int i = 0; i < image_surface->w; i++)
            {
                //allow memory for w int (array size w of int)
                test_mask[i] = malloc(sizeof(int) * image_surface->h);
            }

            for(int i = 0; i < image_surface->w; i++)
            {
                for(int j = 0; j < image_surface->h; j++)
                {
                    test_mask[i][j] = mask[i][j];
                }
            }
            if(r == 255 && test_mask[i][j] != 1)
            {   
                test_mask[i][j] = 1;
                rec_count_area(image_surface, i, j, count, test_mask);
                if(*count >= 100)
                {
                    for(int i = 0; i < image_surface->w; i++)
                    {
                        for(int j = 0; j < image_surface->h; j++)
                        {
                            mask[i][j] = test_mask[i][j];
                        }
                    }
                }
                
            }
            free(test_mask);
        }
    }

    for(int i = 0; i < image_surface->w; i++)
    {
        for(int j = 0; j < image_surface->h; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            Uint32 new_pixel = SDL_MapRGB(image_surface->format, mask[i][j] * r, mask[i][j] * g, mask[i][j] * b);
            put_pixel(image_surface, i, j, new_pixel);
        }
    }
    free(mask);
}
*/

void too_small_surface2(SDL_Surface* image_surface)
{
    int j = 0;
    for(int i = 0; i < image_surface->h-4; i += 4)
    {
        while(j < image_surface->w -25)
        {
            printf("i = %d    j = %d", i, j);
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            if (r == 255)
            {
                int a = i;
                int b = j;
                while (a < i+4 && r == 255)
                {
                    while (b < j+25 && r == 255)
                    {
                        Uint32 pixel = get_pixel(image_surface, a, b);
                        Uint8 r, g, b;
                        SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
                        b++;
                    }
                    a++;
                }
                if(a != i + 4 || b != j + 25)
                {
                    int a = i;
                    int b = j;
                    while (a < i+4)
                    {
                        while (b < j+25)
                        {
                            Uint32 pixel = get_pixel(image_surface, a, b);
                            
                            SDL_GetRGB(pixel, image_surface->format, 0, 0, 0);
                            b++;
                        }
                        a++;
                    }
                
                }
                j += 25;
            }
            j++;
            
        }
    }
}


int main()
{
    SDL_Surface* image_surface;
    SDL_Surface* screen_surface;

    init_sdl();

    image_surface = load_image("images/gaussien.png");
    
    screen_surface = display_image(image_surface);

    wait_for_keypressed();

    //=====================================
    //              FILTRES
    //=====================================

    grayscale(image_surface);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    filtre_gaussien(image_surface);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    contour_vertical(image_surface);

/*
    gradient_vertical(image_surface);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    binarisation(image_surface);
*/

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    fermeture_verticale(image_surface, 4);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    fermeture_horizontale(image_surface, 9);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    fermeture_verticale(image_surface, 2);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    too_small_surface2(image_surface);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    SDL_FreeSurface(image_surface);
    SDL_FreeSurface(screen_surface);

    return 0;
}
