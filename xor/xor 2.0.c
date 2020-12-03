
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
    double BiasO;
    //Output Hidden
    double *OutputH;
    //Output of output
    double OutputO;
    //Array of delta bias for hidden
    double *dBiasH;
    //delta bias for output
    double dBiasO;
    //delta weight
    double *dWeightIH;
    double *dWeightHO;
    //delta of output
    double dOutputO;
    //delta of hidden
    double *dHidden;
    
    double ErrorRate;
    double eta;
    double alpha;
};

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

static double sigmoid(double x)
{
  return(1.0/(1.0+exp(-x)));
}

static double dSigmoid(double z)   //derivative of the activating sigmoid function
{
    double res = z * (1 - z);
    return res;
}

struct Neural_Network InitalizeNetwork()
{
  struct Neural_Network net;
  net.nbInput = 2;
  net.nbHidden = 2;
  
  net.InputValue = malloc(sizeof(double)*4*2);
  net.Goal = malloc(sizeof(double)*4);
  
  net.WeightIH = malloc(sizeof(double)*net.nbInput*net.nbHidden);
  net.WeightHO = malloc(sizeof(double)*net.nbHidden);
  net.BiasH = malloc(sizeof(double)*net.nbHidden);
  net.BiasO = Random();
  net.OutputH = malloc(sizeof(double)*net.nbHidden);
  net.OutputO = 0;
  
  net.dBiasH = malloc(sizeof(double)*net.nbHidden);
  net.dBiasO = 0.0;
  net.dWeightIH = malloc(sizeof(double)*net.nbInput*net.nbHidden);
  net.dWeightHO = malloc(sizeof(double)*net.nbHidden);
  net.dOutputO = 0.0;  
  net.dHidden = malloc(sizeof(double)*net.nbHidden);
  
  net.ErrorRate = 0.0;
  net.eta = 0.5;
  net.alpha = 0.9;
  
  return(net);
}
void InitalizeValue(struct Neural_Network *net)
{
  
  *(net -> InputValue +0) = 0;
  *(net -> InputValue +1) = 0;
  *(net -> InputValue +2) = 0;
  *(net -> InputValue +3) = 1;
  *(net -> InputValue +4) = 1;
  *(net -> InputValue +5) = 0;
  *(net -> InputValue +6) = 1;
  *(net -> InputValue +7) = 1;
  
  *(net -> Goal +0) = 0; //0-0 => 0
  *(net -> Goal +1) = 1; //0-1 => 1
  *(net -> Goal +2) = 1; //1-0 => 1
  *(net -> Goal +3) = 0; //1-1 => 0


  for (int i = 0;i< net -> nbInput; i++)
  {
    for (int h = 0; h < net -> nbHidden; ++h)
    {
      *(net -> WeightIH + (h + i * net -> nbHidden)) = Random();
      *(net -> dWeightIH + (h + i * net -> nbHidden)) = 0.0;
    }
  }
  
  for (int h = 0; h < net -> nbHidden; ++h)
  {
    *(net -> WeightHO + h) = Random();
    *(net -> dWeightHO + h) = 0.0;
    *(net -> BiasH + h) = Random();
    *(net -> dBiasH + h) = 0.0;
  }
  net -> dBiasO= 0.0;
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

void XOR()//called function
{
  srand(time(NULL));
  
  int NbPattern = 4;
  int NbEpoch = 20000;

  struct Neural_Network net_1 = InitalizeNetwork();
  struct Neural_Network *net = &net_1;

  InitalizeValue(net);
  for (int epoch = 0; epoch <= NbEpoch; ++epoch)
  {
    net -> ErrorRate = 0.0;
    for (int p = 0; p < NbPattern; ++p)
    {
      ForwardPass(net,p,epoch);
      BackwardPass(net,p);
    }
    //print
    if (epoch % 1000 == 0)
    {
      printf("Epoch %-5d => ErrorRate = %f\n\n", epoch, net -> ErrorRate);
    }
  }
  free(net->OutputH);
  free(net->BiasH);
  free(net->WeightHO);
  free(net->InputValue);
  free(net->Goal);
  free(net->WeightIH);
  free(net->dBiasH);
  free(net->dWeightIH);
  free(net->dWeightHO);
  free(net->dHidden);
}

int main()
{
  XOR();
  return 1;
}