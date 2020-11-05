#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


double init()	//fonction permettant d'initialiser mes poids de manière aléatoire
{
	double rnd;
	double a = 1.0;
	srand(time(NULL));
	for(int i = 0; i < 2; i++)
	{
		rnd = ((double)rand()/(double)(RAND_MAX)) * a;
	}
	return rnd;
}

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
/*
void init_weight()
{
	for(int input = 0; input < numInputs)
	{
		for(int hidden = 0; hidden < numHiddenNodes)
		{
			hiddenWeights[input][hidden] = init();
		}
	}
	for(int hidden = 0; hidden < numHiddenNodes)
	{
		for(int output = 0; output < numOutputs)
		{
			outputWeights[hidden][output] = init();
		}
	}
}

void init_bias()
{
	for(int hidden = 0; hidden < numHiddenNodes)
	{
		hiddenLayerBias[hidden];
	}
	for(int output = 0; output < numOutputs)
	{
		outputLayerBias[output];
	}
}*/




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
	

	static const int numTrainingSets = 4;

	double training_inputs[4][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};

	double training_outputs[4][1] = {{0.0f}, {1.0f}, {1.0f}, {0.0f}};

	static const int epochs = 10000; //nombre d'itération des tests

	static const double lr = 0.1f; //taux d'apprentissage
	/*init_weight();
	init_bias();*/
	for(int input = 0; input < numInputs; input++) //initialisation des poids
	{
		for(int hidden = 0; hidden < numHiddenNodes; hidden++)
		{
			hiddenWeights[input][hidden] = init();
		}
	}
	for(int hidden = 0; hidden < numHiddenNodes; hidden++)
	{
		for(int output = 0; output < numOutputs; output++)
		{
			outputWeights[hidden][output] = init();
		}
	}
	for(int hidden = 0; hidden < numHiddenNodes; hidden++) //initialisation des bias
	{
		hiddenLayerBias[hidden];
	}
	for(int output = 0; output < numOutputs; output++)
	{
		outputLayerBias[output];
	}
	//itère le training entier pour un certain nombre d'epochs
	for (int n = 0; n < epochs; n++)
	{
		int trainingSetOrder[] = {0,1,2,3}; //mélange l'ordre de l'ensemble d'entrainement
		if(n > 9900)
		{
			printf("epoch %d, ", n);
		}
		for (int x = 0; x < numTrainingSets; x++) //Parcourez chacun des élément de l'ensemble d'apprentissage
		{
			int i = trainingSetOrder[x];
			
			for (int j = 0; j < numHiddenNodes; j++) //calcul de l'activation de la couche caché
			{
	  			double activation = hiddenLayerBias[j];
  			
	    		for (int k = 0; k < numInputs; k++) 
	    		{
	    			if(n > 9900)
					{
						printf("%lf, ", training_inputs[i][k]);
					}
	      			activation += training_inputs[i][k] * hiddenWeights[k][j];
	      			printf("%lf\n", training_inputs[i][k]);
	    		}
	  			hiddenLayer[j] = sigmoid(activation);
	  			
			}
			
			for (int j = 0; j < numOutputs; j++) //calcul de l'activation de la couche de sortie
			{
				double activation = outputLayerBias[j];
				
				for (int k = 0; k < numHiddenNodes; k++) 
				{
					activation += hiddenLayer[k] * outputWeights[k][j];
				}
				
				outputLayer[j] = sigmoid(activation);
				printf(" %lf\n", outputLayer[j]);
			}
			
			double deltaOutput[numOutputs];
		
			for (int j = 0; j < numOutputs; j++) //calcul des modifications des poids de sortie
			{
				double dError = (training_outputs[i][j] - outputLayer[j]);
				deltaOutput[j] = dError * dSigmoid(outputLayer[j]);
				//printf("delta output: %lf\n", deltaOutput[j]);
			}
			
			double deltaHidden[numHiddenNodes];
			
			for (int j = 0; j < numHiddenNodes; j++) //calcul des modifications des poids des couches caché
			{
				double dError = 0.0f;
				
				for(int k = 0; k < numOutputs; k++) 
				{
					dError += deltaOutput[k] * outputWeights[j][k];
				}
			
				deltaHidden[j] = dError*dSigmoid(hiddenLayer[j]);
				//printf("delta hidden: %lf\n", deltaHidden[j]);
			}
		
			for (int j = 0; j < numOutputs; j++) //Appliaction des modifications des poids de sortie
			{
				outputLayerBias[j] += deltaOutput[j]*lr;
			
				for (int k = 0; k < numHiddenNodes; k++) 
				{
					outputWeights[k][j] += hiddenLayer[k] * deltaOutput[j]*lr;
				}
			}
		
			for (int j = 0; j < numHiddenNodes; j++) //Appliaction des modifications des poids des couches cachés
			{
				hiddenLayerBias[j] += deltaHidden[j] * lr;
				
				for(int k = 0; k < numInputs; k++) 
				{
					hiddenWeights[k][j] += training_inputs[i][k] * deltaHidden[j] * lr;
				}
			}
		}
	}
}







































