#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//#include "../sdl/pixel_operations.h"

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

//Create & return the double* pixels values from filename
double* matrixFromFile(char* filename)
{
    double* matrix = malloc(sizeof(double) * 28 * 28);
    FILE* file = fopen(filename, "r");

    if(file == NULL)
    {
        printf("File is NULL \n");
    }

    for(int i = 0; i < 28; i++)
    {
        for(int j = 0; j <= 28; j++)
        {
            int c = fgetc(file);
            if(c == 49)
            matrix[j+i*28] = 1;
            if(c == 48)
            matrix[j+i*28] = 0;
        }
    }
    fclose(file);
    return matrix;
}

double** lettersMatrix()
{
  //Variables
  char uppercase_path[22] = "../uppercase/0/00.txt\0";
  char lowercase_path[22] = "../lowercase/0/00.txt\0";
  double** lettersMatrix = malloc(sizeof(double *) * 52);
  char uppercase = 'A';
  char lowercase = 'a';
  char count = '4';

  for(int i = 0; i < 52; i++)
  {

    if(i < 26)
    {
      uppercase_path[13] = uppercase;
      uppercase_path[15] = uppercase;
      uppercase_path[16] = count;
      lettersMatrix[i] = matrixFromFile(uppercase_path);
      uppercase++;

    }
    else if(i >= 26)
    {
      count = '3';
      lowercase_path[13] = lowercase;
      lowercase_path[15] = lowercase;
      lowercase_path[16] = count;
      lettersMatrix[i] = matrixFromFile(lowercase_path);
      lowercase++;

    }
  }
  return lettersMatrix;
}

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
        fprintf(file, "%f ", (double) *(net->WeightIH + (i * net->nbInput) + j ));
      }
      //printf("hidden\n");
      fprintf(file, "%lf\n", (double) *(net->BiasH + i));
    }
    for(int k = 0; k < net->nbOutput; k++)
    {
      for(int l = 0; l < net->nbHidden; l++)
      {
        fprintf(file, "%f ", (double) *(net->WeightHO + (k * net->nbHidden) + l ));
      }
      //printf("output\n");
      fprintf(file, "%lf\n", (double) *(net->BiasO + k));
    }
  }
  fclose(file);
}

// Fonction qui va a partir d'un fichier charger toutes les composantes d'un réseau de neurone
void LoadData(struct Neural_Network *net)
{
  FILE* file = fopen("network.txt", "r");

  int idc;
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
        idc = fscanf(file, "%lf ", (net -> WeightIH + (i * net->nbHidden) + j ));
      }
      //printf("hidden\n");
      idc = fscanf(file, "%lf\n", (net -> BiasH + i));
    }
    for(int k = 0; k < net->nbOutput; k++)
    {
      for(int l = 0; l < net->nbHidden; l++)
      {
        idc = fscanf(file, "%lf ", (net -> WeightHO + (k * net->nbOutput) + l ));
      }
      //printf("output\n");
      idc = fscanf(file, "%lf\n", (net -> BiasO + k));
    }
  }
  idc++;
  fclose(file);
}

