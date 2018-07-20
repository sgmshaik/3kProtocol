/**
 * 
 * Simulation of 'geometric' security attack on the kkk protocol.
 * 
 * Author: Maduka Attamah
 * Edited and parralelised using MPI : Mousa Shaikh-Soltan
 * Copyright 2016 Maduka Attamah
 * Copyright 2017 Mousa Shaikh-Soltan
 *
 */

//tasks i need to stop a single attacker is successfull
#include <stdlib.h>
#include "3kprotocol.h"
#include <mpi.h>
#include <time.h>
#include <unistd.h>
#define EPOCH_LIMIT 12500000
#define SYNCHRONISATION_THRESHOLD 25

int main(int argc, char *argv[])
{
    MPI_Init(NULL, NULL);

    int comm_sz;
    int rank;

    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // create new seed for different networks

    int k = 0;
    int n = 0;
    int l = 0;
    
 
    
    // This is number of Attacker on each rank
    int nAttackers = 0;
   //set to 1 is if you want to enter the inputs your self
    int userinput = 1;
   //set @networkprint to 1 and @rankprint to the desired rank you want to see the weights for 
   //usefull for testing 
    int networkprint = 0; 
    int rankprint = 0;
    
    //setup on rank 0 since only rank0 has access to stdin 


    if (rank == 0)
    {
        //use user input
        if (userinput == 1)
        {
            fflush(stdout);
            printf("Enter the value of k:");
            fflush(stdout);
            scanf("%d", &k);
            printf("Enter the value of n:");
            fflush(stdout);
            scanf("%d", &n);
            printf("Enter the value of l:");
            fflush(stdout);
            scanf("%d", &l);
            printf("Enter number of Attackers Per Node:");
            fflush(stdout);
            scanf("%d", &nAttackers);
            printf("Do you want to print network weights enter 0 for no 1 for yes:");
            fflush(stdout);
            scanf("%d", &networkprint);
           printf("Which ranks weight do you want to print enter an integer 0-(comm_sz-1):");
            fflush(stdout);
            scanf("%d",&rankprint);
            fflush(stdout);
      
        }
        else
        {
            //for bash script 
            k = atoi(argv[1]);
            n = atoi(argv[2]);
            l = atoi(argv[3]);
            nAttackers = atoi(argv[4]);
        }
    }

    //   broadcast the inputs to create attackers and A,B

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&l, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nAttackers, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rankprint, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&networkprint, 1, MPI_INT, 0, MPI_COMM_WORLD);

    struct NeuralNetwork *neuralNetC = malloc((sizeof(struct NeuralNetwork)) * nAttackers);

    struct NeuralNetwork neuralNetA;
    struct NeuralNetwork neuralNetB;

    neuralNetA = constructNeuralNetwork(k, n, l);

    int **inputs = getRandomInputs(k, n);

    neuralNetB = constructNeuralNetwork(k, n, l);

    // start from a different seed for each attacker at new rank..
    //srank is called once per rank
    //shifted by 100*rank to create a sigificant difference between ranks. 

    srand((unsigned)time(NULL) + 100 * (rank));

    // create set of attackers on each on each node
    for (int i = 0; i < nAttackers; i++)
    {

        // need to fix random number generator

        neuralNetC[i] = constructNeuralNetwork(k, n, l);
    }
    //create a nAttackers at each node starting with random starting weights

    if (networkprint == 1 && rankprint==rank)
    {
        printf("\n==============BEFORE PROTOCOL RUN A=====================RANK[%d]===\n", rank);

        printNetworkWeights(neuralNetA, k, n);

        printf("\n==============BEFORE PROTOCOL RUN B =====================RANK[%d]===\n", rank);

        printNetworkWeights(neuralNetB, k, n);

        printf("\n=====================================\n");

        for (int i = 0; i < nAttackers; i++)
        {

            printf("\n========================Attacker%d===========Rank%d\n", i, rank);

            printNetworkWeights(neuralNetC[i], k, n);

            printf("\n========================Attacker%d===========Rank%d\n", i, rank);
        }
    }

    double success_count = 0;

    int **tempInputs = malloc(sizeof(int *) * k);
    //tempInputs to original value for inputs
    for (int i = 0; i < k; i++)
    {
        tempInputs[i] = malloc(sizeof(int) * n);
        for (int j = 0; j < n; j++)
        {
            tempInputs[i][j] = inputs[i][j];
        }
    }

        


    //epoch value for each attack tell us number of rounds it took if succeed or fail (epochmax)
    int localepoch =0 ; 
    
    struct NeuralNetwork AbeforeAttack =  cloneNeuralNetwork(k,  n,  neuralNetA) ;
    struct NeuralNetwork BbeforeAttack =  cloneNeuralNetwork( k,  n, neuralNetB) ;


    for (int i = 0; i < nAttackers; i++)
    {

        int epoch = 0;

        // each rank has the same seed for the A, B and inputs because
        // these are created before the call to the srank(time(NULL) + 100*rank)
        // This was the source of error originally because in runGeometricAttack
        // they are updated so vary for different attacker this is not what we want
        // to preven this i create a copy of netA , netB and inputs so at round 0
        // each attacker starts with the same A,B and inputs without creating copies
        // runGeo updates these and gives a different value of A,B and inputs for
        // each attacker before there attacks have started

       

        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < n; j++)
            {
                inputs[i][j] = tempInputs[i][j];
            }
        }

        copyNeuralNetwork(k, n,  &neuralNetA, &AbeforeAttack); 
        copyNeuralNetwork(k, n, &neuralNetB, &BbeforeAttack); 



        bool status = runGeometricAttackKKKProtocol(AbeforeAttack, BbeforeAttack, neuralNetC[i], inputs, k, n, l, SYNCHRONISATION_THRESHOLD, EPOCH_LIMIT, &epoch);

        if (status == true)
        {
            success_count = success_count + 1.0;
        }

        if (networkprint == 1 && rankprint == rank)
        {
            printf("\n==============AFTER PROTOCOL RUN====================RANK[%d]===\n", rank);

            if (status == true)
            {

                printf("The networks are now synchronised after %d epochs!\n", epoch);
                fflush(stdout);

                printf("\n==============After PROTOCOL RUN A=====================RANK[%d]===\n", rank);

                fflush(stdout);

                printNetworkWeights(AbeforeAttack, k, n);
                fflush(stdout);

                printf("\n=====================================\n");
                fflush(stdout);

                printf("\n==============After PROTOCOL RUN B=====================RANK[%d]===\n", rank);
                fflush(stdout);

                printNetworkWeights(BbeforeAttack, k, n);
                fflush(stdout);

                printf("\n=====================================\n");
                fflush(stdout);

                printf("\n========================Attacker%d AFTER Run===========Rank%d\n", i, rank);
                fflush(stdout);

                printNetworkWeights(neuralNetC[i], k, n);
                fflush(stdout);

                printf("\n========================Attacker%d===========Rank%d\n", i, rank);
                fflush(stdout);

                printf("\n=====================================\n");
                fflush(stdout);
            }
            else
            {

                printf("\n==============After PROTOCOL RUN A fail=====================RANK[%d]===\n", rank);
                fflush(stdout);

                printNetworkWeights(AbeforeAttack, k, n);
                fflush(stdout);

                printf("\n==============After PROTOCOL RUN B fail=====================RANK[%d]===\n", rank);
                fflush(stdout);

                printNetworkWeights(BbeforeAttack, k, n);
                fflush(stdout);

                printf("\n==============After PROTOCOL RUN C fail  attacl[%d]=====================RANK[%d]===\n", i, rank);
                fflush(stdout);

                printNetworkWeights(neuralNetC[i], k, n);
                fflush(stdout);

                printf("Attack Failed after %d epochs.", EPOCH_LIMIT);
                fflush(stdout);
            }
        }

        

        
        //set count the epochtotal for each rank
        localepoch += epoch;
    }

        freeMemoryForNetwork(AbeforeAttack, k, n);
        freeMemoryForNetwork(BbeforeAttack, k, n);

    double global_count = 0;
    int global_epoch = 0;
    //MPI_Reduce is blocking so no MPI_Barrier required
    //Reduces all the local atacker successes and reduces them to global count for rank 0;
    MPI_Reduce(&success_count, &global_count, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&localepoch, &global_epoch, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


    if (rank == 0)
    {
        //print final output which is piped to a file in the terminal (did like this because of chadwick bash scripts)
        printf("%d ,  %d  , %d  , %d  ,  %lf , %lf , %d \n", k, n, l, nAttackers * comm_sz, 100. * global_count / (double)(nAttackers * comm_sz), global_count, global_epoch);
        fflush(stdout);
    }

    freeMemoryForNetwork(neuralNetA, k, n);
    freeMemoryForNetwork(neuralNetB, k, n);

    for (int i = 0; i < nAttackers; i++)
    {
        freeMemoryForNetwork(neuralNetC[i], k, n);
    }

    freeinputs(inputs,k);
    freeinputs(tempInputs,k);

    MPI_Finalize();

    return 0;
}
