/**
 * 
 * Simulation of 'geometric' security attack on the kkk protocol.
 * 
 * Author: Maduka Attamah
 * Edited By : Mousa Shaikh-Soltan
 * Copyright 2016 Maduka Attamah
 */

#include "3kprotocol.h"

#define EPOCH_LIMIT 1500000
#define SYNCHRONISATION_THRESHOLD 50

int main() {
    int k = 0;
    int n = 0;
    int l = 0;


    printf("Enter the value of k:");
    scanf("%d", &k);
    printf("Enter the value of n:");
    scanf("%d", &n);
    printf("Enter the value of l:");
    scanf("%d", &l);

    struct NeuralNetwork neuralNetA = constructNeuralNetwork(k, n, l);
    struct NeuralNetwork neuralNetB = constructNeuralNetwork(k, n, l);
    struct NeuralNetwork neuralNetC = constructNeuralNetwork(k, n, l);

    srand(time(NULL));
    int** inputs = getRandomInputs(k, n);
    printf("\n==============BEFORE PROTOCOL RUN=======================\n");
    printNetworkWeights(neuralNetA, k, n);
    printf("\n=====================================\n");
    printNetworkWeights(neuralNetB, k, n);
    printf("\n=====================================\n");
    printNetworkWeights(neuralNetC, k, n);
    
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
     bool status = runGeometricAttackKKKProtocol(neuralNetA, neuralNetB, neuralNetC, inputs, k, n, l, SYNCHRONISATION_THRESHOLD, EPOCH_LIMIT, &epoch);

    printf("\n==============AFTER PROTOCOL RUN=======================\n");
    if (status == true) {
        printf("The networks are now synchronised after %d epochs!\n", epoch);
        printNetworkWeights(neuralNetA, k, n);
        printf("\n=====================================\n");
        printNetworkWeights(neuralNetB, k, n);
        printf("\n=====================================\n");
        printNetworkWeights(neuralNetC, k, n);
    } else {
        printNetworkWeights(neuralNetA, k, n);
        printf("\n=====================================\n");
        printNetworkWeights(neuralNetB, k, n);
        printf("\n=====================================\n");
        printNetworkWeights(neuralNetC, k, n);
        printf("Networks are unsynchronised after %d epochs.", EPOCH_LIMIT);
    }

    freeMemoryForNetwork(neuralNetA, k, n);
    freeMemoryForNetwork(neuralNetB, k, n);
    freeMemoryForNetwork(neuralNetC, k, n);
    free(inputs);
    return 0;
}











