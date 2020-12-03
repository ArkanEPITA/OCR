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
    double a = 1.0;
    for(int i = 0; i < 2; i++)	//allows to obtain a value with a greater variation for i = 1 than for 1 = 0
    {
        rnd = ((double)rand()/(double)(RAND_MAX)) * a;	//allows you to retrieve a random value between 0 and 1
    }
    return rnd;
}


//=========================================//
//                Structures               //
//=========================================//
typedef struct Neuron Neuron; 	//structure of our neurons
struct Neuron
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

void initWeigthsWithValue(Neuron* hNeuron, double weight1, double weight2, double bias)
{
    hNeuron->weights[0] = weight1;
    hNeuron->weights[1] = weight2;
    hNeuron->biasWeight = bias;
}

void adjustWeights(Neuron* hNeuron)	//updates the weights of a neuron with respect to the input and the error
{									//updates through a neuron with respect to the error
    hNeuron->weights[0] += hNeuron->error * hNeuron->inputs[0];
    hNeuron->weights[1] += hNeuron->error * hNeuron->inputs[1];
    hNeuron->biasWeight += hNeuron->error;
}

void training(double epochs, Neuron* hneuron1, Neuron* hneuron2, Neuron* neuron, double inputTraining[][2], double results[4])
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

void run(Neuron* hneuron1, Neuron* hneuron2, Neuron* neuron, double input[2])
{
    hneuron1->inputs[0] = input[0];
    hneuron1->inputs[1] = input[1];
    hneuron2->inputs[0] = input[0];
    hneuron2->inputs[1] = input[1];
    neuron->inputs[0] = output(hneuron1);
    neuron->inputs[1] = output(hneuron2);

    printf("%f xor %f = %f\n",input[0],input[1], output(neuron));
}

