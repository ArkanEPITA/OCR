#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "xor_test.h"


void main(int argc, char *argv[])
{

//========================================//
//                Variables               //
//========================================//

    Nodes N;
    Training T;

//========================================//
//                program                 //
//========================================//
    srand(time(NULL));
    init_weight(numInputs, numHiddenNodes, numOutputs, hiddenWeights, outputWeights);

    init_bias(numHiddenNodes, numOutputs, hiddenLayerBias, outputLayerBias);

    training(epochs, numInputs, numHiddenNodes, numOutputs, hiddenLayer, outputLayer, hiddenLayerBias, outputLayerBias, hiddenWeights, outputWeights, numTrainingSets, training_inputs, training_outputs, deltaOutput);

    
}
