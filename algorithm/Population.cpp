//
// Created by alican on 10.05.2016.
//

#include <iostream>
#include <functional>
#include "Population.h"
#include <algorithm>
#include <omp.h> // Include OpenMP header



Population Population::selection(int generation) {

    std::default_random_engine generator;

    float ex = (float) (0.5 + (generation / 300.0));
    std::exponential_distribution<double> distribution(ex);

    std::vector<Chromosome> new_chromosomes;

    for(int i = 0; i < chromosomes.size();){
        double number = distribution(generator);
        if (number<1.0){
            i++;
            int position = int(chromosomes.size()*number);
            new_chromosomes.push_back(chromosomes.at(position));
        }
    }

    return Population(new_chromosomes);
}


Population Population::tournament_selection(int generation) {

    std::vector<Chromosome> new_chromosomes;


    //Fisher-Yates shuffle
    int t_rounds = (int) chromosomes.size();
    //std::cout << crossover_count << std::endl;

    long left = std::distance(chromosomes.begin(), chromosomes.end());
    auto current = chromosomes.begin();

    while (t_rounds) {
        auto rI1 = current;
        auto rI2 = current;
        std::advance(rI1, rand() % left);
        std::advance(rI2, rand() % left);
        //std::swap(*current, *r);
        Chromosome &ch1 = (*(rI1));
        Chromosome &ch2 = (*(rI2));

        Chromosome winner = (ch1.getFitness() > ch2.getFitness()) ? ch1 : ch2;
        new_chromosomes.push_back(std::move(winner));

        --t_rounds;
    }
    return Population(new_chromosomes);

 }


/* Serial version */
void Population::crossover_selection_serial(double crossover_rate) {

    // 0.25 der chromosome.
    //Fisher-Yates shuffle
    int crossover_count = (int) (chromosomes.size() * crossover_rate );
    //std::cout << crossover_count << std::endl;

    long left = std::distance(chromosomes.begin(), chromosomes.end());
    auto current = chromosomes.begin();

    while (crossover_count) {
        auto r = current;
        std::advance(r, rand()%left);
        //std::swap(*current, *r);
        Chromosome& ch1 = (*(current));
        Chromosome& ch2 = (*(r));

        if(ch1.id != ch2.id){
            ch1.crossover(ch2);
            --crossover_count;
        }
        ++current;
        --left;
    }

}
/* Parallel version */
void Population::crossover_selection_parallel(double crossover_rate) {
    int num_chromosomes = chromosomes.size();
    int crossover_count = static_cast<int>(num_chromosomes * crossover_rate);

    // Ensure crossover_count is even and does not exceed half of the population
    crossover_count = std::min(crossover_count - (crossover_count % 2), (num_chromosomes / 2) * 2);

    // Shuffle the chromosomes
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(chromosomes.begin(), chromosomes.end(), rng);

    // Perform crossover on the first crossover_count chromosomes
    #pragma omp parallel for
    for (int i = 0; i < crossover_count; i += 2) {
        Chromosome& ch1 = chromosomes[i];
        Chromosome& ch2 = chromosomes[i + 1];
        ch1.crossover(ch2);
    }
}

void Population::mutation_serial(double mutation_rate) {
    /*
     *   Wahl der Mutationen innerhalb der Population
     *
     */
    double mutation_count = std::ceil((chromosomes.size() * mutation_rate ));
    //double mutation_count = chromosomes.size();

    long size = chromosomes.size();
    auto current = chromosomes.begin();

    while (mutation_count--) {
        auto r = current;
        std::advance(r, rand()%size);
        (*(r)).mutate();
    }


}

void Population::mutation_parallel(double mutation_rate) {
    /*
     *   Wahl der Mutationen innerhalb der Population
     *
     */
    double mutation_count = std::ceil((chromosomes.size() * mutation_rate ));
    //double mutation_count = chromosomes.size();

    long size = chromosomes.size();
    auto current = chromosomes.begin();
    #pragma omp parallel for
    for (int i=0; i<(int)mutation_count; i++) {
        auto r = current;
        std::advance(r, rand()%size);
        (*(r)).mutate();
    }


}