int main(int argc, char* argv[])
{

//========================================//
//                Variables               //
//========================================//

    Neuron hiddenNeuron1, hiddenNeuron2, outputNeuron;	//creation of three variables of the Neuron structure

    if(argc == 1 || argc > 4)
    {
        printf("Entrez un argument après l'exécution de la commande.\nPour réaliser un entrainement passer 1 comme argument\nPour utiliser le réseau de neurone veuillez passer 2 suivit de ce que vous voulez tester : par exemple 0 0 ou 0 1 ...\n");
    }
    else if(argc == 2)
    {
        FILE* fichier = NULL;

        char train[] = "1";

        int i;

        int trains = 0;

        for(i = 0; argv[1][i] == train[i] && argv[1][i] == '\0'; i++)
        if(argv[1][i] == train[i])
        {
            trains = 1;
        }
        else
        {
            
        }
        if(trains == 1)
        {

            //========================================//
            //                Training                //
            //========================================//

            double inputTraining[4][2] = 		//set the possible input values
            {
                { 0.0f, 0.0f},
                { 0.0f, 1.0f},
                { 1.0f, 0.0f},
                { 1.0f, 1.0f}
            };

            double results[4] = { 0, 1, 1, 0 };	//the different expected results compared to inputTraining
            double epochs = 2000;
 
            srand(time(NULL));		//allows to have a random value in "init ()"

            randomizeWeights(&hiddenNeuron1);	//calls functions with pointers to Neuron variables
            randomizeWeights(&hiddenNeuron2);
            randomizeWeights(&outputNeuron);
            training(epochs, &hiddenNeuron1, &hiddenNeuron2, &outputNeuron, inputTraining, results);

            fichier = fopen("inputs.txt", "r");
            if(fichier != NULL)
            {
                double weigth1 = hiddenNeuron1.weights[0];
                double weigth2 = hiddenNeuron1.weights[1];
                double bias = hiddenNeuron1.biasWeight;
                char string[100];
                sprintf(string, "%lf", weigth1);
                string += "\n";
                sprintf(string, "%lf", weigth2);
                string += "\n";
                sprintf(string, "%lf", bias);
                string += "\n";
                weigth1 = hiddenNeuron2.weights[0];
                weigth2 = hiddenNeuron2.weights[1];
                bias = hiddenNeuron2.biasWeight;
                sprintf(string, "%lf", weigth1);
                string += "\n";
                sprintf(string, "%lf", weigth2);
                string += "\n";
                sprintf(string, "%lf", bias);
                string += "\n";
                weigth1 = outputNeuron.weights[0];
                weigth2 = outputNeuron.weights[1];
                bias = outputNeuron.biasWeight;
                sprintf(string, "%lf", weigth1);
                string += "\n";
                sprintf(string, "%lf", weigth2);
                string += "\n";
                sprintf(string, "%lf", bias);
                string += "\n";
                fputs(string, fichier);
            }
            else
            {
                printf("Erreur d'ouverture du fichier\n");
            }
        }
        else
        {
            printf("Entrez un argument après l'exécution de la commande.\nPour réaliser un entrainement passer 1 comme argument\nPour utiliser le réseau de neurone veuillez passer 2 suivit de ce que vous voulez tester : par exemple 0 0 ou 0 1 ...\n");
        }
    }
    else if(argc == 4)
    {
        char run[] = "2";

        int i;

        int runs = 0;

        for(i = 0; argv[1][i] == run[i] && argv[1][i] == '\0'; i++)
            if(argv[1][i] == run[i])
            {
                runs = 1;
            }
        if(runs == 1 && (argv[2][0] == '0' || argv[2][0] == '0') && (argv[3][0] == '0' || argv[3][0] == '0'))
        {
            fichier = fopen("inputs.txt", "r");
            if(fichier != NULL)
            {
                char string[100] = "";
                double weigth1;
                double weigth2;
                double bias;
                int caractereActuel = fgetc(fichier);
                while(caractereActuel != 10) //tant que mon caractere n'est pas un retoura la ligne
                {
                    string += (char) caractereActuel;
                }
                weigth1 = atof(string);
                string = "";
                while(caractereActuel != 10) //tant que mon caractere n'est pas un retoura la ligne
                {
                    string += (char) caractereActuel;
                }
                weigth2 = atof(string);
                string = "";
                while(caractereActuel != 10) //tant que mon caractere n'est pas un retoura la ligne
                {
                    string += (char) caractereActuel;
                }
                bias = atof(string);
                string = "";
                int input1 = argv[2][0] - '0';
                int input2 = argv[3][0] - '0';
                double input[2] = {double input1, double input2};

                initWeigthsWithValue(&hiddenNeuron1, weigth1, weigth2, bias);

                while(caractereActuel != 10) //tant que mon caractere n'est pas un retoura la ligne
                {
                    string += (char) caractereActuel;
                }
                weigth1 = atof(string);
                string = "";
                while(caractereActuel != 10) //tant que mon caractere n'est pas un retoura la ligne
                {
                    string += (char) caractereActuel;
                }
                weigth2 = atof(string);
                string = "";
                while(caractereActuel != 10) //tant que mon caractere n'est pas un retoura la ligne
                {
                    string += (char) caractereActuel;
                }
                bias = atof(string);
                string = "";
                int input1 = argv[2][0] - '0';
                int input2 = argv[3][0] - '0';
                double input[2] = {double input1, double input2};

                initWeigthsWithValue(&hiddenNeuron2, weigth1, weigth2, bias);

                while(caractereActuel != 10) //tant que mon caractere n'est pas un retoura la ligne
                {
                    string += (char) caractereActuel;
                }
                weigth1 = atof(string);
                string = "";
                while(caractereActuel != 10) //tant que mon caractere n'est pas un retoura la ligne
                {
                    string += (char) caractereActuel;
                }
                weigth2 = atof(string);
                string = "";
                while(caractereActuel != 10) //tant que mon caractere n'est pas un retoura la ligne
                {
                    string += (char) caractereActuel;
                }
                bias = atof(string);
                string = "";
                int input1 = argv[2][0] - '0';
                int input2 = argv[3][0] - '0';
                double input[2] = {double input1, double input2};

                initWeigthsWithValue(&outputNeuron, weigth1, weigth2, bias);

                run(&hiddenNeuron1, &hiddenNeuron2, &outputNeuron, input);

                fclose(fichier);
                printf("\nFin de l'execution du programme\n");
            }
            else
            {
                printf("Veuillez lancer l'entrainement avant de tester le réseau\n");
            }
            
        }
        else
        {
            printf("Entrez un argument après l'exécution de la commande.\nPour réaliser un entrainement passer 1 comme argument\nPour utiliser le réseau de neurone veuillez passer 2 suivit de ce que vous voulez tester : par exemple 0 0 ou 0 1 ...\n");
        }
    }
    
    return 0;

}
