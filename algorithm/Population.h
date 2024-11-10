//
// Created by alican on 10.05.2016.
//
#ifndef GENETICALGORITHM_POPULATION_H
#define GENETICALGORITHM_POPULATION_H

#include "Chromosome.h"
#include <vector>
#include <iostream>
#include "../lib/json.hpp"

using json = nlohmann::json;

class Population {
public:

    Population() { }

    Population(unsigned long size) {

        for (int i = 0; i < size; i++){
            Chromosome chrom;
            chrom.createRandomTurnList();
            chrom.process();
            chrom.setId();
            chromosomes.push_back(std::move(chrom));
        }
    }

    Population(std::vector<Chromosome> selection){
        chromosomes = selection;
    }

    Population selection(int);
    void mutation_serial(double);
    void mutation_parallel(double);
    void crossover_selection_serial(double crossover_rate);
    void crossover_selection_parallel(double crossover_rate);
    void process_serial(); // serial implementation
    void process_parallel(); // parallel implementation
    void printChromos();
    void printBestCandidate();

    void calcDiversity_serial();
    void calcDiversity_parallel();

    double minFitness;
    double maxFitness;
    double averageFitness;

    float diversity;


    json toJson();

    Population tournament_selection(int generation);

private:
    std::vector<Chromosome> chromosomes;

};

#endif //GENETICALGORITHM_POPULATION_H