void free_array(struct Neural_Network *net)
{
  printf("test tes\n");
  free(net->InputValue);
  printf("1 ok\n");

  free(net->Goal);
  printf("2 ok\n");

  free(net->WeightIH);
  printf("3 ok\n");

  free(net->WeightHO);
  printf("4 ok\n");

  free(net->BiasH);
  printf("5 ok\n");

  free(net->BiasO);
  printf("6 ok\n");

  free(net->OutputO);
  printf("7 ok\n");

  free(net->OutputH);
  printf("8 ok\n");

  free(net->dBiasH);
  printf("9 ok\n");

  free(net->dBiasO);
  printf("10 ok\n");

  free(net->dWeightIH);
  printf("11 ok\n");

  free(net->dWeightHO);
  printf("12 ok\n");

  free(net->dOutputO);
  printf("13 ok\n");

  free(net->dHidden);
  printf("14 ok\n");
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
  double** Matrix = lettersMatrix();
  int letter = 0;
  for (int i = 0; i < net->nbOutput; i++)
  {
    for (int j = 0; j < net->nbInput; j++)
    {
      //printf("%d\n",j);
        net-> InputValue[(i * net->nbInput) + j] = Matrix[letter][j];
    }
    //printf("%d\n",i);
    //printf("%d\n",letter);
    letter++;
  }
  
  for(int i = 0; i < net->nbOutput; i++)
  {
    for(int j = 0; j < net->nbOutput; j++)
    {
      if(i != j)
      {
        net->Goal[(i * net->nbOutput) + j] = 0;
      }
      else
      {
        net->Goal[(i * net->nbOutput) + j] = 1;
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
  
  net.InputValue = malloc(sizeof(double) * net.nbOutput * net.nbInput);
  net.Goal = malloc(sizeof(double) * net.nbOutput * net.nbOutput);
  
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
  //printf("ok 1\n");
  for(int h = 0; h < net->nbHidden; h++)
  {
    double sum_input_hidden = 0.0;
    for(int i = 0; i < net->nbInput; i++)
    {
      // on calcule la somme des poids * les inputs d'un neurone
      sum_input_hidden += net->WeightIH[(h * net->nbInput) + i] * net->InputValue[(h * net->nbInput) + i];
    }
    // on calcule la fonction d'activation de notre réseau
    net->OutputH[h] = sigmoid(sum_input_hidden + net->BiasH[h]);
  }
  //printf("ok 2\n");

  for(int o = 0; o < net->nbOutput; o++)
  {
    double sum_hidden_output = 0.0;
    for(int h = 0; h < net->nbHidden; h++)
    {
      // on calcule la somme des poids * les inputs d'un neurone
      sum_hidden_output += net->WeightHO[(o * net->nbHidden) + h] * net->OutputH[(o * net->nbHidden) + h];
    }
    net->OutputO[o] = sigmoid(sum_hidden_output + net->BiasO[o]);
  }

  //printf("ok 3\n");

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

	char goal;
	if(p < 26)
	{
		net->act = (char) net->OutputO[lmax] + (int) 'A';
		goal = (char) (p + (int) 'A');
	}
	else
	{
		net->act = (char) net->OutputO[lmax] + (int) 'a';
		goal = (char) (p + (int) 'a' - 26);
	}

	//printf("ok 5\n");
	if(epoch % 100 == 0)
	{
		printf("#########################\n");
		printf("Essai n°%d\n\n\n", epoch);
		printf("La réponse du réseau est : %c\n\n", net-> act);
		printf("La réponse attendu est : %c\n\n\n", goal);
	}
}

void BackwardPass(struct Neural_Network *net,int p) //backpropagation
{
  for(int o = 0; o < net->nbOutput; o++)
  {
    // cacul de l'erreur de sortie
    net->dOutputO[o] = (net->Goal[(p * net->nbInput) + o] - net->OutputO[o]) * dSigmoid(net->OutputO[o]);
    for(int h = 0; h < net->nbHidden; h++)
    {
      double dSumOutput = net->WeightHO[(o * net->nbHidden) + h] * net->dOutputO[(o * net->nbHidden) + h];
      net->dHidden[h + (o * net->nbHidden)] = dSumOutput * dSigmoid(net->OutputH[h]);
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
      net->dWeightIH[(h * net->nbInput) + i] = net->eta * net->InputValue[i + (p * net->nbInput)] * net->dHidden[h] + net->alpha * net->dWeightIH[(h * net->nbInput) + i];
      net->WeightIH[(h * net->nbInput) + i] += net->dWeightIH[(h * net->nbInput) + i];
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
      net->dWeightHO[(o * net->nbHidden) + h] = net->eta * net->OutputH[h] * net->dOutputO[o] + net->alpha * net->dWeightHO[(o * net->nbHidden) + h];
      net->WeightHO[(o * net->nbHidden) + h] += net->dWeightHO[(o * net->nbHidden) + h];
    }
  }
  //printf("ok 2\n");
}


void OCR()
{
  srand(time(NULL));

  int NbPattern = 52;
  int NbEpoch = 100;

  struct Neural_Network net_1 = InitalizeNetwork();
  struct Neural_Network *net = &net_1;

  //LoadData(net);

  InitalizeValue(net);
  for (int epoch = 0; epoch <= NbEpoch; ++epoch)
  {
    net -> ErrorRate = 0.0;
    for (int p = 0; p < NbPattern; ++p)
    {
      //printf("1\n");
      ForwardPass(net, p, epoch);
      //printf("2\n");
      BackwardPass(net, p);
      //printf("3\n");
    }
    
  }

  printf("Fail here\n");
  SaveData(net);
  printf("No here\n");
  free_array(net);
  printf("No here\n");
}

int main()
{
  OCR();
  return 1;
}