void Population::printChromos() {


    for (auto chromo : chromosomes){
        std::cout << chromo.id << ": ";
        chromo.printTurns();
    }
    std::cout << chromosomes.size()<< std::endl;
}

void Population::process_serial() {

    double totalFitness = 0;
    // re-calculate changed chromosomes
    for (Chromosome& chromo : chromosomes){
        chromo.process();
        totalFitness += chromo.getFitness();
    }
    // sort list by fitness
    std::sort(chromosomes.rbegin(), chromosomes.rend());
    // calculate min/max fitness of population

    minFitness = chromosomes.back().getFitness();
    maxFitness = chromosomes.front().getFitness();
    averageFitness = totalFitness/chromosomes.size();
}

void Population::process_parallel() {
    double totalFitness = 0;
    double minFitnessLocal = std::numeric_limits<double>::max();
    double maxFitnessLocal = std::numeric_limits<double>::lowest();

    // Recalculate changed chromosomes
    #pragma omp parallel for reduction(+:totalFitness) reduction(min:minFitnessLocal) reduction(max:maxFitnessLocal)
    for (int i = 0; i < chromosomes.size(); ++i) {
        Chromosome& chromo = chromosomes[i];
        chromo.process();

        double fitness = chromo.getFitness();
        totalFitness += fitness;

        // Update local min and max fitness
        if (fitness < minFitnessLocal) {
            minFitnessLocal = fitness;
        }
        if (fitness > maxFitnessLocal) {
            maxFitnessLocal = fitness;
        }
    }

    // Update class members with the results
    minFitness = minFitnessLocal;
    maxFitness = maxFitnessLocal;
    averageFitness = totalFitness / chromosomes.size();

    // Sort chromosomes by fitness
    std::sort(chromosomes.begin(), chromosomes.end(),
              [](const Chromosome& a, const Chromosome& b) {
                  return a.getFitness() > b.getFitness();
              });
}

void Population::printBestCandidate() {
    std::sort(chromosomes.rbegin(), chromosomes.rend());
    chromosomes.front().printInfo();
}

json Population::toJson() {
    json j;
    j["minFitness"] = minFitness;
    j["maxFitness"] = maxFitness;
    j["averageFitness"] = averageFitness;
    return j;
}

void Population::calcDiversity_serial() {


    size_t orientationCount = chromosomes.front().turnList.size();

    int populationSize = (int) chromosomes.size();

    uint64_t sum = 0;

    for (size_t i = 0; i < populationSize; ++i)
    {
        for (size_t j = i + 1; j < populationSize; ++j)
        {
            const Chromosome & c1 = chromosomes[i];
            const Chromosome & c2 = chromosomes[j];

            for (size_t pos = 0; pos < orientationCount; ++pos)
            {
                if (c1.turnList[pos] != c2.turnList[pos])
                {
                    ++sum;
                }
            }
        }
    };

    uint64_t numComparisons = (uint64_t) ((populationSize * populationSize / 2) - (populationSize / 2));

    uint64_t total = numComparisons * orientationCount;

    diversity = static_cast<float>(static_cast<double>(sum) / total);
    diversity *= 100.0f;

    diversity = std::roundf(diversity);

}

void Population::calcDiversity_parallel() {
    size_t orientationCount = chromosomes.front().turnList.size();
    int populationSize = static_cast<int>(chromosomes.size());

    uint64_t sum = 0;

    #pragma omp parallel for reduction(+:sum) schedule(dynamic)
    for (int i = 0; i < populationSize; ++i)
    {
        for (int j = i + 1; j < populationSize; ++j)
        {
            const Chromosome& c1 = chromosomes[i];
            const Chromosome& c2 = chromosomes[j];

            for (size_t pos = 0; pos < orientationCount; ++pos)
            {
                if (c1.turnList[pos] != c2.turnList[pos])
                {
                    ++sum;
                }
            }
        }
    }

    uint64_t numComparisons = static_cast<uint64_t>((populationSize * (populationSize - 1)) / 2);
    uint64_t total = numComparisons * orientationCount;

    diversity = static_cast<float>(static_cast<double>(sum) / total) * 100.0f;
    diversity = std::roundf(diversity);
}



