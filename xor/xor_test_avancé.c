#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "xor_test.h"

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

void init_weight(int inputs,int hiddenNodes,int outputs,double hiddenWeights[][],double outputWeights[][])
{
    for (int input = 0; input < inputs ; ++input)
    {
        for (int hidden = 0; hidden < hiddenNodes; ++hidden)
        {
            hiddenWeights[input][hidden] = init();
        }
    }
    for (int hidden = 0; hidden < hiddenNodes; ++hidden)
    {
        for(int output = 0; output < outputs; ++output)
        {
            outputWeights[hidden][output] = init();
        }
    }
}

void init_bias(int hiddenNodes, int ouputs, double hiddenLayerBias[], double outputLayerBias[])
{
    for (int i = 0; i < hiddenNodes; ++i)
    {
        //hiddenLayerBias[i] = init();
        hiddenLayerBias = 1;
    }
    for (int i = 0; i < outputs; ++i)
    {
        //outputLayerBias[i] = init();
        outputLayerBias = 1;
    }
}

void training(int epochs, int numInputs, int numHiddenNodes, int numOutputs,double hiddenLayer[], double outputLayer[], double hiddenLayerBias[], double outputLayerBias[], double hiddenWeights[][], double outputWeights[][], int numTrainingSets, double training_inputs[][], double training_outputs[][])
{

    for (int e = 0; e < epochs; ++e)
    {
        if(e < 10 || e > 9990)
        {
            printf("epoch %d, ", e);
        }

        for (int i = 0; i < numTrainingSets; ++i)
        {
            hidden_activation(numHiddenNodes, numInputs, hiddenLayerBias, training_inputs[i], hiddenWeights, hiddenLayer);
            output_activation(numOutputs, numHiddenNodes, outputLayerBias, hiddenLayer, outputWeights, outputLayer);
            modif_hidden_weight(numHiddenNodes, numOutputs, numInputs, hiddenLayer, outputWeights, hiddenLayerBias, training_inputs[i]);
            modif_output_weight(numOutputs, numHiddenNodes, training_outputs[i], outputLayer, outputWeights, hiddenLayer);
        }
    }
}

void hidden_activation(int numHiddenNodes, int numInputs, double hiddenLayerBias[], double training_inputs[], double hiddenWeights[], double hiddenLayer[])
{
	for (int j = 0; j < numHiddenNodes; j++) //calcul de l'activation de la couche caché
	{
		double activation = hiddenLayerBias[j];
  	
		for (int k = 0; k < numInputs; k++) 
	   	{
	   		/*if(n > 9900)
			{
				printf("%lf, ", training_inputs[i][k]);
			}*/
	   		activation += training_inputs[i][k] * hiddenWeights[k][j];
	   		//printf("%lf\n", training_inputs[i][k]);
	   	}
		hiddenLayer[j] = sigmoid(activation);

	}
}

void output_activation(int numOutputs, int numHiddenNodes, double outputLayerBias[], double hiddenLayer[], double outputWeights[], double outputLayer[])
{
	for (int j = 0; j < numOutputs; j++) //calcul de l'activation de la couche de sortie
	{
		double activation = outputLayerBias[j];
		
		for (int k = 0; k < numHiddenNodes; k++) 
		{
			activation += hiddenLayer[k] * outputWeights[k][j];
		}
				
		outputLayer[j] = sigmoid(activation);
		//printf(" %lf\n", outputLayer[j]);
	}
}

void modif_hidden_weight(int numHiddenNodes, int numOutputs, int numInputs, double hiddenLayer[], double outputWeights[][], double hiddenLayerBias[],double hiddenWeights[][],double training_inputs[])
{
	double deltaHidden[numHiddenNodes];
	
	for (int j = 0; j < numHiddenNodes; j++) //calcul des modifications des poids des couches caché
	{
		double dError = 0.0f;
		
		for(int k = 0; k < numOutputs; k++) 
		{
			dError += deltaOutput[k] * outputWeights[j][k];
		}
			
		deltaHidden[j] = dError * dSigmoid(hiddenLayer[j]);
		//printf("delta hidden: %lf\n", deltaHidden[j]);
	}
	for (int j = 0; j < numHiddenNodes; j++) //Appliaction des modifications des poids des couches cachés
	{
		hiddenLayerBias[j] += deltaHidden[j] * lr;
				
		for(int k = 0; k < numInputs; k++) 
		{
			hiddenWeights[k][j] += training_inputs[i][k] * deltaHidden[j];
		}
	}
	
}

void modif_output_weight(int numOutputs, int numHiddenNodes, double training_outputs[], double outputLayer[], double outputWeights[][], double hiddenLayer[])
{
	double deltaOutput[numOutputs];
		
	for (int j = 0; j < numOutputs; j++) //calcul des modifications des poids de sortie
	{
		double dError = (training_outputs[j] - outputLayer[j]);
		deltaOutput[j] = dError * dSigmoid(outputLayer[j]);
		//printf("delta output: %lf\n", deltaOutput[j]);
	}
	
	for (int j = 0; j < numOutputs; j++) //Appliaction des modifications des poids de sortie
	{
		outputLayerBias[j] += deltaOutput[j]*lr;
	
		for (int k = 0; k < numHiddenNodes; k++) 
		{
			outputWeights[k][j] += hiddenLayer[k] * deltaOutput[j];
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








































