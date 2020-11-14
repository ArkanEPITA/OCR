#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


double sigmoid(double z)    //fonction d'activation de notre réseau de neurone
{
    double res = 1 / (1 + exp(-z));
    return res;
}

double dSigmoid(double z)       //dériver de la fonction d'activation
{
    double res = z * (1 - z);
    return res;
}



double init()   //fonction permettant d'initialiser mes poids de manière aléatoire
{
    double rnd;
    double a = 1.0;
    for(int i = 0; i < 2; i++)
    {
        rnd = ((double)rand()/(double)(RAND_MAX)) * a;
    }
    int negative = floor(rnd * 10000);
    if(negative % 2 == 0)rnd = -rnd;
    return rnd;
}



//=========================================//
//                Structures               //
//=========================================//
typedef struct Neuron Neuron;
struct Neuron
{
   double inputs[2];
   double weights[2];
   double error;
   double biasWeight;
};

double output(Neuron* hNeuron)
{
    return (sigmoid(hNeuron->weights[0] * hNeuron->inputs[0] + hNeuron->weights[1] * hNeuron->inputs[1] + hNeuron->biasWeight));
}

void adjustWeights(Neuron* hNeuron)
{
    hNeuron->weights[0] += hNeuron->error * hNeuron->inputs[0];
    hNeuron->weights[1] += hNeuron->error * hNeuron->inputs[1];
    hNeuron->biasWeight += hNeuron->error;
}

void randomizeWeights(Neuron* hNeuron)
{
    hNeuron->weights[0] = init();
    hNeuron->weights[1] = init();
    hNeuron->biasWeight = init();
}

void training(double epochs, Neuron* hneuron1,Neuron* hneuron2,Neuron* neuron, double inputTraining[][2], double results[4])
{
    for (double j = 0; j < epochs; ++j)
    {
        for (int i = 0; i < 4; ++i)
        {
            //forward propagation (calculates output)
            hneuron1->inputs[0] = inputTraining[i][0];
            hneuron1->inputs[1] = inputTraining[i][1];
            hneuron2->inputs[0] = inputTraining[i][0];
            hneuron2->inputs[1] = inputTraining[i][1];
            neuron->inputs[0] = output(hneuron1);
            neuron->inputs[1] = output(hneuron2);

            printf("%f xor %f = %f\n",inputTraining[i][0],inputTraining[i][1], output(neuron));

            //back propagation (adjusts weights)

            // adjusts the weight of the output neuron, based on its error
            neuron->error = dSigmoid(output(neuron)) * (results[i] - output(neuron));
            adjustWeights(neuron);
            // then adjusts the hidden neurons' weights, based on their errors
            hneuron1->error = dSigmoid(output(hneuron1)) * neuron->error * neuron->weights[0];
            hneuron2->error = dSigmoid(output(hneuron2)) * neuron->error * neuron->weights[1];

            adjustWeights(hneuron1);
            adjustWeights(hneuron2);
        }
    }
}


void main()
{

//========================================//
//                Variables               //
//========================================//

    Neuron hiddenNeuron1, hiddenNeuron2, outputNeuron;
    // the input values

    
    double results[4] = { 0, 1, 1, 0 };
    double epochs = 2000;
    
    double inputTraining[4][2] = 
    {
        { 0.0f, 0.0f},
        { 0.0f, 1.0f},
        { 1.0f, 0.0f},
        { 1.0f, 1.0f}
    };
    


//========================================//
//                program                 //
//========================================//
    srand(time(NULL));

    randomizeWeights(&hiddenNeuron1);
    randomizeWeights(&hiddenNeuron2);
    randomizeWeights(&outputNeuron);
    training(epochs,&hiddenNeuron1,&hiddenNeuron2,&outputNeuron,inputTraining,results);

}