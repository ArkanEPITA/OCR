#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"
#include <math.h>
#include <errno.h>
#include <string.h>


//=============================
//          Filters
//=============================



void greyscale(SDL_Surface* image_surface)
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








void too_short_weight(SDL_Surface* image_surface, int max)
{
    int pos;
    if(max == 0)
    {
        max = 1;
    }
    for(int j = 0; j < (image_surface->h); j++)
    {
        for(int i = 0; i < (image_surface->w); i+=pos)
        {
            int max2 = max;
            if(i > (image_surface->w) - max)
            {
                max2 = (image_surface->w - i);
            }

            pos = 0;
            int delete = 0;
            while(pos < max2 && delete == 0)
            {
                Uint32 pixel = get_pixel(image_surface, i+pos, j);
                Uint8 r, g, b;
                SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

                if(r == 0)
                {
                    delete = 1;
                }
                pos++;
            }

            if(delete == 1 || pos < max)
            {
                for(int k = 0; k < pos; k++)
                {
                    Uint32 black_pixel = SDL_MapRGB(image_surface->format, 0, 0, 0);
                    put_pixel(image_surface, i+k, j, black_pixel);
                }
            }
            else
            {
                Uint32 next_pixel = get_pixel(image_surface, i+pos, j);
                Uint8 nr, ng, nb;
                SDL_GetRGB(next_pixel, image_surface->format, &nr, &ng, &nb);
                while(nr == 255)
                {
                    pos++;
                    next_pixel = get_pixel(image_surface, i+pos, j);
                    SDL_GetRGB(next_pixel, image_surface->format, &nr, &ng, &nb);
                }
            }
        }
    }
}





void too_short_height(SDL_Surface* image_surface, int max)
{
    int pos;
    if(max == 0)
    {
        max = 1;
    }
    for(int i = 0; i < (image_surface->w); i++)
    {
        for(int j = 0; j < (image_surface->h); j+=pos)
        {
            int max2 = max;
            if(j > (image_surface->h) - max)
            {
                max2 = (image_surface->h - j);
            }
            pos = 0;
            int delete = 0;
            while(pos < max2 && delete == 0)
            {
                Uint32 pixel = get_pixel(image_surface, i, j+pos);
                Uint8 r, g, b;
                SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

                if(r == 0)
                {
                    delete = 1;
                }
                pos++;
            }

            if(delete == 1 || pos < max)
            {
                for(int k = 0; k < pos; k++)
                {
                    Uint32 black_pixel = SDL_MapRGB(image_surface->format, 0, 0, 0);
                    put_pixel(image_surface, i, j+k, black_pixel);
                }
            }
            else
            {
                Uint32 next_pixel = get_pixel(image_surface, i, j+pos);
                Uint8 nr, ng, nb;
                SDL_GetRGB(next_pixel, image_surface->format, &nr, &ng, &nb);
                while(nr == 255)
                {
                    pos++;
                    next_pixel = get_pixel(image_surface, i, j+pos);
                    SDL_GetRGB(next_pixel, image_surface->format, &nr, &ng, &nb);
                }
            }
        }
    }
}






int RectangularityFactor(SDL_Surface* image_surface, int rect_left, int rect_right, int rect_up, int rect_down, int rectangularity_factor)
{
    int great = 0;
    int nb_pixel = 0;
    int nb_white_pixel = 0;
    for (int i = rect_left; i <= rect_right+1; i++)
    {
        for (int j = rect_up; j <= rect_down+1; j++)
        {
            //printf("(i = %d, j = %d)\n", i, j);
            nb_pixel += 1;
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            if(r == 255)
            {
                nb_white_pixel++;
            }
        }
    }
    //printf("rect_up = %d, rect_down = %d, rect_left = %d, rect_right = %d\n", rect_up, rect_down, rect_left, rect_right);
    //printf("nb_white_pixel = %d, nb_pixel = %d\n", nb_white_pixel, nb_pixel);
    double isRectagularityFactor = (double)nb_white_pixel/(double)nb_pixel;
    //printf("ratio = %f\n", isRectagularityFactor);
    if(isRectagularityFactor < rectangularity_factor || nb_white_pixel < 100)
    {
        for (int i = rect_left; i <= rect_right; i++)
        {
            for (int j = rect_up; j <= rect_down; j++)
            {
                Uint32 white_pixel = SDL_MapRGB(image_surface->format, 0, 0, 0);
                put_pixel(image_surface, i, j, white_pixel);
            }
        }
    }

    else
    {
        great = 1;
        for (int i = rect_left; i <= rect_right; i++)
        {
            for (int j = rect_up; j <= rect_down; j++)
            {
                Uint32 white_pixel = SDL_MapRGB(image_surface->format, 255, 255, 255);
                put_pixel(image_surface, i, j, white_pixel);
            }
        }
    }
    

   return great;
}








