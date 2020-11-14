#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "xor_test.h"



//========================================//
//                Structures               //
//========================================//


typedef struct Nodes Nodes;
struct Nodes
{
    int numInputs = 2;  //nombre d'entré
    int numHiddenNodes = 2; //nombre de noeud caché
    int numOutputs = 1; //nombre de sortie

    double hiddenLayer[numHiddenNodes]; //création des couches caché
    double outputLayer[numOutputs]; //création des couches de sortie
    
    double hiddenLayerBias[numHiddenNodes]; //biais des couches caché
    double outputLayerBias[numOutputs]; //bias des couches de sortie

    double hiddenWeights[numInputs][numHiddenNodes]; //poid des couches caché
    double outputWeights[numHiddenNodes][numOutputs]; //poid des couches de sortie
    double deltaOutput[numOutputs];
};

typedef struct Training Training;
struct Training
{
    int numTrainingSets = 4; //nombre de cas d'entrées

    double training_inputs[4][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};

    double training_outputs[4][1] = {{0.0f}, {1.0f}, {1.0f}, {0.0f}};

    int epochs = 10000; //nombre d'itération des numTrainingSets
};


//========================================//
//                Fonctions               //
//========================================//

double sigmoid(double z)	//fonction d'activation de notre réseau de neurone
{
	double res = 1 / (1 + exp(z));
	return res;
}

double dSigmoid(double z)		//dériver de la fonction d'activation
{
	double res = (sigmoid(z)) + ( 1 - sigmoid(z));
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
    return rnd;
}

void init_weight(Nodes* N)
{
    for (int input = 0; input < (*N).inputs ; ++input)
    {
        for (int hidden = 0; hidden < (*N).hiddenNodes; ++hidden)
        {
            (*N).hiddenWeights[input][hidden] = init();
        }
    }
    for (int hidden = 0; hidden < (*N).hiddenNodes; ++hidden)
    {
        for(int output = 0; output < (*N).outputs; ++output)
        {
            (*N).outputWeights[hidden][output] = init();
        }
    }
}

void init_bias(Nodes* N)
{
    for (int i = 0; i < (*N).hiddenNodes; ++i)
    {
        //hiddenLayerBias[i] = init();
        (*N).hiddenLayerBias[i] = 1.0f;
    }
    for (int i = 0; i < (*N).outputs; ++i)
    {
        //outputLayerBias[i] = init();
        (*N).outputLayerBias[i] = 1.0f;
    }
}

void hidden_activation(Nodes* N)
{
	for (int j = 0; j < (*N).numHiddenNodes; j++) //calcul de l'activation de la couche caché
	{
		double activation = (*N).hiddenLayerBias[j];
  	
		for (int k = 0; k < (*N).numInputs; k++) 
	   	{
	   		/*if(n > 9900)
			{
				printf("%lf, ", training_inputs[i][k]);
			}*/
	   		activation += (*N).training_inputs[k] * (*N).hiddenWeights[k][j];
	   		//printf("%lf\n", training_inputs[i][k]);
	   	}
		(*N).hiddenLayer[j] = sigmoid(activation);

	}
}

void output_activation(Nodes* N)
{
	for (int j = 0; j < (*N).numOutputs; j++) //calcul de l'activation de la couche de sortie
	{
		double activation = (*N).outputLayerBias[j];
		
		for (int k = 0; k < (*N).numHiddenNodes; k++) 
		{
			activation += (*N).hiddenLayer[k] * (*N).outputWeights[k][j];
		}
				
		(*N).outputLayer[j] = sigmoid(activation);
		//printf(" %lf\n", outputLayer[j]);
	}
}

void modif_hidden_weight(Nodes* N, Training* T)
{
	double deltaHidden[numHiddenNodes];
	
	for (int j = 0; j < (*N).numHiddenNodes; j++) //calcul des modifications des poids des couches caché
	{
		double dError = 0.0f;
		
		for(int k = 0; k < (*N).numOutputs; k++) 
		{
			dError += (*N).deltaOutput[k] * (*N).outputWeights[j][k];
		}
			
		(*N).deltaHidden[j] = dError * dSigmoid((*N).hiddenLayer[j]);
		//printf("delta hidden: %lf\n", deltaHidden[j]);
	}
	for (int j = 0; j < (*N).numHiddenNodes; j++) //Appliaction des modifications des poids des couches cachés
	{
		(*N).hiddenLayerBias[j] += deltaHidden[j];
				
		for(int k = 0; k < (*N).numInputs; k++) 
		{
			(*N).hiddenWeights[k][j] += (*T).training_inputs[k] * deltaHidden[j];
		}
	}
	
}

void modif_output_weight(Nodes* N, Training )
{
		
	for (int j = 0; j < (*N).numOutputs; j++) //calcul des modifications des poids de sortie
	{
		double dError = ((*T).training_outputs[j] - (*N).outputLayer[j]);
		(*N).deltaOutput[j] = dError * dSigmoid((*N).outputLayer[j]);
		//printf("delta output: %lf\n", deltaOutput[j]);
	}
	
	for (int j = 0; j < (*N).numOutputs; j++) //Appliaction des modifications des poids de sortie
	{
	
		for (int k = 0; k < (*N).numHiddenNodes; k++) 
		{
			(*N).outputWeights[k][j] += (*N).hiddenLayer[k] * (*N).deltaOutput[j];
		}
	}
}

int print_xor(Traning* T, Nodes* N)
{
	if(epoch % 1000 == 0)
	{
		if(print == 1)
		{
			printf("epoch :%d\n", epoch);
			printf("weight input 1 :%f\n", (*N).hiddenWeight[0][0]);
			printf("weight input 2 :%f\n", (*N).hiddenWeight[0][1]);
			printf("weight input 3 :%f\n", (*N).hiddenWeight[1][0]);
			printf("weight input 4 :%f\n", (*N).hiddenWeight[1][1]);
			printf("weight output 1 :%f\n", (*N).outputWeight[0][0]);
			printf("weight output 2 :%f\n", (*N).outputWeight[0][1]);
			printf("hidden bias 1:%f\n", (*N).hiddenLayerBias[0]);
			printf("hidden bias 2:%f\n", (*N).hiddenLayerBias[1]);
			printf("output bias :%f\n\n\n", (*N).outputLayerBias[0]);
			print += 1;
		}
		printf("res(%f,%f) = %f\n\n\n", (*T).training_inputs[0], (*T).training_inputs[1], (*N).outputLayer[0]);
	}
	return print;
}

void training(Nodes* N, Traning* T)
{

    for (int e = 0; e < (*T).epochs; ++e)
    {
    	int print = 1;
        for (int i = 0; i < (*T).numTrainingSets; ++i)
        {
            hidden_activation(N);
            output_activation(N);
            modif_output_weight(N, T);
            modif_hidden_weight(N, T);
            print = print_xor(N, T);
        }
    }
}

/*
void modif_hidden_bias()
{
	
}

void modif_output_bias()
{

}
*/



void main(int argc, char *argv[])
{

//========================================//
//                Variables               //
//========================================//

    Nodes N;
    Training T;

//========================================//
//                program                 //
//========================================//
    srand(time(NULL));
    init_weight(&N);

    init_bias(&N);

    training(&N, &T);
}




































