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

}

void init_weight()
{

}

void init_bias()
{

}

void training()
{

}

void calcul_error()
{

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
	static const int numInputs = 2;	//nombre d'entré
	static const int numHiddenNodes = 2;	//nombre de noeud caché
	static const int numOutputs = 1;	//nombre de sortie

	double hiddenLayer[numHiddenNodes]; //création des couches caché
	double outputLayer[numOutputs]; //création des couches de sortie
	
	double hiddenLayerBias[numHiddenNodes]; //biais des couches caché
	double outputLayerBias[numOutputs]; //bias des couches de sortie

	double hiddenWeights[numInputs][numHiddenNodes]; //poid des couches caché
	double outputWeights[numHiddenNodes][numOutputs]; //poid des couches de sortie
	

	static const int numTrainingSets = 4; //nombre de cas d'entrées

	double training_inputs[4][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};

	double training_outputs[4][1] = {{0.0f}, {1.0f}, {1.0f}, {0.0f}};

	static const int epochs = 10000; //nombre d'itération des tests

	static const double lr = 0.1f; //taux d'apprentissage
}







































