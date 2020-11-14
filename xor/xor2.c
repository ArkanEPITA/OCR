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

typedef struct Nodes Nodes;
struct Nodes
{
    double output()
    {
        return (sigmoid(weights[0] * inputs[0] + weights[1] * inputs[1] + biasWeight));
    }

    void adjustWeights()
    {
        N.weights[0] += N.error * N.inputs[0];
        N.weights[1] += N.error * N.inputs[1];
        N.biasWeight += N.error;
    }

    void randomizeWeights()
    {
        weights[0] = init();
        weights[1] = init();
        biasWeight = init();
    }

};





/*
void hidden_activation(Nodes* N, Training* T)
{
    for (int j = 0; j < (*N).numHiddenNodes; j++) //calcul de l'activation de la couche caché
    {
        double activation = (*N).hiddenLayerBias[j];
    
        for (int k = 0; k < (*N).numInputs; k++) 
        {
            activation += (*T).training_inputs[j][k] * (*N).hiddenWeights[k][j];
        }
        (*N).hiddenLayer[j] = sigmoid(activation);
        //printf("%f\n", sigmoid(activation));
    }
}

double output_activation(Nodes* N, int j)
{
    double activation = (*N).outputLayerBias[j];
    
    for (int k = 0; k < (*N).numHiddenNodes; k++) 
    {
        activation += (*N).hiddenLayer[k] * (*N).outputWeights[k][0];
    }
    printf("activation :%f\n", activation);
    return activation;
}

void hidden_error(Nodes* N, Training* T)
{
    for (int input = 0; input < (*N).numInputs ; ++input)
    {
        for (int hidden = 0; hidden < (*N).numHiddenNodes; ++hidden)
        {
            printf("avant : %f\n",(*N).hiddenWeights[input][hidden] );
            double error = (*N).hiddenLayer[hidden] - (*T).training_outputs[hidden][input];
            if (error > 0)
                (*N).hiddenWeights[input][hidden] -= (*N).lr * dSigmoid(error);
            else
                (*N).hiddenWeights[input][hidden] += (*N).lr * dSigmoid(error);
            printf("error : %f\n",error);
            printf("sig : %f\n",dSigmoid(error));
            printf("apres : %f\n\n",(*N).hiddenWeights[input][hidden] );
        }
    }
}

void output_error(Nodes* N, Training* T)
{
    
    for (int hidden = 0; hidden < (*N).numHiddenNodes; ++hidden)
    {
        printf("avant : %f\n",(*N).hiddenWeights[hidden][0] );
        double activation = (*N).outputLayerBias[hidden];
        for (int k = 0; k < (*N).numHiddenNodes; k++) 
        {
            activation += (*N).hiddenLayer[k] * (*N).outputWeights[k][0];
        }

        double error = (*N).hiddenLayer[hidden] - (*T).training_outputs[hidden][0];
        (*N).hiddenWeights[hidden][0] -= (*N).lr * error * activation;
        printf("error : %f\n",error);
        printf("sig : %f\n",dSigmoid(error));
        printf("apres : %f\n\n",(*N).hiddenWeights[hidden][0] );
    }
}

int print_xor(Training* T, Nodes* N, int epoch, int print)
{
    if(epoch % 1000 == 0)
    {
        if(print == 1)
        {
            printf("epoch :%d\n", epoch);
            printf("weight input 1 :%f\n", (*N).hiddenWeights[0][0]);
            printf("weight input 2 :%f\n", (*N).hiddenWeights[0][1]);
            printf("weight hidden 1 :%f\n", (*N).hiddenWeights[1][0]);
            printf("weight hidden 2 :%f\n", (*N).hiddenWeights[1][1]);
            printf("weight output 1 :%f\n", (*N).outputWeights[0][0]);
            //printf("weight output 2 :%f\n", (*N).outputWeights[0][1]);
            printf("hidden bias 1:%f\n", (*N).hiddenLayerBias[0]);
            printf("hidden bias 2:%f\n", (*N).hiddenLayerBias[1]);
            printf("output bias :%f\n\n\n", (*N).outputLayerBias[0]);
            print += 1;
        }
        printf("res(%f,%f) = %f\n\n\n", (*T).training_inputs[(*T).actTraining][0], (*T).training_inputs[(*T).actTraining][1], (*N).outputLayer[0]);
    }
    return print;
}
/*void training(Nodes* N, Training* T)
{

    for (int e = 0; e <= (*T).epochs; ++e)
    {
        int print = 1;
        for (int i = 0; i < (*T).numTrainingSets; ++i)
        {
            (*T).actTraining = i;
            hidden_activation(N, T);
            output_activation(N);
            modif_output_weight(N, T);
            modif_hidden_weight(N, T);
            print = print_xor(T, N, e, print);
        }
    }
}
*/

void main()
{

//========================================//
//                Variables               //
//========================================//

    Neuron hiddenNeuron1, hiddenNeuron2, outpuNeuron;
    // the input values
    double inputTraining[4][2] = 
    {
        { 0, 0},
        { 0, 1},
        { 1, 0},
        { 1, 1}
    };
    
    double results[4] = { 0, 1, 1, 0 };
    double epochs = 2000;
    
    
    hiddenNeuron1.weights[] = randomizeWeights();


//========================================//
//                program                 //
//========================================//
    srand(time(NULL));

    

}