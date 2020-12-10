# ifndef NEURAL_NETWORK2_H_
# define NEURAL_NETWORK2_H_

# include <stdlib.h>
# include <stdio.h>
# include <err.h>
# include <time.h>
# include <math.h>

struct Neural_Network
{
    int nbInput;
    int nbHidden;
    int nbOutput;

    //Arrays
    double InputValue[784];
    double Goal[52];
    //Weight Arrays
    double WeightIH[784][64];
    double WeightHO[64][52];
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
    double dWeightIH[784][64];
    double dWeightHO[64][52];
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

double* matrixFromFile(char* filename);

double** lettersMatrix();

double sigmoid(double x);

double dSigmoid(double z);   //derivative of the activating sigmoid function

void SaveData(struct Neural_Network *net);

void LoadData(struct Neural_Network *net);

void free_array(struct Neural_Network *net);

void InitalizeValue(struct Neural_Network *net);

void ForwardPass(struct Neural_Network *net, int p, int epoch);

void BackwardPass(struct Neural_Network *net);

void OCR();

struct Neural_Network *InitalizeNetwork();

# endif