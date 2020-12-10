#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../sdl/pixel_operations.h"
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
    double *dBiasO;
    //delta weight
    double *dWeightIH;
    double *dWeightHO;
    //delta of output
    double *dOutputO;
    //delta of hidden
    double *dHidden;

    char act;
    
    double ErrorRate;
    double eta;
    double alpha;
};

// Fonction qui va créer un fichier en enregistrant dedans tout les poids et les biais, ils vont avoir le forme suivante : 
// les poids suivit d'un espace pour les séparer et a la fin les biais
void SaveData(struct Neural_Network *net)
{
  FILE* file = fopen("network.txt", "w+");

  if(file == NULL)
  {
    printf("Erreur dans le chargement");
  }
  else
  {
    for(int i = 0; i < net->nbHidden; i++)
    {
      for(int j = 0; j < net->nbInput; j++)
      {
        fprintf(file, "%f ", (double) *(net -> WeightIH + (i * net->nbHidden) + j ));
      }
      //printf("hidden\n");
      fprintf(file, "%lf\n", (double) *(net -> BiasH + i));
    }
    for(int k = 0; k < net->nbOutput; k++)
    {
      for(int l = 0; l < net->nbHidden; l++)
      {
        fprintf(file, "%f ", (double) *(net -> WeightHO + (k * net->nbOutput) + l ));
      }
      //printf("output\n");
      fprintf(file, "%lf\n", (double) *net -> BiasO + k);
    }
  }
  fclose(file);
}

// Fonction qui va a partir d'un fichier charger toutes les composantes d'un réseau de neurone
void LoadData(struct Neural_Network *net)
{
  FILE* file = fopen("network.txt", "r");

  if(file == NULL)
  {
    printf("Erreur dans le chargement");
  }
  else
  {
    for(int i = 0; i < net->nbHidden; i++)
    {
      for(int j = 0; j < net->nbInput; j++)
      {
        fscanf(file, "%lf ", (net -> WeightIH + (i * net->nbHidden) + j ));
      }
      printf("hidden\n");
      fscanf(file, "%lf\n", (net -> BiasH + i));
    }
    for(int k = 0; k < net->nbOutput; k++)
    {
      for(int l = 0; l < net->nbHidden; l++)
      {
        fscanf(file, "%lf ", (net -> WeightHO + (k * net->nbOutput) + l ));
      }
      printf("output\n");
      fscanf(file, "%lf\n", (net -> BiasO + k));
    }
  }
  fclose(file);
}

void free_array(struct Neural_Network *net)
{
  free(net->InputValue);
  free(net->Goal);
  free(net->WeightIH);
  free(net->WeightHO);
  free(net->BiasH);
  free(net->BiasO);
  free(net->OutputO);
  free(net->OutputH);
  free(net->dBiasH);
  free(net->dBiasO);
  free(net->dWeightIH);
  free(net->dWeightHO);
  free(net->dOutputO);
  free(net->dHidden);
}

// Fonction random permettant de générer un chifre au hasard
static double Random()
{
    double rnd;
    double a = 1.0;
    for(int i = 0; i < 2; i++)  //allows to obtain a value with a greater variation for i = 1 than for 1 = 0
    {
        rnd = ((double)rand()/(double)(RAND_MAX)) * a;  //allows you to retrieve a random value between 0 and 1
    }
    return rnd;
}

void InitalizeValue(struct Neural_Network *net)
{

  // il faut initialiser les valeur d'input
  double** lettersMatrix = LetterMatrix();
  int letter = 0;
  for (int i = 0; i < net->nbOutput; i++)
  {
    for (int j = 0; j < net->nbInput; j++)
    {
      net-> InputValue[(i * net->nbOutput) + j] = lettersMatrix[letter][j];
    }
    letter++;
  }
  
  for(int i = 0; i < net->nbOutput; i++)
  {
    for(int j = 0; j < net->nbOutput; j++)
    {
      if(i != j)
      {
        Goal[(i * net->nbOutput) + j] = 0;
      }
      else
      {
        Goal[(i * net->nbOutput) + j] = 1;
      }
    }
  }

  for (int h = 0; h < net->nbHidden; h++)
  {
    for (int i = 0; i < net->nbInput; i++)
    {
      net->WeightIH[(h * net->nbHidden) + i] = Random();
      net->dWeightIH[(h * net->nbHidden) + i] = 0.0;
    }
    net->BiasH[h] = Random();
    net->dBiasH[h] = 0.0;
  }
  
  
  for(int o = 0; o < net->nbOutput; o++)
  {
    for(int h  = 0; h < net->nbHidden; h++)
    {
      net->WeightHO[h + (o * net->nbOutput)] = Random();
      net->dWeightHO[h + (o * net->nbOutput)] = 0.0;
    }
    net->BiasO[o] = Random();
    net->dBiasO[o] = 0.0;
  }
}


// Fonction sigmoid servant de fonction d'activation
static double sigmoid(double x)
{
  return(1.0 / (1.0 + exp(-x)));
}

// Fonction dérivé de la sigmoid pour propager vers l'arrière notre erreur
static double dSigmoid(double z)   //derivative of the activating sigmoid function
{
    double res = z * (1 - z);
    return res;
}

struct Neural_Network InitalizeNetwork()
{
  struct Neural_Network net;
  net.nbInput = 784;
  net.nbHidden = 64;
  net.nbOutput = 52;
  
  net.InputValue = malloc(sizeof(double) * 52 * 784);
  net.Goal = malloc(sizeof(double) * 52 * 52);
  
