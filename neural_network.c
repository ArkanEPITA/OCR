//Colors for print
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KWHT  "\x1B[37m"

#include "sdl/pixel_operations.h"
#include <time.h>


//Create & return the double* pixels values from filename
double *matrixFromFile(char *filename)
{
  double *matrix = malloc(sizeof(double) * 29 * 29);
  
  FILE *file = fopen(filename, "r"); // Open the file with letter

  if(file == NULL)
  {
      printf("File is NULL \n");
  }

  // Recup every value of the file
  for(int i = 0; i < 28; i++)
  {
      for(int j = 0; j <= 29; j++)
      {
          int c = fgetc(file);
          if(c == 49)
          {
            matrix[j+i*28] = 1;
          }
          if(c == 48)
          {
            matrix[j+i*28] = 0;
          }
      }
  }
  
  fclose(file);
  return matrix;
}

double** lettersMatrix(char count)
{
  char uppercase_path[19] = "uppercase/0/00.txt\0";
  char lowercase_path[19] = "lowercase/0/00.txt\0";
  double** lettersMatrix = malloc(sizeof(double *) * 52);
  char uppercase = 'A';
  char lowercase = 'a';

  // Travel all files to construct a data bank
  for(int i = 0; i < 52; i++)
  {
    if(i < 26)
    {
      uppercase_path[10] = uppercase;
      uppercase_path[12] = uppercase;
      uppercase_path[13] = count;
      lettersMatrix[i] = matrixFromFile(uppercase_path);
      uppercase++;

    }
    else if(i >= 26)
    {
      lowercase_path[10] = lowercase;
      lowercase_path[12] = lowercase;
      lowercase_path[13] = count;
      lettersMatrix[i] = matrixFromFile(lowercase_path);
      lowercase++;
    }
  }
  return lettersMatrix;
}

// Function which will create a file by recording in it all the weights and the biases, they will have the following form :
// the weights followed by a space to separate them and at the end the bias
void SaveData(struct Neural_Network *net)
{
  FILE* file = fopen("network.txt", "w+");

  if(file == NULL)
  {
    printf("Erreur dans le chargement\n");
  }
  else
  {
    for(int h = 0; h < net->nbHidden; h++)
    {
      for(int i = 0; i < net->nbInput; i++)
      {
        fprintf(file, "%f ",(net->WeightIH[h][i]));
      }
      fprintf(file, "%f\n",(net->BiasH[h]));
    }
    for(int o = 0; o < net->nbOutput; o++)
    {
        for(int h = 0; h < net->nbHidden; h++)
        {
          fprintf(file, "%f ",(net->WeightHO[o][h]));
        }
        fprintf(file, "%f\n",(net->BiasO[o]));
    }

  }

  fclose(file);
}


// Function which, from a file, will load all the components of a neural network
void LoadData(struct Neural_Network *net)
{
  FILE* file = fopen("network.txt", "r");

  double idc = 0.0;
  if(file == NULL)
  {
    printf("Erreur dans le chargement\n");
  }
  else
  {
    for(int i = 0; i < net->nbHidden; i++)
    {
      for(int j = 0; j < net->nbInput; j++)
      {
        idc = fscanf(file, "%lf\n", (&net->WeightIH[i][j]));
      }
      
      idc = fscanf(file, "%lf\n", (&net->BiasH[i]));
    }
    for(int k = 0; k < net->nbOutput; k++)
    {
      for(int l = 0; l < net->nbHidden; l++)
      {
        idc = fscanf(file, "%lf\n", (&net->WeightHO[k][l]));
      }
      idc = fscanf(file, "%lf\n", (&net->BiasO[k]));
    }
    fclose(file);
  }
  idc++;
  
}

void free_array(struct Neural_Network *net)
{
  free(net);

}

// Random function allowing to generate a random number
double Random()
{
    double rnd;
    rnd = ((double)rand()) / ((double)RAND_MAX / 2) - 1;
    return rnd / 1000;
}

// Fonction to initialize the network
struct Neural_Network* InitalizeNetwork()
{
  struct Neural_Network *net = NULL;
  net = malloc(sizeof(struct Neural_Network));
  net->nbInput = 784;
  net->nbHidden = 64;
  net->nbOutput = 52;

  net->ErrorRate = 0.0;
  net->eta = 0.5;
  net->alpha = 0.9;
  
  return net;
}

// Fonction that init weight and bias only the first time that we run the training
void initWB(struct Neural_Network *net)
{
  for (int h = 0; h < net->nbHidden; h++)
  {
    for (int i = 0; i < net->nbInput; i++)
    {
      net->WeightIH[h][i] = Random();
      net->dWeightIH[h][i] = 0.0;
    }
    net->BiasH[h] = Random();
    net->dBiasH[h] = 0.0;
  }
  
  
  for(int o = 0; o < net->nbOutput; o++)
  {
    for(int h  = 0; h < net->nbHidden; h++)
    {
      net->WeightHO[o][h] = Random();
      net->dWeightHO[o][h] = 0.0;
    }
    net->BiasO[o] = Random();
    net->dBiasO[o] = 0.0;
  }
}

// Function that will initialize value for each front
void InitalizeValue(struct Neural_Network *net, int nb, char count)
{

  //initialization of input values
  double **Matrix = lettersMatrix(count);
  int letter = 0;
  for (int i = 0; i < net->nbOutput; i++)
  {
    for (int j = 0; j < net->nbInput; j++)
    {
      net-> InputValue[i][j] = Matrix[letter][j];
    }
    letter++;
  }

  //initialize Goal value, weight and bias only the first time that will be called
  if(nb == 0)
  {
    for(int i = 0; i < net->nbOutput; i++)
    {
        for(int j = 0; j < net->nbOutput; j++)
        {
            if(i != j)
            {
                net->Goal[i][j] = 0;
            }
            else
            {
                net->Goal[i][j] = 1;
            }
        }
    }
    initWB(net);
  }
}


