#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


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

void init_weight(int inputs,int hiddenNodes,int outputs,double hiddenWeights[][2],double outputWeights[][1])
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

void init_bias(int hiddenNodes, int outputs, double hiddenLayerBias[], double outputLayerBias[])
{
    for (int i = 0; i < hiddenNodes; ++i)
    {
        //hiddenLayerBias[i] = init();
        hiddenLayerBias[i] = 1.0f;
    }
    for (int i = 0; i < outputs; ++i)
    {
        //outputLayerBias[i] = init();
        outputLayerBias[i] = 1.0f;
    }
}

void hidden_activation(int numHiddenNodes, int numInputs, double hiddenLayerBias[], double training_inputs[], double hiddenWeights[][2], double hiddenLayer[])
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
	   		activation += training_inputs[k] * hiddenWeights[k][j];
	   		//printf("%lf\n", training_inputs[i][k]);
	   	}
		hiddenLayer[j] = sigmoid(activation);

	}
}

void output_activation(int numOutputs, int numHiddenNodes, double outputLayerBias[], double hiddenLayer[], double outputWeights[][1], double outputLayer[])
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

void modif_hidden_weight(int numHiddenNodes, int numOutputs, int numInputs, double hiddenLayer[], double outputWeights[][1], double hiddenLayerBias[], double hiddenWeights[][2], double training_inputs[], double deltaOutput[])
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
		hiddenLayerBias[j] += deltaHidden[j];
				
		for(int k = 0; k < numInputs; k++) 
		{
			hiddenWeights[k][j] += training_inputs[k] * deltaHidden[j];
		}
	}
	
}

void modif_output_weight(int numOutputs, int numHiddenNodes, double training_outputs[], double outputLayer[], double outputWeights[][1], double hiddenLayer[], double outputLayerBias[], double deltaOutput[])
{
		
	for (int j = 0; j < numOutputs; j++) //calcul des modifications des poids de sortie
	{
		double dError = (training_outputs[j] - outputLayer[j]);
		deltaOutput[j] = dError * dSigmoid(outputLayer[j]);
		//printf("delta output: %lf\n", deltaOutput[j]);
	}
	
	for (int j = 0; j < numOutputs; j++) //Appliaction des modifications des poids de sortie
	{
		outputLayerBias[j] += deltaOutput[j];
	
		for (int k = 0; k < numHiddenNodes; k++) 
		{
			outputWeights[k][j] += hiddenLayer[k] * deltaOutput[j];
		}
	}
}

int print_xor(int epoch, int print,double hiddenWeight[][2], double outputWeight[][1], double hiddenLayerBias[], double outputLayerBias[], double training_inputs[], double outputLayer[])
{
	if(epoch % 1000 == 0)
	{
		if(print == 1)
		{
			printf("epoch :%d\n", epoch);
			printf("weight input 1 :%f\n", hiddenWeight[0][0]);
			printf("weight input 2 :%f\n", hiddenWeight[0][1]);
			printf("weight input 3 :%f\n", hiddenWeight[1][0]);
			printf("weight input 4 :%f\n", hiddenWeight[1][1]);
			printf("weight output 1 :%f\n", outputWeight[0][0]);
			printf("weight output 2 :%f\n", outputWeight[0][1]);
			printf("hidden bias 1:%f\n", hiddenLayerBias[0]);
			printf("hidden bias 2:%f\n", hiddenLayerBias[1]);
			printf("output bias :%f\n\n\n", outputLayerBias[0]);
			print += 1;
		}
		printf("res(%f,%f) = %f\n\n\n", training_inputs[0], training_inputs[1], outputLayer[0]);
	}
	return print;
}

void training(int epochs, int numInputs, int numHiddenNodes, int numOutputs,double hiddenLayer[], double outputLayer[], double hiddenLayerBias[], double outputLayerBias[], double hiddenWeights[][2], double outputWeights[][1], int numTrainingSets, double training_inputs[][2], double training_outputs[][1], double deltaOutput[])
{

    for (int e = 0; e < epochs; ++e)
    {
    	int print = 1;
        for (int i = 0; i < numTrainingSets; ++i)
        {
            hidden_activation(numHiddenNodes, numInputs, hiddenLayerBias, training_inputs[i], hiddenWeights, hiddenLayer);
            output_activation(numOutputs, numHiddenNodes, outputLayerBias, hiddenLayer, outputWeights, outputLayer);
            modif_output_weight(numOutputs, numHiddenNodes, training_outputs[i], outputLayer, outputWeights, hiddenLayer, outputLayerBias, deltaOutput);
            modif_hidden_weight(numHiddenNodes, numOutputs, numInputs, hiddenLayer, outputWeights, hiddenLayerBias, hiddenWeights, training_inputs[i], deltaOutput);
            print = print_xor(e, print, hiddenWeights, outputWeights, hiddenLayerBias, outputLayerBias, training_inputs[i], outputLayer);
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

void main()
{

//========================================//
//                Variables               //
//========================================//

	int numInputs = 2;	//nombre d'entré
	int numHiddenNodes = 2;	//nombre de noeud caché
	int numOutputs = 1;	//nombre de sortie

	double hiddenLayer[numHiddenNodes]; //création des couches caché
	double outputLayer[numOutputs]; //création des couches de sortie
	
	double hiddenLayerBias[numHiddenNodes]; //biais des couches caché
	double outputLayerBias[numOutputs]; //bias des couches de sortie

	double hiddenWeights[numInputs][numHiddenNodes]; //poid des couches caché
	double outputWeights[numHiddenNodes][numOutputs]; //poid des couches de sortie
	
	double deltaOutput[numOutputs];

	int numTrainingSets = 4; //nombre de cas d'entrées

	double training_inputs[4][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};

	double training_outputs[4][1] = {{0.0f}, {1.0f}, {1.0f}, {0.0f}};

	int epochs = 10000; //nombre d'itération des tests

//========================================//
//                program                 //
//========================================//
    srand(time(NULL));
    init_weight(numInputs, numHiddenNodes, numOutputs, hiddenWeights, outputWeights);

    init_bias(numHiddenNodes, numOutputs, hiddenLayerBias, outputLayerBias);

    training(epochs, numInputs, numHiddenNodes, numOutputs, hiddenLayer, outputLayer, hiddenLayerBias, outputLayerBias, hiddenWeights, outputWeights, numTrainingSets, training_inputs, training_outputs, deltaOutput);
}







