void RectangleCoordinates(SDL_Surface* image_surface, SDL_Surface* true_surface, Block *blocks, int jmin, int jmax, int i, int rectangularity_factor)
{

    /*
    int nb_carres = 0;
    SDL_Surface ** list = malloc(sizeof(SDL_Surface) * image_surface->h);
    for(int i = 0; i < image_surface->h; i++)
    {
        list[i] = NULL;
    }
    */
   int j = jmin;

    //printf("second fonction\n");
    int rect_left = -1;
    int rect_right = -1;
    int rect_up = -1;
    int rect_down = -1;
    int rectangle_begin = 0;
    int full_black = 1;

    while(i <= image_surface->w)
    {
        if ((full_black == 1 || i == image_surface->w-1) && rectangle_begin == 1)
        {
            //printf("l = %d, r = %d, u = %d, d = %d\n", rect_left, rect_right, rect_up, rect_down);
            if(RectangularityFactor(image_surface, rect_left, rect_right, rect_up, rect_down, rectangularity_factor) == 1)
            {
                
                if(j < image_surface->h - 5)
                {
                    rect_down += 5;
                }
                SDL_Surface* square = copy_image(true_surface, rect_up, rect_down, rect_left, rect_right);
                blocks->images[blocks->nb_block] = square;
                blocks->left[blocks->nb_block] = rect_left;
                blocks->up[blocks->nb_block] = rect_up;
                blocks->down[blocks->nb_block] = rect_down;
                blocks->nb_block += 1;

/*
                image_surface = square;
                printf("NEW SURFACE !!!\n");
                SDL_Surface* screen_surface = display_image(image_surface);
                update_surface(screen_surface, image_surface);
                wait_for_keypressed();


                list[nb_carres] = square;
*/
                //printf("nbcarres 2nd = %d\n", nb_carres);
            }
            rect_left = -1;
            rect_right = -1;
            rect_up = -1;
            rect_down = -1;
            rectangle_begin = 0;
        }

        full_black = 1;
        j = jmin;
        while(j <= jmax)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);


            if(r == 255)
            {
                full_black = 0;
                if(rectangle_begin == 0)
                {
                    rectangle_begin = 1;
                    rect_left = i;
                    rect_up = j;
                    //printf("initial left, up = %d, %d\n", i, j);
                }
                else
                {
                    if(i > rect_right)
                    {
                        rect_right = i;
                    }
                    
                    if(j > rect_down)
                    {
                        rect_down = j;
                    }
                    
                    else if(j < rect_up)
                    {
                        rect_up = j;
                    }
                }
            }
            j++;
        }
        i++;
    }
    /*
    if (full_black == 1 && rectangle_begin == 1)
    {
        nb_carres += RectangularityFactor(image_surface, rect_left, rect_right, rect_up, rect_down);
    }

    return list;
    */
}






void enveloppe_convexe(SDL_Surface* image_surface, SDL_Surface* true_surface, Block *blocks, int rectangularity_factor)
{
    int jmin = -1;
    int j = 0;
    /*
    SDL_Surface ** list = malloc(sizeof(SDL_Surface) * image_surface->h);
    SDL_Surface ** list2 = malloc(sizeof(SDL_Surface) * image_surface->h);
    for(int i = 0; i < image_surface->h; i++)
    {
        list[i] = NULL;
        list2[i] = NULL;
    }
    int nb_carres = 0;
    */

    while(j <= image_surface->h)
    {
        int i = 0;
        int found_jmin = 0;
        while(i <= image_surface->w && found_jmin == 0)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            if(r == 255)
            {
                if(jmin == -1)
                {
                    jmin = j;
                }
                found_jmin = 1;
            }            
            i++;
        }

        if(found_jmin == 0 || j == image_surface->h-1)
        {
            if(jmin != -1)
            {
                
                RectangleCoordinates(image_surface, true_surface, blocks, jmin, j+1, 0, rectangularity_factor);
                /*
                for(int k = 0; list2[k] != NULL; k++)
                {
                    printf("\nnbcarres + k = %d\n\n", nb_carres + k);
                    list[nb_carres] = list2[k];
                    nb_carres++;
                }

                free(list2);
                */

                jmin = -1;
            }
        }
        j++;
    }
    /*
    printf("nbcarres = %d\n", nb_carres);
    SDL_Surface ** final_list = malloc(sizeof(SDL_Surface) * nb_carres);
    for (int i = 0; i < nb_carres; i++)
    {
        final_list[i] = list[i];
    }

    free(list);

    return final_list;
    */
}



