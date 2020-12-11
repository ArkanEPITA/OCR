#include "neural_network2.h"


//Create & return the double* pixels values from filename
double *matrixFromFile(char *filename)
{
    double *matrix = malloc(sizeof(double) * 28 * 28);
    FILE *file = fopen(filename, "r");

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
/*
// Fonction qui va a partir d'un fichier charger toutes les composantes d'un réseau de neurone
void LoadData(struct Neural_Network *net)
{
  FILE* file = fopen("network.txt", "r");

  double idc;
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
        idc = fscanf(file, "%f\n", (net->WeightIH[i][j]));
      }
      //printf("hidden\n");
      idc = fscanf(file, "%f\n", (net->BiasH[i]));
    }
    for(int k = 0; k < net->nbOutput; k++)
    {
      for(int l = 0; l < net->nbHidden; l++)
      {
        idc = fscanf(file, "%f\n", (net->WeightHO[k][l]));
      }
      idc = fscanf(file, "%f\n", (net->BiasO[k]));
    }
  }
  idc++;
  fclose(file);
}
*/
void free_array(struct Neural_Network *net)
{
  printf("test tes\n");
  free(net);

}

// Fonction random permettant de générer un chifre au hasard
double Random()
{
    double rnd;
    double a = 1.0;
    for(int i = 0; i < 2; i++)  //allows to obtain a value with a greater variation for i = 1 than for 1 = 0
    {
        rnd = ((double)rand()/(double)(RAND_MAX)) * a;  //allows you to retrieve a random value between 0 and 1
    }
    return rnd;
}

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

void InitalizeValue(struct Neural_Network *net)
{

  // il faut initialiser les valeur d'input
  double **Matrix = lettersMatrix();
  int letter = 0;
  for (int i = 0; i < net->nbOutput; i++)
  {
    for (int j = 0; j < net->nbInput; j++)
    {
      //printf("%d\n",j);
        net-> InputValue[i][j] = Matrix[letter][j];
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
        net->Goal[i][j] = 0;
      }
      else
      {
        net->Goal[i][j] = 1;
      }
    }
  }

  for (int h = 0; h < net->nbHidden; h++)
  {
    for (int i = 0; i < net->nbInput; i++)
    {
      net->WeightIH[h][i] = Random() / 1000;
      net->dWeightIH[h][i] = 0.0;
    }
    net->BiasH[h] = Random() / 1000;
    net->dBiasH[h] = 0.0;
  }
  
  
  for(int o = 0; o < net->nbOutput; o++)
  {
    for(int h  = 0; h < net->nbHidden; h++)
    {
      net->WeightHO[o][h] = Random() / 1000;
      net->dWeightHO[o][h] = 0.0;
    }
    net->BiasO[o] = Random() / 1000;
    net->dBiasO[o] = 0.0;
  }
}


// Fonction sigmoid servant de fonction d'activation
double sigmoid(double x)
{
  double res = (1.0 / (1.0 + exp(-x)));
  //printf("%lf\n", x);
  //printf("%lf\n", res);
  return res;
}

// Fonction dérivé de la sigmoid pour propager vers l'arrière notre erreur
double dSigmoid(double z)   //derivative of the activating sigmoid function
{
    double res = z * (1 - z);
    return res;
}



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
  // Il faut calculer le taux d'erreur
  for(int o = 0; o < net->nbOutput; o++)
  {
    net->ErrorRate += 0.5 * ((net->Goal[p][o] - net->OutputO[o]) * ((net->Goal[p][o] - net->OutputO[o])));
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
  //rintf("%d\n", lmax);
  if(lmax < 26)
  {
    //printf("%d\n", lmax);
    net->act = (char) lmax + (int) 'A';
  }
  else
  {
    net->act = (char) lmax + (int) 'a' - 26;
    //printf("%d\n", lmax);
  }

  if(p < 26)
  {
    goal = (char) (p + (int) 'A');
  }
  else
  {
    goal = (char) (p + (int) 'a' - 26);
  }

  //printf("ok 5\n");
  if(epoch % 100 == 0)
  {
    printf("La réponse du réseau est : %c\n", net-> act);
    printf("La réponse attendu est :   %c\n\n", goal);
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
        net -> WeightHO[o][h] += (net->eta) * (net-> dOutputO[o]) * (net-> OutputH[h]) + (net->alpha) * (net-> dWeightHO[h][o]);
        net -> dWeightHO[o][h] = (net->eta) * (net->dOutputO[o]) * (net-> OutputH[h]);
      }
      //Update BiasO
      net -> BiasO[o] += (net -> eta) * (net -> dOutputO[o]);
    }


    ///Calculate Cost function for hidden neurons
    double sum;
  
    for (int o = 0; o < net -> nbOutput; o++)
    {
      sum = 0.0;
      for (int h = 0; h < net -> nbHidden; h++)
      {
        sum += (net -> WeightHO[o][h]) * (net->dOutputO[o]);
      }
      net -> dHiddenH[o] = sum * dSigmoid(net -> OutputH[o]);
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


void OCR()
{
  srand(time(NULL));

  int NbPattern = 52;
  int NbEpoch = 5000;

  struct Neural_Network *net = InitalizeNetwork();

  //LoadData(net);

  InitalizeValue(net);
  for (int epoch = 0; epoch <= NbEpoch; ++epoch)
  {
    if(epoch % 100 == 0)
    {
      printf("#########################\n");
      printf("Essai n°%d\n\n", epoch);
    }
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