/**
 * 
 * Simulation of 'geometric' security attack on the kkk protocol.
 * 
 * Author: Maduka Attamah
 * Edited By : Mousa Shaikh-Soltan
 * Copyright 2016 Maduka Attamah
 */


//tasks i need to stop a single attacker is successfull 
#include<stdlib.h>
#include "3kprotocol.h"
#include <mpi.h>
#include<time.h>
#include<unistd.h>
#define EPOCH_LIMIT 10000000
#define SYNCHRONISATION_THRESHOLD 50




int main() {
    

    MPI_Init(NULL,NULL);



    int comm_sz;
    int rank ;    
   
    MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);


      // create new seed for different networks 
    
    int k = 0;
    int n = 0;
    int l = 0;
    int nAttackers=0;
 
  
  if(rank==0)
    {
fflush(stdout);
    printf("Enter the value of k:");
   fflush(stdout);
   scanf("%d", &k);
    printf("Enter the value of n:");
    fflush(stdout) ;
    scanf("%d", &n);
    fflush(stdout) ;

    printf("Enter the value of l:");
    fflush(stdout);
    scanf("%d", &l);
    printf("Enter number of Attackers Per Node:");
    fflush(stdout);
    scanf("%d",&nAttackers); 
	}


    //   broadcast the inputs to create attackers and A,B 
    
    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&k,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&l,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&nAttackers,1,MPI_INT,0,MPI_COMM_WORLD);
 
   struct NeuralNetwork* neuralNetC = malloc((sizeof(struct NeuralNetwork))*nAttackers); 
 
    struct NeuralNetwork neuralNetA;
    struct NeuralNetwork neuralNetB;
    


        neuralNetA = constructNeuralNetwork(k, n, l);


        int** inputs = getRandomInputs(k, n);


// start from a different seed ..
srand(100);
        
        neuralNetB = constructNeuralNetwork(k, n, l); 


//srand(time(NULL)+rank);




    // create set of attackers on each on each node 
   for(int i = 0; i<nAttackers; i++ )
   {
   
   // need to fix random number generator
        
    srand(time(NULL)+rank+i);

    neuralNetC[i] = constructNeuralNetwork(k,n,l); 
  
   }
    //create a nAttackers at each node     

 

    MPI_Barrier(MPI_COMM_WORLD);

    sleep(rank*0.5);
   
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
     
   
     
     double success_count = 0;

     

     for(int i = 0; i<nAttackers; i++)
     {
    
    
    // need to check bool condition for each attacker once one attacker successful then stop .

     int epoch = 0;
   
     srand(time(NULL));
// each mpiprocess has the same seed for the round inputs 

     struct NeuralNetwork AbeforeAttack = cloneNeuralNetwork(k,n, neuralNetA);
     struct NeuralNetwork BbeforeAttack = cloneNeuralNetwork(k,n, neuralNetB);
     bool status = runGeometricAttackKKKProtocol(AbeforeAttack, BbeforeAttack, neuralNetC[i], inputs, k, n, l, SYNCHRONISATION_THRESHOLD, EPOCH_LIMIT, &epoch);
  
  
    // neuralNetA = AbeforeAttack;
    // neuralNetB = BbeforeAttack;
    

    if (status == true)
    {
        success_count = success_count +1.0;
        printf("local success [%lf ]    rank [ %d] \n", success_count, rank);
    }
     
     
    printf("\n==============AFTER PROTOCOL RUN====================RANK[%d]===\n",rank);
    
    if (status == true) {
        
        printf("The networks are now synchronised after %d epochs!\n", epoch);
        
          printf("\n==============After PROTOCOL RUN A=====================RANK[%d]===\n",rank);       

        printNetworkWeights(AbeforeAttack, k, n);
        
        printf("\n=====================================\n");
        
        printf("\n==============After PROTOCOL RUN B=====================RANK[%d]===\n",rank);       

        printNetworkWeights(BbeforeAttack, k, n);
        
        printf("\n=====================================\n");
        
              
            
    printf("\n========================Attacker%d AFTER Run===========Rank%d\n",i,rank);
            

        printNetworkWeights(neuralNetC[i], k, n);
               

    printf("\n========================Attacker%d===========Rank%d\n",i,rank);

        

        

        printf("\n=====================================\n");
          

    } else {
       
       printf("\n==============After PROTOCOL RUN A fail=====================RANK[%d]===\n",rank);       

        printNetworkWeights(AbeforeAttack, k, n);
            
        printf("\n==============After PROTOCOL RUN B fail=====================RANK[%d]===\n",rank);       

        printNetworkWeights(BbeforeAttack, k, n);
            
            
        printf("\n==============After PROTOCOL RUN C fail  attacl[%d]=====================RANK[%d]===\n",i,rank);       

        printNetworkWeights(neuralNetC[i], k, n);

        printf("Networks are unsynchronised after %d epochs.", EPOCH_LIMIT);
       

    }
    MPI_Barrier(MPI_COMM_WORLD);
     
    freeMemoryForNetwork(AbeforeAttack,k,n);
    freeMemoryForNetwork(BbeforeAttack,k,n);

     }

          double global_count = 0 ;

   
     MPI_Reduce(&success_count,&global_count,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD ) ;
   
       
       if (rank==0)
       {
           
       printf("The Success rate  =  [%lf]\n", 100.*global_count/(double)(nAttackers*comm_sz));
       fflush(stdout);

     }


    freeMemoryForNetwork(neuralNetA, k, n);
    freeMemoryForNetwork(neuralNetB, k, n);
    
    for(int i = 0; i < nAttackers; i++)
    {
    freeMemoryForNetwork(neuralNetC[i],k, n);
    } 
    
    free(inputs);


    
    


MPI_Finalize();

return 0;
}











