# GeneticAlgorithm
Parallelized Genetic Algorithm for Predicting Protein Folding in the 2D HP Model using OpenMP.

# Instructions
If you have cmake installed, do

make GeneticAlgorithm

Alternatively, you can compile it manually by running

g++ main.cpp algorithm/GeneticAlgorithm.cpp algorithm/Population.cpp algorithm/Chromosome.cpp -o GeneticAlgorithm -fopenmp

To run the program, do

./GeneticAlgorithm