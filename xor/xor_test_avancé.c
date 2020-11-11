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

void hidden_activation()
{

}

void output_activation()
{

}

void modif_hidden_weight()
{

}

void modif_output_weight()
{

}

void modif_hidden_bias()
{

}

void modif_output_bias()
{

}






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
	

	int numTrainingSets = 4; //nombre de cas d'entrées

	double training_inputs[4][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};

	double training_outputs[4][1] = {{0.0f}, {1.0f}, {1.0f}, {0.0f}};

	int epochs = 10000; //nombre d'itération des tests

//========================================//
//                program                 //
//========================================//
    srand(time(NULL));
    init_weight(inputs,hiddenNodes,outputs,hiddenWeights,outputWeights);

    init_bias(hiddenNodes, ouputs, hiddenLayerBias, outputLayerBias);

    training(epochs, numInputs, numHiddenNodes, numOutputs, hiddenLayer, outputLayer, hiddenLayerBias, outputLayerBias, hiddenWeights, outputWeights, numTrainingSets, training_inputs, training_outputs);
}







































