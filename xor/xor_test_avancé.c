#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>



//========================================//
//                Structures               //
//========================================//


typedef struct Nodes Nodes;
struct Nodes
{
    int numInputs;  //nombre d'entré
    int numHiddenNodes; //nombre de noeud caché
    int numOutputs; //nombre de sortie

    double hiddenLayer[2]; //création des couches caché
    double outputLayer[1]; //création des couches de sortie
    
    double hiddenLayerBias[2]; //biais des couches caché
    double outputLayerBias[1]; //bias des couches de sortie

    double hiddenWeights[2][2]; //poid des couches caché
    double outputWeights[1][1]; //poid des couches de sortie
    double deltaOutput[1];
};

typedef struct Training Training;
struct Training
{
    int numTrainingSets; //nombre de cas d'entrées

    double training_inputs[4][2];

    double training_outputs[4][1];
    
    int actTraining;

    int epochs; //nombre d'itération des numTrainingSets
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
    int negative = floor(rnd * 10000);
    if(negative % 2 == 0)rnd = -rnd;
    return rnd;
}

void init_weight(Nodes* N)
{
    for (int input = 0; input < (*N).numInputs ; ++input)
    {
        for (int hidden = 0; hidden < (*N).numHiddenNodes; ++hidden)
        {
            (*N).hiddenWeights[input][hidden] = init();
        }
    }
    for (int hidden = 0; hidden < (*N).numHiddenNodes; ++hidden)
    {
        for(int output = 0; output < (*N).numOutputs; ++output)
        {
            (*N).outputWeights[hidden][output] = init();
        }
    }
}

void init_bias(Nodes* N)
{
    for (int i = 0; i < (*N).numHiddenNodes; ++i)
    {
        (*N).hiddenLayerBias[i] = init();
        //(*N).hiddenLayerBias[i] = 1.0f;
    }
    for (int i = 0; i < (*N).numOutputs; ++i)
    {
        (*N).outputLayerBias[i] = init();
        //(*N).outputLayerBias[i] = 1.0f;
    }
}

void hidden_activation(Nodes* N, Training* T)
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
	   		activation += (*T).training_inputs[(*T).actTraining][k] * (*N).hiddenWeights[k][j];
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
	double deltaHidden[(*N).numHiddenNodes];
	
	for (int j = 0; j < (*N).numHiddenNodes; j++) //calcul des modifications des poids des couches caché
	{
		double dError;
		
		for(int k = 0; k < (*N).numOutputs; k++) 
		{
			dError = (*N).deltaOutput[k] * (*N).outputWeights[j][k];
			//dError = (*N).deltaOutput[k];
		}
			
		deltaHidden[j] = dError * dSigmoid((*N).hiddenLayer[j]);
		//printf("derror : %lf\n", (*N).outputWeights[j][0]);
		//printf("delta hidden: %lf\n", deltaHidden[j]);
	}
	for (int j = 0; j < (*N).numHiddenNodes; j++) //Appliaction des modifications des poids des couches cachés
	{
		//(*N).hiddenLayerBias[j] += deltaHidden[j];
				
		for(int k = 0; k < (*N).numInputs; k++) 
		{
			(*N).hiddenWeights[k][j] += (*T).training_inputs[(*T).actTraining][k] * deltaHidden[j];
			(*N).hiddenLayerBias[k] += (*T).training_inputs[(*T).actTraining][k] * deltaHidden[j];
		}
	}
	
}

void modif_output_weight(Nodes* N, Training* T)
{
		
	for (int j = 0; j < (*N).numOutputs; j++) //calcul des modifications des poids de sortie
	{
		double dError = ((*T).training_outputs[(*T).actTraining][j] - (*N).outputLayer[j]);
		(*N).deltaOutput[j] = dError * dSigmoid((*N).outputLayer[j]);
		//printf("delta output: %lf\n", (*N).deltaOutput[j]);
	}
	
	for (int j = 0; j < (*N).numOutputs; j++) //Appliaction des modifications des poids de sortie
	{
	
		for (int k = 0; k < (*N).numHiddenNodes; k++) 
		{
			(*N).outputWeights[k][j] += (*N).hiddenLayer[k] * (*N).deltaOutput[j];
			(*N).outputLayerBias[j] += (*N).hiddenLayer[k] * (*N).deltaOutput[j];
			//printf("delta output: %lf\n", (*N).hiddenLayer[k]);
		}
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

void training(Nodes* N, Training* T)
{

    for (int e = 0; e < (*T).epochs; ++e)
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

    Nodes N;
    Training T;
    
    N.numInputs = 2;
    N.numHiddenNodes = 2;
    N.numOutputs = 1;
    
    T.numTrainingSets = 4;
    T.training_inputs[0][0] = 0.0f;
    T.training_inputs[0][1] = 0.0f;
    T.training_inputs[1][0] = 1.0f;
    T.training_inputs[1][1] = 0.0f;
    T.training_inputs[2][0] = 0.0f;
    T.training_inputs[2][1] = 1.0f;
    T.training_inputs[3][0] = 1.0f;
    T.training_inputs[3][1] = 1.0f;
    
    T.training_outputs[0][0] = 0.0f;
    T.training_outputs[1][0] = 1.0f;
    T.training_outputs[2][0] = 1.0f;
    T.training_outputs[3][0] = 0.0f;
    T.epochs = 50000;
    T.actTraining = 0;

//========================================//
//                program                 //
//========================================//
    srand(time(NULL));
    init_weight(&N);

    init_bias(&N);

    training(&N, &T);
}




































