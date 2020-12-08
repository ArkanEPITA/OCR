#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct Neural_Network
{
    int nbInput;
    int nbHidden;
    int nbOutput;

    //Arrays
    double *InputValue;
    double *Goal;
    //Weight Arrays
    double *WeightIH;
    double *WeightHO;
    //Bias Array
    double *BiasH;
    //Bias output
    double *BiasO;
    //Output Hidden
    double *OutputH;
    //Output of output
    double *OutputO;
    //Array of delta bias for hidden
    double *dBiasH;
    //delta bias for output
    double dBiasO;
    //delta weight
    double *dWeightIH;
    double *dWeightHO;
    //delta of output
    double *dOutputO;
    //delta of hidden
    double *dHidden;
    
    double ErrorRate;
    double eta;
    double alpha;
};

void SaveData(struct Neural_Network *net)
{
  FILE* file = fopen("network.txt", "w+");

  if(file == NULL)
  {
    printf("Erreur dans le chargement");
  }
  else
  {
    for(size_t i = 0; i < net->nbHidden; i++)
    {
      for(size_t j = 0; j < net->nbInput; j++)
      {
        fprintf(file, "%f ", (double) *(net -> WeightIH + i + j ));
      }
      //printf("hidden\n");
      fprintf(file, "%lf\n", (double) *(net -> BiasH + i));
    }
    for(size_t k = 0; k < net->nbOutput; k++)
    {
      for(size_t l = 0; l < net->nbHidden; l++)
      {
        fprintf(file, "%f ", (double) *(net -> WeightHO + k + l ));
      }
      //printf("output\n");
      fprintf(file, "%lf\n", (double) *net -> BiasO + k));
    }
  }
  fclose(file);
}

void LoadData(struct Neural_Network *net)
{
  FILE* file = fopen("network.txt", "r");

  if(file == NULL)
  {
    printf("Erreur dans le chargement");
  }
  else
  {
    for(size_t i = 0; i < net->nbHidden; i++)
    {
      for(size_t j = 0; j < net->nbInput; j++)
      {
        fscanf(file, "%lf ", (net -> WeightIH + i + j ));
      }
      printf("hidden\n");
      fscanf(file, "%lf\n", (net -> BiasH + i));
    }
    for(size_t k = 0; k < net->nbOutput; k++)
    {
      for(size_t l = 0; l < net->nbHidden; l++)
      {
        fscanf(file, "%lf ", (net -> WeightHO + k + l ));
      }
      printf("output\n");
      fscanf(file, "%lf\n", (net -> BiasO + k));
    }
  }
  fclose(file);
}

struct Neural_Network InitalizeNetwork()
{
  struct Neural_Network net;
  net.nbInput = 784;
  net.nbHidden = 64;
  net.nbOutput = 56;
  
  net.InputValue = malloc(sizeof(double)*56*784);
  net.Goal = malloc(sizeof(double)*56);
  
  net.WeightIH = malloc(sizeof(double)*net.nbInput * net.nbHidden);
  net.WeightHO = malloc(sizeof(double)*net.nbHidden);
  net.BiasH = malloc(sizeof(double)*net.nbHidden);
  net.BiasO = malloc(sizeof(double)*net.nbOutput);
  net.OutputH = malloc(sizeof(double)*net.nbHidden);
  net.OutputO = malloc(sizeof(double)*net.nbOutput);
  
  net.dBiasH = malloc(sizeof(double)*net.nbHidden);
  net.dBiasO = malloc(sizeof(double)*net.nbOutput);
  net.dWeightIH = malloc(sizeof(double)*net.nbInput * net.nbHidden);
  net.dWeightHO = malloc(sizeof(double)*net.nbHidden);
  net.dOutputO = malloc(sizeof(double)*net.nbOutput); 
  net.dHidden = malloc(sizeof(double)*net.nbHidden);
  
  net.ErrorRate = 0.0;
  net.eta = 0.5;
  net.alpha = 0.9;
  
  return(net);
}

void ForwardPass(struct Neural_Network *net, int p,int epoch)
{
  for (int h = 0; h <  net -> nbHidden; ++h)
  {
    double SumIH = 0.0;
    for (int i = 0; i < net -> nbInput; ++i)
    {
      SumIH += *(net -> WeightIH+(h+i*net -> nbHidden)) * 
                *(net -> InputValue+(i+p*net -> nbInput));
    }
    *(net -> OutputH +h) = sigmoid(SumIH +  *(net -> BiasH + h));
  }
  double SumHO = 0;
  for (int h = 0; h < net -> nbHidden; ++h)
  {
    SumHO += *(net -> WeightHO + h) * *(net -> OutputH +h);
  }
  net -> OutputO = sigmoid(SumHO + net -> BiasO);

  //Squared error function
  net -> ErrorRate += 0.5 * ( *(net -> Goal+p) - net -> OutputO) * 
                      (*(net -> Goal + p) - net -> OutputO);
  if (epoch % 1000 == 0)
  {
    printf("Pattern n°: %d | Input 1 : %f | Input 2 : %f | => Output: %f \n"
                    , p 
                    ,*(net -> InputValue + p*2 )
                    ,*(net -> InputValue + p*2 +1)
                    , net -> OutputO);
  }
}

void BackwardPass(struct Neural_Network *net,int p) //backpropagation
{
  
  net -> dOutputO = (*(net -> Goal+p) - net -> OutputO) * dSigmoid(net -> OutputO); //derivation sigmoid
  for (int h = 0; h < net -> nbHidden; ++h)
  {
    double dSumOutput = *(net -> WeightHO + h) * net -> dOutputO ;
    *(net -> dHidden +h) = dSumOutput * dSigmoid(*(net -> OutputH+h)); //derivation sigmoid
  }
  //update weights & bias between Input and Hidden layers
  for (int h = 0; h < net -> nbHidden; ++h)
  {
    //update BiasH
    *(net -> dBiasH + h) = net -> eta *  *(net -> dHidden +h);
    *(net -> BiasH +h) += *(net -> dBiasH +h) ;
    for (int i = 0; i < net -> nbInput; ++i)
    {
      //update WeightIH
      *(net -> dWeightIH+(h+i*net -> nbHidden)) = net -> eta * *(net -> InputValue + (i+p*net -> nbInput)) * *(net -> dHidden +h) + net -> alpha * *(net -> dWeightIH + (h+i*net -> nbHidden));
      *(net -> WeightIH + (h+i*net -> nbHidden)) += *(net -> dWeightIH + (h+i*net -> nbHidden));
    }
  }
    //update weights & bias between Hidden and Ouput layers
  //update BiasO
  net -> dBiasO = net -> eta * net -> dOutputO;
  net -> BiasO  += net -> dBiasO ;
  for (int h = 0; h < net -> nbHidden; ++h)
  {
    //update WeightsHO
    *(net -> dWeightHO + h) = net -> eta * *(net -> OutputH + h) * net -> dOutputO + net -> alpha * *(net -> dWeightHO + h);
    *(net -> WeightHO + h) += *(net -> dWeightHO + h) ;
  }
}