// Sigmoid fonction (activation fonction)
double sigmoid(double x)
{
  double res = (1.0 / (1.0 + exp(-x)));
  return res;
}

//Derivative of sigmoid for the backpropagation
double dSigmoid(double z)   //derivative of the activating sigmoid function
{
    double res = z * (1 - z);
    return res;
}


//Fonction that will compute all network value
void ForwardPass(struct Neural_Network *net, int p, int epoch)
{
  double sum;

  //Calculate Output for Hidden neurons
  for (int h = 0; h < net -> nbHidden; h++)
  {
    sum = 0.0;
    for (int i = 0; i < net -> nbInput; i++)
    {
      sum += (net -> WeightIH[h][i]) * (net -> InputValue[p][i]);
    }
    net -> OutputH[h] = sigmoid(sum + (net->BiasH[h]));
  }

  //Calculate Output for Output neurons
  for (int o = 0; o < net -> nbOutput; o++)
  {
    sum = 0.0;
    for (int h = 0; h < net -> nbHidden; h++)
    {
      sum += (net->WeightHO[o][h]) * (net-> OutputH[h]);
    }
    net -> OutputO[o] = sigmoid(sum + net->BiasO[o]);
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
  if(lmax < 26)
  {
    net->act = (char) lmax + (int) 'A';
  }
  else
  {
    net->act = (char) lmax + (int) 'a' - 26;
  }

  if(p < 26)
  {
    goal = (char) (p + (int) 'A');
  }
  else
  {
    goal = (char) (p + (int) 'a' - 26);
  }

  if(epoch % 10 == 0)
  	{
  		if(goal == 'A')
      {
        printf("\n\n");
        printf("###########################################\n");
        printf("              Essai numéro : %d\n\n\n", epoch);
      }
      if(goal != net->act)
      {
        printf("La réponse du réseau est   : %c\n", net-> act);
        printf("La réponse attendu est     : %c\n", goal);
        printf("%s", KWHT);
        printf("Le résultat de l'essai est :      %sKO\n\n", KRED);
        printf("%s", KWHT);
      }
      else
      {
        printf("La réponse du réseau est   : %c\n", net-> act);
        printf("La réponse attendu est     : %c\n", goal);	
        printf("%s", KWHT);
        printf("Le résultat de l'essai est :      %sOK\n\n", KGRN);
        printf("%s", KWHT);
      }
  	}
}

//backpropagation
void BackwardPass(struct Neural_Network *net, int p) 
{
  //Calulate Cost function for output neurons
    for (int o = 0; o < net -> nbOutput; o++)
    {
      net -> dOutputO[o] = ((net -> Goal[p][o]) - (net -> OutputO[o])) * dSigmoid(net -> OutputO[o]);
    }

    //Weights between Hidden and Ouput layers
    for (int o = 0; o < net -> nbOutput; o++)
    {
      for (int h = 0; h < net -> nbHidden; h++)
      {
        net -> WeightHO[o][h] += (net->eta) * (net-> dOutputO[o]) * (net-> OutputH[h]) + (net->alpha) * (net-> dWeightHO[o][h]);
        net -> dWeightHO[o][h] = (net->eta) * (net->dOutputO[o]) * (net-> OutputH[h]);
      }
      //Update BiasO
      net -> BiasO[o] += (net -> eta) * (net -> dOutputO[o]);
    }


    ///Calculate Cost function for hidden neurons
    double sum;
  
    for(int h = 0; h < net->nbHidden; h++)
    {
      sum = 0.0;
      for(int o = 0; o < net->nbOutput; o++)
      {
        sum += net->WeightHO[o][h] * net->dOutputO[o]; 
      }
      net->dHiddenH[h] = sum * dSigmoid(net->OutputH[h]);
    }

     //Weights between Input and Hidden layers
    for (int h = 0; h < net -> nbHidden; h++)
    {
      for(int i = 0; i < net -> nbInput; i++)
      {
        net -> WeightIH[h][i] += (net -> eta) * (net -> dHiddenH[h]) * (net -> InputValue[p][i]) + (net -> alpha) * (net -> dWeightIH[h][i]);
        net -> dWeightIH[h][i] = (net -> eta) * (net -> dHiddenH[h]) * (net->InputValue[p][i]);
      }
      //Update BiasH
      net -> BiasH[h] += (net->eta) * (net-> dHiddenH[h]);
    }
}

// Training fonction
void train()
{
  srand(time(NULL));

  int NbPattern = 52;
  int NbEpoch = 1500;
  int nb = 0;
  struct Neural_Network *net = InitalizeNetwork();
  
  for (int epoch = 0; epoch <= NbEpoch; ++epoch)
  {
    for(char count = '0'; count < '3'; count++)
    {
      InitalizeValue(net, nb, count);

      for (int p = 0; p < NbPattern; ++p)
      {
        ForwardPass(net, p, epoch);
        BackwardPass(net, p);
      }
      nb++;
    }
  }
  SaveData(net);
  free_array(net);
  printf("End\n");
}

// fonction that will use the neural network and return the letter
char run(double letter[784])
{
  struct Neural_Network *net = InitalizeNetwork();
  for(int i = 0; i < 28; i++)
  {
    for(int j = 0; j < 28; j++)
    {
      net->InputValue[0][i * 28 + j] = letter[i * 28 + j];
    }
  }

  LoadData(net);
  ForwardPass(net,0 , 5);

  char res = net->act;

  free_array(net);
  return res;
}