int LetterSize(SDL_Surface* image_surface)
{
    int begin_letter = -1;
    int end_letter = -1;
    int i = 0;
    while(i < image_surface->h-1 && end_letter == -1)
    {
        int white_pixel = 0;
        int j = 0;
        while(j < image_surface->w-1 && white_pixel == 0)
        {
            Uint32 pixel = get_pixel(image_surface, j, i);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);

            if(r == 255)
            {
                white_pixel = 1;
                if(begin_letter == -1)
                {
                    begin_letter = i;

                }
            }
            j++;
        }
        if(white_pixel == 0 && begin_letter != -1)
        {
            end_letter = i;
        }
        i++;
    }

    return end_letter - begin_letter;
}



char* final(SDL_Surface* image_surface, SDL_Surface* true_surface, char* s)
{
    int string_line = 0;

    int h = image_surface->h/100; // Barre de gauche
    int w = image_surface->w/100; // Barre du haut

    greyscale(true_surface);
    binarisation(true_surface);
    greyscale(image_surface);
    filtre_gaussien(image_surface);
    contour_vertical(image_surface);
    binarisation(image_surface);

/*
    int resize = LetterSize(image_surface)/15;

    printf("resize = %d\n", resize);

    image_surface = Resize(image_surface, image_surface->w/(2*resize), image_surface->h/(2*resize));
*/


    Block *blocks = NULL;
    blocks = malloc(sizeof(Block) * 1);
    blocks->images = malloc(sizeof(SDL_Surface) * image_surface->h);
    blocks->left = malloc(sizeof(int) * image_surface->h);
    blocks->up = malloc(sizeof(int) * image_surface->h);
    blocks->down = malloc(sizeof(int) * image_surface->h);

    for (int i = 0; i < image_surface->h; i++)
    {
        blocks->images[i] = NULL;
    }
    blocks->nb_block = 0;
     
    fermeture_verticale(image_surface, h);
    fermeture_horizontale(image_surface, w*3);
    fermeture_verticale(image_surface, h/2);
    too_short_weight(image_surface, w*4);

    too_short_height(image_surface, h/4);
    enveloppe_convexe(image_surface, true_surface, blocks, 0.35);           // A MODIFIER !!!!!!!!!!!!!!!!!!!!!!!!!

    int number_lines = blocks->nb_block;
    int begin = blocks->left[0];
    int back_space_up = blocks->up[0]; 
    int back_space_down = blocks->down[0];
    int begin_space = back_space_up;

    for(int i = 0; i < number_lines; i++)
    {
        if(begin > blocks->left[i])
        {
            begin = blocks->left[i];
        }
    }

    for(int i = 1; i < number_lines; i++)
    {
        back_space_up = blocks->up[i];
        if(begin_space > back_space_up - back_space_down)
        {
            begin_space = back_space_up - back_space_down;
        }
        back_space_down = blocks->down[i];
    }

    //for(int i = 0; i < number_lines-1; i++)
    for(int i = 13; i < 14; i++)
    {
        string_line = print_line(blocks->images[i], blocks->left[i], begin, s, string_line);

        if(i != number_lines-1)
        {
            s[string_line] = '\n';
            string_line++;
        }

        if(number_lines != 1 && i != number_lines-1)
        {
            if(i != 0)
            {
                int gap = blocks->up[i+1] - blocks->down[i];
                while (begin_space + 30 < gap)
                {
                    gap -= begin_space;
                    s[string_line] = '\n';
                    string_line++;
                }
            }
            else
            {
                int gap = blocks->up[1] - blocks->down[0];
                while (begin_space + 30 < gap)
                {
                    gap -= begin_space;
                    s[string_line] = '\n';
                    string_line++;
                }
            }
        }
        
    }

    free(image_surface);
    free(true_surface);

    return s;
}


/*
int main()
{
    SDL_Surface* image_surface;
    SDL_Surface* screen_surface;
    SDL_Surface* True_surface;

    char* s = malloc((sizeof(char))* 4095);
    strcat(s, "");
    
    char path[] = "images/algo.jpg";

    image_surface = load_image(path);
    True_surface = load_image(path);

    //image_surface = Resize(image_surface, 700, 600);
    //True_surface = Resize(True_surface, 700, 600);


    screen_surface = display_image(image_surface);

    wait_for_keypressed();

    //=====================================
    //              FILTRES
    //=====================================

    s = final(image_surface, True_surface, screen_surface);

    

    printf("%s", s);

    return 0;
}
*/