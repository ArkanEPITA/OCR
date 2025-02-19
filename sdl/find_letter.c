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
    int weigth = 0;
    int first_black2 = 0;
    for(int i = 0; i < image_surface->w; i++)
    {
        int white = 0;
        for(int j = 0; j < image_surface->h; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            if (r == 0)
            {
                white = 1;
                first_black2 = 1;
            }
            if(white == 0 && first_black2 == 1)
            {
                first_black2 = 0;
                weigth += 1;
            }
        }
    }

    Word *Word = NULL;
    Word = malloc(sizeof(Word) * weigth*4);
   
    int letter = 0;
    int first_black = 0;
    for(int i = 0; i < image_surface->w; i++)
    {
        int white = 0;
        for(int j = 0; j < image_surface->h; j++)
        {
            Uint32 pixel = get_pixel(image_surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            if (r == 0)
            {
                int vmax = j;
                int vmin = j;
                int hmax = i;
                int hmin = i;

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
        if((white == 0 || i == image_surface->w -1) && first_black == 1)
        {
            letter++;
            first_black = 0;
        }
    }
    Array_word varialbe;
    varialbe.word = Word;
    varialbe.length = letter;
    varialbe.weigth = weigth;
    return varialbe;
}



int print_line(SDL_Surface* image_surface, int alinea, int begin, char* s, int string_line)
{
    Array_word Word = find_letters(image_surface);
    int nb_letters = Word.length;
    
    SDL_Surface* image2;
    SDL_Surface* image3;
    double* matrix_letter = malloc(sizeof(double) * 28 * 28);

    int jmaxref = Word.word[0].maxj_word;
    int jminref = Word.word[0].minj_word;
    int max_space = 0;
    int one_word = 0;
    int max_space_avant = 0;

    for (int i = 1; i < nb_letters; i++)
    {
        jminref = Word.word[i].minj_word;
        if(max_space < jminref - jmaxref)
        {
            max_space = jminref - jmaxref;
        }
        if((max_space_avant < max_space-4 || max_space_avant > max_space +4) && i != 1)
        {
            one_word = 1;
        }
        max_space_avant = jminref - jmaxref;
        jmaxref = Word.word[i].maxj_word;
    }

    if(nb_letters != 0)
    {
        if(one_word == 0)
        {
            while (alinea > begin + jmaxref - jminref + 20)
            {
                alinea = alinea - (jmaxref - jminref);
                s[string_line] = ' ';
                string_line++;
            }
        }
        else
        {
            while (alinea > begin + max_space + 20)
            {
                alinea = alinea - max_space;
                s[string_line] = ' ';
                string_line++;
            }
        }
    }

    int space = 0;

    int imax = Word.word[0].maxi_word;
    int imin = Word.word[0].mini_word;
    int jmax = Word.word[0].maxj_word;
    int jmin = Word.word[0].minj_word;


    
    image2 = copy_image(image_surface, imin, imax, jmin, jmax);

    image3 = Resize_letter(image2);

    matrix_letter = create_matrix_letter(image3);
    
    s[string_line] = run(matrix_letter);
    string_line++;

    for (int i = 1; i < nb_letters; i++)
    {
        jmin = Word.word[i].minj_word;
        space = jmin - jmax;        
        imax = Word.word[i].maxi_word;
        jmax = Word.word[i].maxj_word;
        imin = Word.word[i].mini_word;
        
        if(space-5 < max_space && space+5 > max_space && one_word == 1)
        {
            s[string_line] = ' ';
            string_line++;
        }
        
        image2 = copy_image(image_surface, imin, imax, jmin, jmax);

        image3 = Resize_letter(image2);
        
        matrix_letter = create_matrix_letter(image3);
       
        s[string_line] = run(matrix_letter);
        string_line++;
        
        free(image2);
        free(image3);   
    }
    free(matrix_letter);
    return string_line;
}