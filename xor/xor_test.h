#define XOR_TEST_H
#define XOR_TEST_H

typedef struct Nodes Nodes;
struct Nodes
{
    int numInputs = 2;  //nombre d'entré
    int numHiddenNodes = 2; //nombre de noeud caché
    int numOutputs = 1; //nombre de sortie

    double hiddenLayer[numHiddenNodes]; //création des couches caché
    double outputLayer[numOutputs]; //création des couches de sortie
    
    double hiddenLayerBias[numHiddenNodes]; //biais des couches caché
    double outputLayerBias[numOutputs]; //bias des couches de sortie

    double hiddenWeights[numInputs][numHiddenNodes]; //poid des couches caché
    double outputWeights[numHiddenNodes][numOutputs]; //poid des couches de sortie
};

typedef struct Training Training;
struct Training
{
    int numTrainingSets = 4; //nombre de cas d'entrées

    double training_inputs[4][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};

    double training_outputs[4][1] = {{0.0f}, {1.0f}, {1.0f}, {0.0f}};

    int epochs = 10000; //nombre d'itération des numTrainingSets
};

double sigmoid();
double dSigmoid();
double init()
void init_weight();
void init_bias();
void training();
void hidden_activation();
void output_activation();
void modif_hidden_weight();
void modif_output_weight();


#endif