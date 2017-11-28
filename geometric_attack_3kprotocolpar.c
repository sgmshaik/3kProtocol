/**
 * 
 * Simulation of 'geometric' security attack on the kkk protocol.
 * 
 * Author: Maduka Attamah
 * Edited By : Mousa Shaikh-Soltan
 * Copyright 2016 Maduka Attamah
 */

#include "3kprotocol.h"
#include <mpi.h>
#define EPOCH_LIMIT 1500000
#define SYNCHRONISATION_THRESHOLD 50

void freeAttackerMemory(struct NeuralNetwork *attackers, int numAttackers, int k, int n)
{
    for(int i = 0; i < numAttackers; i++)
    {
    freeMemoryForNetwork(attackers[i], k, n);
    }
}


int main() {
    
    
    int comm_sz;
    int rank ;    
   
    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int k = 0;
    int n = 0;
    int l = 0;
    int nAttackers=0;
 
  if(rank==0)
    {
    printf("Enter the value of k:");
    scanf("%d", &k);
    printf("Enter the value of n:");
    scanf("%d", &n);
    printf("Enter the value of l:");
    scanf("%d", &l);
    printf("Enter number of Attackers Per Node:");
    scanf("%d",&nAttackers);
  
  
   }


    //   broadcast the inputs to create attackers and A,B 
    
    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&k,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&l,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&nAttackers,1,MPI_INT,0,MPI_COMM_WORLD);
    struct NeuralNetwork* neuralNetC = malloc((sizeof(struct NeuralNetwork))*nAttackers); 

    struct NeuralNetwork neuralNetA = constructNeuralNetwork(k, n, l);
    struct NeuralNetwork neuralNetB = constructNeuralNetwork(k, n, l); 
    
    srand(time(NULL)+rank);  // create new seed for different networks 

    neuralNetC[rank] = constructNeuralNetwork(k,n,l); 

    //create a nAttackers at each node     
    
    neuralNetC[rank] = constructNeuralNetwork(k, n, l);


    int** inputs = getRandomInputs(k, n);
    printf("\n==============BEFORE PROTOCOL RUN=====================RANK[%d]===\n",rank);
    printNetworkWeights(neuralNetA, k, n);
    printf("\n=====================================\n");
    printNetworkWeights(neuralNetB, k, n);
    printf("\n=====================================\n");
  
    printf("\n=====================================\n");
    printNetworkWeights(neuralNetC[rank], k, n);

     /*
     * To run the KKK Protocol normally (without any attacks), uncomment the call to function runKKKProtocol(...) below, 
     * and comment out the call to function runGeometricAttackKKKProtocol(...) which follows.  To run the attack in 'offline'
     * mode the call to runGeometricAttackKKKProtocol(...) can come after the runKKKProtocol(...), but then you ought to modify 
     * (or create another version of) the function runGeometricAttackKKKProtocol(...) to not update the weights of
     * neuralNetA and neuralNetB during the attack.
     */

    int epoch = 0;
   
   // runKKKProtocol(struct NeuralNetwork neuralNetA, struct NeuralNetwork neuralNetB, int** inputs, int k, int n, int l, int syncThreshold, int epochLimit)
     //bool status = runKKKProtocol(neuralNetA, neuralNetB, inputs, k, n, l,SYNCHRONISATION_THRESHOLD, EPOCH_LIMIT);
     bool status = runGeometricAttackKKKProtocol(neuralNetA, neuralNetB, neuralNetC[rank], inputs, k, n, l, SYNCHRONISATION_THRESHOLD, EPOCH_LIMIT, &epoch);


    MPI_Barrier(MPI_COMM_WORLD);
    printf("\n==============AFTER PROTOCOL RUN====================RANK[%d]===\n",rank);
    if (status == true) {
        printf("The networks are now synchronised after %d epochs!\n", epoch);
        printNetworkWeights(neuralNetA, k, n);
        printf("\n=====================================\n");
        printNetworkWeights(neuralNetB, k, n);
        printf("\n=====================================\n");
        printNetworkWeights(neuralNetC[rank], k, n);
        printf("\n=====================================\n");
    } else {
        printNetworkWeights(neuralNetA, k, n);
        printNetworkWeights(neuralNetB, k, n);
        printNetworkWeights(neuralNetC[rank], k, n);
        printf("Networks are unsynchronised after %d epochs.", EPOCH_LIMIT);
    }

    freeMemoryForNetwork(neuralNetA, k, n);
    freeMemoryForNetwork(neuralNetB, k, n);
    freeMemoryForNetwork(neuralNetC[rank],k, n);
    free(inputs);

MPI_Finalize;
return 0;
}











