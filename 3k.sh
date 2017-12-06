#!/bin/bash
  
 mpicc -std=c99 -o par1.out  geometric_attack_3kprotocolpar.c -lm
  
for((k = 1;k<5;k++ ))
do
 	for((n= 1;n<5;n++))
	do
 		for((l= 1;l< 5;l++))
		do
			for((na= 1;na< 3;na++))
			do
				mpiexec -n 4 ./par1.out $k $n $l $na
			done
		done
	done
done
