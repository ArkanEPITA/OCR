#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


double sigmoid(double z)    //fonction sigmoïde d'activation de notre réseau de neurone
{
    double res = 1 / (1 + exp(-z));
    return res;
}

double dSigmoid(double z)   //dériver de la fonction sigmoïde d'activation
{
    double res = z * (1 - z);
    return res;
}

double init()   //fonction permettant d'initialiser mes poids de manière aléatoire
{
    double rnd;
    double a = 1.0;
    for(int i = 0; i < 2; i++)	//permet d'obtenir une valeur avec une variation plus importante pour i = 1 que pour 1 = 0
    {
        rnd = ((double)rand()/(double)(RAND_MAX)) * a;	//permet de recuperer une valeur aléatoire comprise entre 0 et 1
    }
    return rnd;
}


//=========================================//
//                Structures               //
//=========================================//
typedef struct Neuron Neuron; 	//structure de nos neurones 
struct Neuron
{
   double inputs[2];	//les deux entrées de notre neurone
   double weights[2];	//les poids des liens entre les neurones d'entrés et le neurone actuel
   double error;		//l'erreur de notre neurone
   double biasWeight;	//le biais de notre neurone
};


//========================================//
//                fonctions               //
//========================================//

double output(Neuron* hNeuron)	//calcule le fonction d'activation d'un neurone grace à la sigmoïde
{
    return (sigmoid(hNeuron->weights[0] * hNeuron->inputs[0] + hNeuron->weights[1] * hNeuron->inputs[1] + hNeuron->biasWeight));
}

void randomizeWeights(Neuron* hNeuron)	//initilse les poids d'un neurone de manière aléatoire grace à "init()"
{										//initialise le biais de manière aléatoire
    hNeuron->weights[0] = init();
    hNeuron->weights[1] = init();
    hNeuron->biasWeight = init();
}

void adjustWeights(Neuron* hNeuron)	//actualise les poids d'un neurone par rapport à l'entrées et l'erreur
{									//actualise le biais d'un neurone par rapport à l'erreur
    hNeuron->weights[0] += hNeuron->error * hNeuron->inputs[0];
    hNeuron->weights[1] += hNeuron->error * hNeuron->inputs[1];
    hNeuron->biasWeight += hNeuron->error;
}

void training(double epochs, Neuron* hneuron1,Neuron* hneuron2,Neuron* neuron, double inputTraining[][2], double results[4])
{
    for (double j = 0; j < epochs; ++j)
    {
        for (int i = 0; i < 4; ++i)
        {
            //forward propagation -> calcule l'output du neurone de la couche de sortie par rapport aux valeurs d'entrées
            hneuron1->inputs[0] = inputTraining[i][0];
            hneuron1->inputs[1] = inputTraining[i][1];
            hneuron2->inputs[0] = inputTraining[i][0];
            hneuron2->inputs[1] = inputTraining[i][1];
            neuron->inputs[0] = output(hneuron1);
            neuron->inputs[1] = output(hneuron2);

            printf("%f xor %f = %f\n",inputTraining[i][0],inputTraining[i][1], output(neuron));

            //back propagation -> permet d'ajuster les valeurs de poids et de biais en fonction de l'erreur

            // ajuste le poids du neurone de sortie, en fonction de son erreur en utilisant la dérivée de sigmoïde
            neuron->error = dSigmoid(output(neuron)) * (results[i] - output(neuron));
            adjustWeights(neuron);
            // ajuste le poids des neurones cachés, en fonction de leurs erreurs en utilisant la dérivée de sigmoïde
            hneuron1->error = dSigmoid(output(hneuron1)) * neuron->error * neuron->weights[0];
            hneuron2->error = dSigmoid(output(hneuron2)) * neuron->error * neuron->weights[1];

            adjustWeights(hneuron1);
            adjustWeights(hneuron2);
        }
        printf("\n______________________________\n\n");
    }
}


int main()
{

//========================================//
//                Variables               //
//========================================//

    Neuron hiddenNeuron1, hiddenNeuron2, outputNeuron;	//création de trois variables de la structure Neuron

    double inputTraining[4][2] = 		//défini les valeurs d'entrées possibles
    {
        { 0.0f, 0.0f},
        { 0.0f, 1.0f},
        { 1.0f, 0.0f},
        { 1.0f, 1.0f}
    };

    double results[4] = { 0, 1, 1, 0 };	//les différents résultats attendus par rapport à inputTraining
    double epochs = 2000;



//========================================//
//                program                 //
//========================================//
    srand(time(NULL));		//permet d'avoir une valeur aléatoir dans "init()"

    randomizeWeights(&hiddenNeuron1);	//appelle les fonctions avec des pointeurs sur les variables de Neuron
    randomizeWeights(&hiddenNeuron2);
    randomizeWeights(&outputNeuron);
    training(epochs,&hiddenNeuron1,&hiddenNeuron2,&outputNeuron,inputTraining,results);
    return 0;

}
