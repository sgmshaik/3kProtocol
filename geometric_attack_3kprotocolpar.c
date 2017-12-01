/**
 * 
 * Simulation of 'geometric' security attack on the kkk protocol.
 * 
 * Author: Maduka Attamah
 * Edited By : Mousa Shaikh-Soltan
 * Copyright 2016 Maduka Attamah
 */


//tasks i need to stop a single attacker is successfull 

#include "3kprotocol.h"
#include <mpi.h>
#define EPOCH_LIMIT 1500000
#define SYNCHRONISATION_THRESHOLD 500




int main() {
    

    MPI_Init(NULL,NULL);


    int comm_sz;
    int rank ;    
   
    MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int k = 0;
    int n = 0;
    int l = 0;
    int nAttackers=0;
 
  if(rank==0)
    {
    printf("Enter the value of k:");
   fflush(stdout);
   scanf("%d", &k);
    printf("Enter the value of n:");
    fflush(stdout) ;
    scanf("%d", &n);
    printf("Enter the value of l:");
    fflush(stdout);
    scanf("%d", &l);
    printf("Enter number of Attackers Per Node:");
    fflush(stdout);
    scanf("%d",&nAttackers); 
   }

 MPI_Barrier(MPI_COMM_WORLD);

    //   broadcast the inputs to create attackers and A,B 
    
    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&k,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&l,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&nAttackers,1,MPI_INT,0,MPI_COMM_WORLD);
    struct NeuralNetwork* neuralNetC = malloc((sizeof(struct NeuralNetwork))*nAttackers); 

          

    struct NeuralNetwork neuralNetA = constructNeuralNetwork(k, n, l);
       

    struct NeuralNetwork neuralNetB = constructNeuralNetwork(k, n, l); 
                


    // create set of attackers on each on each node 
   for(int i = 0; i<nAttackers; i++ )
   {
   
   // need to fix random number generator
    //srand((unsigned)time(NULL)+rank+i);  // create new seed for different networks 
        
    

    neuralNetC[i] = constructNeuralNetwork(k,n,l); 
  
   }
    //create a nAttackers at each node     
    

    


    int** inputs = getRandomInputs(k, n);

    for(int i = 0;i<comm_sz;i++)
    {
MPI_Barrier(MPI_COMM_WORLD);

if(rank==i){
    printf("\n==============BEFORE PROTOCOL RUN=====================RANK[%d]===\n",rank);      

      printf("\n==============BEFORE PROTOCOL RUN A=====================RANK[%d]===\n",rank);      

    printNetworkWeights(neuralNetA, k, n);
    
    printf("\n=====================================\n");
    
     printf("\n==============BEFORE PROTOCOL RUN B =====================RANK[%d]===\n",rank);       

    printNetworkWeights(neuralNetB, k, n);
    
    printf("\n=====================================\n");
    
    printf("\n=====================================\n");
    
    for(int i = 0; i < nAttackers; i++)
    {
    
    printf("\n========================Attacker%d===========Rank%d\n",i,rank);
       

    printNetworkWeights(neuralNetC[i], k, n);
       

    printf("\n========================Attacker%d===========Rank%d\n",i,rank);
    
    }
     
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
     
     for(int i = 0; i<nAttackers; i++)
     {
    
    
    // need to check bool condition for each attacker once one attacker successful then stop .


     bool status = runGeometricAttackKKKProtocol(neuralNetA, neuralNetB, neuralNetC[i], inputs, k, n, l, SYNCHRONISATION_THRESHOLD, EPOCH_LIMIT, &epoch);


    
    printf("\n==============AFTER PROTOCOL RUN====================RANK[%d]===\n",rank);
    if (status == true) {
        
        printf("The networks are now synchronised after %d epochs!\n", epoch);
        
          printf("\n==============After PROTOCOL RUN A=====================RANK[%d]===\n",rank);       

        printNetworkWeights(neuralNetA, k, n);
        
        printf("\n=====================================\n");
        
        printf("\n==============After PROTOCOL RUN B=====================RANK[%d]===\n",rank);       

        printNetworkWeights(neuralNetB, k, n);
        
        printf("\n=====================================\n");
        
       
       // for(int i = 0;i<nAttackers;i++)
        //{       
            
    printf("\n========================Attacker%d AFTER Run===========Rank%d\n",i,rank);
            

        printNetworkWeights(neuralNetC[i], k, n);
               

    printf("\n========================Attacker%d===========Rank%d\n",i,rank);

        //}

        

        printf("\n=====================================\n");
          

    } else {
       
                  printf("\n==============After PROTOCOL RUN A fail=====================RANK[%d]===\n",rank);       

        printNetworkWeights(neuralNetA, k, n);
            
                  printf("\n==============After PROTOCOL RUN B fail=====================RANK[%d]===\n",rank);       

        printNetworkWeights(neuralNetB, k, n);
       
       // for(int i = 0;i<nAttackers;i++)
        //{       
            
        printf("\n==============After PROTOCOL RUN C fail  attacl[%d]=====================RANK[%d]===\n",i,rank);       

        printNetworkWeights(neuralNetC[i], k, n);
                   

        //}
       

        printf("Networks are unsynchronised after %d epochs.", EPOCH_LIMIT);
       

    }
     
}
freeMemoryForNetwork(neuralNetA, k, n);
    freeMemoryForNetwork(neuralNetB, k, n);
    
    for(int i = 0; i < nAttackers; i++)
    {
    freeMemoryForNetwork(neuralNetC[i],k, n);
    } 
    free(inputs);

}
    
    }


MPI_Finalize();

return 0;
}