  net.WeightIH = malloc(sizeof(double) * net.nbInput * net.nbHidden);
  net.WeightHO = malloc(sizeof(double) * net.nbHidden * net.nbOutput);
  net.BiasH = malloc(sizeof(double) * net.nbHidden);
  net.BiasO = malloc(sizeof(double) * net.nbOutput);
  net.OutputH = malloc(sizeof(double) * net.nbHidden);
  net.OutputO = malloc(sizeof(double) * net.nbOutput);
  
  net.dBiasH = malloc(sizeof(double) * net.nbHidden);
  net.dBiasO = malloc(sizeof(double) * net.nbOutput);
  net.dWeightIH = malloc(sizeof(double) * net.nbInput * net.nbHidden);
  net.dWeightHO = malloc(sizeof(double) * net.nbHidden * net.nbOutput);
  net.dOutputO = malloc(sizeof(double) * net.nbOutput); 
  net.dHidden = malloc(sizeof(double) * net.nbHidden);
  
  net.ErrorRate = 0.0;
  net.eta = 0.5;
  net.alpha = 0.9;
  
  return(net);
}

void ForwardPass(struct Neural_Network *net, int p, int epoch)
{
  for(int h = 0; h < net->nbHidden; h++)
  {
    double sum_input_hidden = 0.0;
    for(int i = 0; i < nbInput; i++)
    {
      // on calcule la somme des poids * les inputs d'un neurone
      sum_input_hidden += net->WeightIH[(h * net->nbHidden) + i] * net->InputValue[(h * net->nbHidden) + i];
    }
    // on calcule la fonction d'activation de notre réseau
    net->OutputH[h] = sigmoid(sum_input_hidden + net->BiasH[h]);
  }


  for(int o = 0; o < net->nbOutput; o++)
  {
    double sum_hidden_output = 0.0;
    for(int h = 0; h < net->nbHidden; h++)
    {
      // on calcule la somme des poids * les inputs d'un neurone
      sum_hidden_output += net->WeightHO[(o * net->nbOutput) + h] * net->OutputH[(o * net->nbOutput) + h];
    }
    net->OutputO[o] = sigmoid(sum_hidden_output + net->BiasO[o]):
  }


  // Il faut calculer le taux d'erreur

  for(int o = 0; o < net->nbOutput; o++)
  {
    net->ErrorRate += 0.5 * ((net->Goal[(p * net->nbOutput) + o] - net->OutputO[o]) * (net->Goal[(p * net->nbOutput) + o] - net->OutputO[o]));
  }
  
  double max = 0.0;
  int lmax = 0;
  for(int o = 0; o < net->nbOutput; o++)
  {
    double act = net->OutputO[o];
    if(act > max)
    {
      lmax = o;
      max = act;
    }
  }
  net->act = (char) act + 65;

  if(epoch % 100 == 0)
  {
    printf("#########################\n");
    printf("Essai n°%d\n\n\n", epoch);
    printf("La réponse du réseau est : %c\n\n", net-> act);
    printf("La réponse attendu est : %c\n\n\n", (char) p + 65);
  }

  /*
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
  }*/
}

void BackwardPass(struct Neural_Network *net,int p) //backpropagation
{
  for(int o = 0; o < net->nbOutput; o++)
  {
    // cacul de l'erreur de sortie
    net->dOutputO[o] = (net->Goal[(p * net->nbOutput) + o] - net->OutputO[o]) * dSigmoid(net->OutputO[o]);
    for(int h = 0; h < net->nbHidden; h++)
    {
      double dSumOutput = net->WeightHO[(o * net->nbOutput) + h] * net->dOutputO[(o * net->nbOutput) + h];
      net->dHidden[h + (o * net->nbOutput)]
    }
  }

  for (int h = 0; h < net->nbHidden; ++h)
  {
    //update BiasH
    net->dBiasH[h] = net->eta * net->dHidden[h];
    net->BiasH[h] += net->dBiasH[h];
    for (int i = 0; i < net->nbInput; ++i)
    {
      //update WeightIH
      net->dWeightIH[(h * net->nbHidden) + i] = net->eta * net->InputValue[i + (p * net->nbOutput)] * net->dHidden[h] + net->alpha * net->dWeightIH[(h * net->nbHidden) + i];
      net->WeightIH[(h * net->nbHidden) + i] += net->dWeightIH[(h * net->nbHidden) + i];
    }
  }

  for(int o = 0; o < net->nbOutput; o++)
  {
    //update BiasO
    net->dBiasO[o] = net->eta * net->dOutputO[o];
    net->BiasO[o]  += net->dBiasO[o];
    for (int h = 0; h < net -> nbHidden; ++h)
    {
      //update WeightsHO
      net->dWeightHO[(o * net->nbOutput) + h] = net->eta * net->OutputH[h] * net->dOutputO[o] + net->alpha * net->dWeightHO[(o * net->nbOutput) + h];
      net->WeightHO[(o * net->nbOutput) + h] += net->dWeightHO[(o * net->nbOutput) + h];
    }
  }

  /*
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
  }*/
}


void OCR()
{
  srand(time(NULL));

  int NbPattern = 52;
  int NbEpoch = 500;

  struct Neural_Network net_1 = InitalizeNetwork();
  struct Neural_Network *net = &net_1;

  LoadData(net);

  //InitalizeValue(net);
  for (int epoch = 0; epoch <= NbEpoch; ++epoch)
  {
    net -> ErrorRate = 0.0;
    for (int p = 0; p < NbPattern; ++p)
    {
      ForwardPass(net, p, epoch);
      BackwardPass(net, p);
    }
    // TODO print
  }

  SaveData(net);
  free_array(net);
}

int main()
{
  OCR();
  return 1;
}

