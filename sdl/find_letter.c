#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"
#include <math.h>
#include <errno.h>


//=============================
//          Filters
//=============================


Array_word find_letters(SDL_Surface* image_surface)
{
    Word *Word = NULL;
    Word = malloc(sizeof(Word) * image_surface->w);
   
    int letter = 0;
    int first_black = 0;
    printf("h = %d et %d\n", image_surface->h, image_surface->w);
    for(int i = 0; i < image_surface->w; i++)
    {
        int white = 0;
        for(int j = 0; j < image_surface->h; j++)
        {
            printf("i = %d  j = %d\n", i, j);
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            if (r == 0)
            {
                int vmax = j+1;
                int vmin = j-1;
                int hmax = i+1;
                int hmin = i-1;

                if (first_black == 0)
                {
                    Word[letter].maxi_word = vmax;
                    Word[letter].mini_word = vmin;
                    Word[letter].maxj_word = hmax;
                    Word[letter].minj_word = hmin;
                }
                white = 1;
                first_black = 1;

                if(vmax > Word[letter].maxi_word)
                {
                    Word[letter].maxi_word = vmax;
                }
                if(vmin < Word[letter].mini_word)
                {
                    Word[letter].mini_word = vmin;
                }
                if(hmax > Word[letter].maxj_word)
                {
                    Word[letter].maxj_word = hmax;
                }
                if(hmin < Word[letter].minj_word)
                {
                    Word[letter].minj_word = hmin;
                }
            }
        }
        if(white == 0 && first_black == 1)
        {
            letter++;
            first_black = 0;
        }
    }
    Array_word varialbe;
    varialbe.word = Word;
    varialbe.length = letter;
    return varialbe;
}



void print_line(SDL_Surface* image_surface)
{
    Array_word Word = find_letters(image_surface);
    int nb_letters = Word.length;
    Uint32 green = SDL_MapRGB(image_surface->format, 0, 255, 0);
    printf("letter = %d\n", nb_letters);
    
    for (int i = 0; i < nb_letters; i++)
    {
        printf("i = %d\n", i);
        int imax = Word.word[i].maxi_word;
        int imin = Word.word[i].mini_word;
        int jmax = Word.word[i].maxj_word;
        int jmin = Word.word[i].minj_word;
        printf("imax = %d\n", imax);
        printf("imin = %d\n", imin);
        printf("jmax = %d\n", jmax);
        printf("jmin = %d\n", jmin);

        for(int j = imin; j < imax; j++)
        {
            printf("j = %d\n", j);
            put_pixel(image_surface, jmin, j, green);
            put_pixel(image_surface, jmax, j, green);
        }
        for(int j = jmin; j < jmax; j++)
        {
            printf("j = %d\n", j);
            put_pixel(image_surface, j, imin, green);
            put_pixel(image_surface, j, imax, green);
        }
    }
}


/*
int main()
{
    SDL_Surface* image_surface;
    SDL_Surface* screen_surface;

    init_sdl();

    image_surface = load_image("images/phrase.png");
    
    screen_surface = display_image(image_surface);

    wait_for_keypressed();

    //=====================================
    //              FILTRES
    //=====================================

    find_letters(image_surface);
    print_line(image_surface);

    update_surface(screen_surface, image_surface);
    wait_for_keypressed();

    SDL_FreeSurface(image_surface);
    SDL_FreeSurface(screen_surface);

    return 0;
}
*/
