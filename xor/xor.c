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
    double outputWeights[2][1]; //poid des couches de sortie
    
    double deltaOutput[1];
    double deltaHidden[2];
    double Error;
    
    double lr;
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
	double res = 1 / (1 + exp(-z));
	return res;
}

double dSigmoid(double z)		//dériver de la fonction d'activation
{
	double res = (sigmoid(z)) * ( 1 - sigmoid(z));
	return res;
}

/*double init()   //fonction permettant d'initialiser mes poids de manière aléatoire
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
*/
double init()
{
	return (double) ((rand()%41)+30)/100;
}

void init_weight(Nodes* N)
{
    for (int input = 0; input < 2 ; ++input)
    {
        for (int hidden = 0; hidden < 2; ++hidden)
        {
            (*N).hiddenWeights[input][hidden] = init();
            //(*N).hiddenWeights[input][0] = 0.0;
        }
        
        (*N).outputWeights[input][0] = init();
    	//(*N).outputWeights[hidden][0] = 0.0;
    }
}

void init_bias(Nodes* N)
{
    for (int i = 0; i < 2; ++i)
    {
        (*N).hiddenLayerBias[i] = init();
        //(*N).hiddenLayerBias[i] = 1.0f;
    }
    
    (*N).outputLayerBias[0] = init();
    //(*N).outputLayerBias[0] = 1.0f;
}

void hidden_activation(Nodes* N, Training* T)
{
	for (int j = 0; j < 2; j++) //calcul de l'activation de la couche caché
	{
		double activation = (*N).hiddenLayerBias[j];
  	
		for (int k = 0; k < 2; k++) 
	   	{
	   		activation += (*T).training_inputs[(*T).actTraining][k] * (*N).hiddenWeights[k][j];
	   		
	   	}
		(*N).hiddenLayer[j] = sigmoid(activation);
	}
}

void output_activation(Nodes* N)
{
	double activation = (*N).outputLayerBias[0];
	
	for (int k = 0; k < 2; k++) 
	{
		activation += (*N).hiddenLayer[k] * (*N).outputWeights[k][0];
	}
				
	(*N).outputLayer[0] = sigmoid(activation);
	//printf("layer :%lf\n", (*N).hiddenLayer[j]);
}
/*
void modif_hidden_weight(Nodes* N, Training* T)
{
	double deltaHidden[(*N).numHiddenNodes];
	
	for (int j = 0; j < 2; j++) //calcul des modifications des poids des couches caché
	{
		double dError = 0.0f;
		
		dError += (*N).deltaOutput[0] * (*N).outputWeights[j][0];
		
		deltaHidden[j] = dError * dSigmoid((*N).hiddenLayer[j]);
		//printf("derror : %lf\n", deltaHidden[j]);
		//printf("delta hidden: %lf\n", deltaHidden[j]);
	}
	for (int j = 0; j < 2; j++) //Appliaction des modifications des poids des couches cachés
	{
		//(*N).hiddenLayerBias[j] += deltaHidden[j] * (*N).lr;
		(*N).hiddenLayerBias[j] += deltaHidden[j];
				
		for(int k = 0; k < 2; k++) 
		{
			//(*N).hiddenWeights[k][j] += (*T).training_inputs[(*T).actTraining][k] * deltaHidden[j] * (*N).lr;
			(*N).hiddenWeights[k][j] += (*T).training_inputs[(*T).actTraining][k] * deltaHidden[j];
		}
	}
	
}
/*Je pense que l'erreur est la au dessus*/
/*
void modif_output_weight(Nodes* N, Training* T)
{
		
	double dError = ((*T).training_outputs[(*T).actTraining][0] - (*N).outputLayer[0]);
	(*N).Error = dError;
	(*N).deltaOutput[0] = dError * dSigmoid((*N).outputLayer[0]);
	//printf("delta output: %lf\n", (*N).deltaOutput[j]);
		
	//(*N).outputLayerBias[j] += (*N).deltaOutput[j] * (*N).lr;
	(*N).outputLayerBias[0] += (*N).deltaOutput[0];
	for (int k = 0; k < 2; k++) 
	{
		//(*N).outputWeights[k][j] += (*N).hiddenLayer[k] * (*N).deltaOutput[j] * (*N).lr;
		(*N).outputWeights[k][0] += (*N).hiddenLayer[k] * (*N).deltaOutput[0];
		//printf("delta output: %lf\n", (*N).hiddenLayer[k] * (*N).deltaOutput[j]);
	}
}
*/

void change_hidden_weight(Nodes* N)
{
	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			(*N).hiddenWeights[i][j] += (*N).deltaHidden[i];
		}
		
		(*N).hiddenLayerBias[i] += (*N).deltaHidden[i];
	}
	
}

void change_output_weight(Nodes* N)
{
	for(int i = 0; i < 2; i++)
	{
		(*N).outputWeights[i][0] += (*N).deltaOutput[0];
	}
	(*N).outputLayerBias[0] += (*N).deltaOutput[0];
}

void calcul_erreur_changement_poids(Nodes* N, Training* T)
{
	(*N).deltaOutput[0] = dSigmoid((*N).outputLayer[0]) * ((*T).training_outputs[(*T).actTraining][0] - (*N).outputLayer[0]);
	change_output_weight(N);
	for(int i = 0; i < 2; i++)
	{
		(*N).deltaHidden[i] = dSigmoid((*N).hiddenLayer[0]) * (*N).deltaOutput[0] * (*N).outputWeights[i][0];
	}
	change_hidden_weight(N);
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
			printf("weight output 2 :%f\n", (*N).outputWeights[0][1]);
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
/*
    for (int e = 0; e <= (*T).epochs; ++e)
    {
    	int print = 1;
        for (int i = 0; i < 4; ++i)
        {
        	(*T).actTraining = i;
            hidden_activation(N, T);
            output_activation(N);
            modif_output_weight(N, T);
            modif_hidden_weight(N, T);
            print = print_xor(T, N, e, print);
        }
    }
*/
	int e = 0;
	while(fabs((*N).Error) > 0.01)
	{
		int print = 1;
        for (int i = 0; i < 4; ++i)
        {
        	(*T).actTraining = i;
            hidden_activation(N, T);
            output_activation(N);
            //modif_output_weight(N, T);
            //modif_hidden_weight(N, T);
            calcul_erreur_changement_poids(N, T);
            print = print_xor(T, N, e, print);
        }
        e += 1;
	}
	printf("nombre d'iteration : %d\n\n", e);
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
    N.lr = 0.1;
    N.Error = 0.5;
    
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
    T.epochs = 200000;
    T.actTraining = 0;

//========================================//
//                program                 //
//========================================//
    srand(time(NULL));
    
    init_weight(&N);

    init_bias(&N);

    training(&N, &T);
}
