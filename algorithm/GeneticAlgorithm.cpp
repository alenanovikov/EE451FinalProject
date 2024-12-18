#include <iostream>
#include <fstream>
#include "GeneticAlgorithm.h"

// Serial version without OpenMP 
void GeneticAlgorithm::run_serial() {


    Population p = createBasePopulation();
    p.process_serial();
    generations.push_back(p);

    double fitness = 0.001;
    int generation = 0;

    while(generation < params.generations){
        //p.printChromos();
        generation++;
        p = p.tournament_selection(generation);
        p.crossover_selection_serial(params.crossoverPercent);
        p.mutation_serial(params.mutationPercent);
        p.process_serial();

        p.calcDiversity_serial();

        generations.push_back(p);
     }

    generations.back().printBestCandidate();
    resultAsJson();
}

// Parallel version using OpenMP
void GeneticAlgorithm::run_parallel() {
    // create random base popuolation
    Population p = createBasePopulation();
    // sort foldings according to their firness score
    p.process_parallel();
    // add this configuration
    generations.push_back(p);

    double fitness = 0.001;
    int generation = 0;

    while(generation < params.generations){
        generation++;
        p = p.tournament_selection(generation); // shuffle chromosomes
        p.crossover_selection_parallel(params.crossoverPercent); // crossover
        p.mutation_parallel(params.mutationPercent); // mutate random chromosomes
        p.process_parallel(); // recalculate fitness

        p.calcDiversity_parallel();

        generations.push_back(p);
     }

    generations.back().printBestCandidate();
    resultAsJson();
}

Population GeneticAlgorithm::createBasePopulation() {
    Population base(params.populationSize);
    return base;
}

bool GeneticAlgorithm::keepGoing() {
    return false;
}

void GeneticAlgorithm::resultAsJson() {
    json results;
    json min;
    json max;
    json average;
    json diversity;

    int generation = 0;
    for (auto p : generations){
        min.push_back(json::object({{"x", generation},{"y", p.minFitness}}));
        max.push_back(json::object({{"x", generation},{"y", p.maxFitness}}));
        average.push_back(json::object({{"x", generation},{"y", p.averageFitness}}));
        diversity.push_back(json::object({{"x", generation},{"y", p.diversity*100}}));
        generation++;
        //results.push_back(p.toJson());
    }
    results["min"] =  min;
    results["max"] =  max;
    results["average"] =  average;
    results["diversity"] =  diversity;

    std::ofstream myfile ("C:\\Users\\nao92\\cs104\\ee451\\GeneticAlgorithm\\viewer\\results.json");
    if (myfile.is_open())
    {
        myfile << "data_test = " << results;
        myfile.close();
    }

}

void GeneticAlgorithm::resultToFile() {
    std::ofstream myfile ("C:\\Users\\nao92\\cs104\\ee451\\GeneticAlgorithm\\viewer\\results.tsv");
    if (myfile.is_open())
    {
        myfile << "Generation" << "\t" << "max fitness" << "\t" << "min fitness" << "\t" << "average fitness" << "\n";
        for (auto p : generations){
            myfile << p.minFitness << "\t" << p.maxFitness << "\t" << p.averageFitness << "\n";
        };
        myfile.close();
    }
}












