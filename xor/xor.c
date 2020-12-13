#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


double sigmoid(double z)    // sigmoid activation function of our neuron network
{
    double res = 1 / (1 + exp(-z));
    return res;
}

double dSigmoid(double z)   //derivative of the activating sigmoid function
{
    double res = z * (1 - z);
    return res;
}

double init()   //function to initialize the weights randomly
{
    double rnd;
    rnd = ((double)rand()) / ((double)RAND_MAX / 2) - 1;
    return rnd /2;
}


//=========================================//
//                Structures               //
//=========================================//
typedef struct Neurons Neuron; 	//structure of our neurons
struct Neurons
{
   double inputs[2];	//the two inputs of our neuron
   double weights[2];	//the weights of the links between the input neurons and the current neuron
   double error;		//the error of our neuron
   double biasWeight;	//through our neuron
};


//========================================//
//                functions               //
//========================================//

double output(Neuron* hNeuron)	//calculates the activation function of a neuron using the sigmoid
{
    return (sigmoid(hNeuron->weights[0] * hNeuron->inputs[0] + hNeuron->weights[1] * hNeuron->inputs[1] + hNeuron->biasWeight));
}

void randomizeWeights(Neuron* hNeuron)	//randomly initializes the weights of a neuron with "init ()"
{										//randomly initializes the bias
    hNeuron->weights[0] = init();
    hNeuron->weights[1] = init();
    hNeuron->biasWeight = init();
}

void adjustWeights(Neuron* hNeuron)	//updates the weights of a neuron with respect to the input and the error
{									//updates through a neuron with respect to the error
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
            //forward propagation -> calculates the output of the neuron of the output layer compared to the input values
            hneuron1->inputs[0] = inputTraining[i][0];
            hneuron1->inputs[1] = inputTraining[i][1];
            hneuron2->inputs[0] = inputTraining[i][0];
            hneuron2->inputs[1] = inputTraining[i][1];
            neuron->inputs[0] = output(hneuron1);
            neuron->inputs[1] = output(hneuron2);

            printf("%f xor %f = %f\n",inputTraining[i][0],inputTraining[i][1], output(neuron));

            //back propagation -> allows to adjust the weight and bias values according to the error

            //adjusts the weight of the output neuron, depending on its error using the sigmoid derivative
            neuron->error = dSigmoid(output(neuron)) * (results[i] - output(neuron));
            adjustWeights(neuron);
            //adjusts the weight of hidden neurons, based on their errors using the sigmoid derivative
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

    Neuron hiddenNeuron1, hiddenNeuron2, outputNeuron;	//creation of three variables of the Neuron structure

    double inputTraining[4][2] = 		//set the possible input values
    {
        { 0.0f, 0.0f},
        { 0.0f, 1.0f},
        { 1.0f, 0.0f},
        { 1.0f, 1.0f}
    };

    double results[4] = { 0, 1, 1, 0 };	//the different expected results compared to inputTraining
    double epochs = 20000;



//========================================//
//                programs                 //
//========================================//
    srand(time(NULL));		//allows to have a random value in "init ()"

    randomizeWeights(&hiddenNeuron1);	//calls functions with pointers to Neuron variables
    randomizeWeights(&hiddenNeuron2);
    randomizeWeights(&outputNeuron);
    training(epochs,&hiddenNeuron1,&hiddenNeuron2,&outputNeuron,inputTraining,results);
    return 0;

}
