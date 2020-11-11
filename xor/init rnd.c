#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double init()   //fonction permettant d'initialiser mes poids de manière aléatoire
{
    double rnd;
    double a = 1.0;
    for(int i = 0; i < 2; i++)
    {
        rnd = ((double)rand()/(double)(RAND_MAX)) * a;
    }
    printf("%lf\n",rnd);
    return rnd;
}

void main()
{
    srand(time(NULL));
    init();
    init();
}