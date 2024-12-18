#ifndef GENETICALGORITHM_GENETICALGORITHM_H
#define GENETICALGORITHM_GENETICALGORITHM_H

#include <string>
#include <vector>
#include <random>
#include "Chromosome.h"
#include "Population.h"

#include "../lib/json.hpp"

using json = nlohmann::json;

struct GeneticAlgorithmParams{
    const std::string sequence;
    const unsigned long populationSize;
    const unsigned long generations;
    const double elitePercent;
    const double crossoverPercent;
    const double mutationPercent;
};



class GeneticAlgorithm {

public:
    static GeneticAlgorithmParams params;
    GeneticAlgorithm(){
    }
    void run_serial();
    void run_parallel();
    void showTurnList();

    void resultAsJson();
    void resultToFile();

private:
    Population createBasePopulation();
    std::vector<Population> generations;

    bool keepGoing();
};


#endif //GENETICALGORITHM_GENETICALGORITHM_H
