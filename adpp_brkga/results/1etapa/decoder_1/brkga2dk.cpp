/**
 * api-usage.cpp
 *
 * Driver class with a simple example of how to instantiate and use the BRKGA API.
 * See SampleDecoder.h for details on the decoder's implementation.
 *
 * Created on : Nov 17, 2011 by rtoso
 * Authors    : Rodrigo Franco Toso <rtoso@cs.rutgers.edu>
 *              Mauricio G.C. Resende <mgcr@research.att.com>
 * Copyright 2010, 2011 Rodrigo Franco Toso and Mauricio G.C. Resende.
 * 
 * This file is part of the BRKGA API.
 *
 * The BRKGA API is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The BRKGA API is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the BRKGA API. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <algorithm>
#include "brkgaAPI/BRKGA.h"
#include "brkgaAPI/MTRand.h"
#include "2dbinpack/2dbinpack.h"
#include "brkga2dkDecoder.h"
#include "brkga2dkDecoder_2.h"
#include "brkga2dkDecoder_3.h"
#include "brkga2dkDecoder_4.h"
#include "knapsack2d.h"
#include <ctime>
#include <limits>
#include <iomanip>
int main(int argc, char* argv[]) {
    std::cout << "Welcome to the 2dKBRKGA"<< std::endl;

    if (argc > 1) {
        //const unsigned p = atoi(argv[2]);
        //const double pe = atof(argv[3]);
        //const double pm = atof(argv[4]);
        //const double rhoe = atof(argv[5]);
        //const unsigned K = atoi(argv[6]);
        //const unsigned MAXT = atoi(argv[7]);
        //const unsigned X_NUMBER = atoi(argv[9]);
        //const unsigned MAX_GENS = atoi(argv[10]);
        //const unsigned X_INTVL = MAX_GENS * atof(argv[8]);

        //const unsigned n = 100;		// size of chromosomes
        const double pe = 0.10;		// fraction of population to be the elite-set
        const double pm = 0.10;		// fraction of population to be replaced by mutants
        const double rhoe = 0.70;	// probability that offspring inherit an allele from elite parent
        const unsigned K = 3;		// number of independent populations
        const unsigned MAXT = 4;	// number of threads for parallel decoding

        const unsigned X_INTVL = 100;	// exchange best individuals at every 100 generations
        const unsigned X_NUMBER = 2;	// exchange top 2 best
        const unsigned MAX_GENS = 10000;	// run for 
        const double fitRate = 0.98;
        const double sizeRate = 0.05;
        const bool DiversifyPopulation = true;
        double bestFitness = std::numeric_limits<double>::max();
        double TIMELIMIT = 1800.0;
	double elapsed_secs;


        knapsack2d instance(argv[1]);
        brkga2dkDecoder decoder(instance);				// initialize the decoder
        //brkga2dkDecoder_2 decoder_2(instance);
        //brkga2dkDecoder_3 decoder_3(instance);
        //brkga2dkDecoder_4 decoder_4(instance);

        const unsigned n = decoder.getChromosomeSize();
        //const unsigned n = decoder_2.getChromosomeSize();
        //const unsigned n = decoder_3.getChromosomeSize();
        //const unsigned n = decoder_4.getChromosomeSize();

        int logL =  7 * (int)(log(decoder.getChromosomeSize()) / log(2));	
        
        const unsigned p = ((logL % 2 == 0)?logL: logL+1);	// size of population
        std::clock_t begin = clock();

        const long unsigned rngSeed = time(NULL);	// seed to the random number generator
        MTRand rng(rngSeed);				// initialize the random number generator

        // initialize the BRKGA-based heuristic
        //first decoder
        BRKGA< brkga2dkDecoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);
        //BRKGA< brkga2dkDecoder_2, MTRand > algorithm(n, p, pe, pm, rhoe, decoder_2, rng, K, MAXT);
        //BRKGA< brkga2dkDecoder_3, MTRand > algorithm(n, p, pe, pm, rhoe, decoder_3, rng, K, MAXT);
        //BRKGA< brkga2dkDecoder_4, MTRand > algorithm(n, p, pe, pm, rhoe, decoder_4, rng, K, MAXT);

        unsigned generation = 0;		// current generation
        std::cout << "Running for " << MAX_GENS << " generations..." << std::endl;

        do {
            algorithm.evolve();	// evolve the population for one generation


            if(algorithm.getBestFitness() < bestFitness){
                bestFitness = algorithm.getBestFitness();
                elapsed_secs = double(clock()- begin) / CLOCKS_PER_SEC;


                cout << "Gen: " << generation << " Improved solution: " << bestFitness << " Time: " << elapsed_secs<< endl;

            }

            if((++generation) % X_INTVL == 0) {
                algorithm.exchangeElite(X_NUMBER);	// exchange top individuals
            }
            if(DiversifyPopulation){

                //For k populations
                for(unsigned i = 0; i < K; ++i) {
                    double fit = algorithm.getPopulation(i).getBestFitness(); 
                    //cout << fit << endl;

                    //Diversity
                    for(unsigned j = 0; j < algorithm.getPopulation(i).getP(); ++j) {
                        // dont want to remove the fitrate(%) of bests

                        if(algorithm.getPopulation(i).getFitness(j) / fit >= fitRate) continue;

                        for(unsigned z = j + 1; z < algorithm.getPopulation(i).getP(); ++z) {
                            int count = 0;
                            //get a chromoesome and compare 1-by-1 elements from population i
                            for(unsigned m = 0; m < (int)algorithm.getPopulation(i).getChromosome(j).size(); ++m) {

                                std::vector< double >& chrmoJ= algorithm.getPopulation(i).getChromosome(j);
                                std::vector< double >& chrmoZ= algorithm.getPopulation(i).getChromosome(z);

                                //cout << "J: "<< std::setprecision(9) <<  chrmoJ[m]<< " Z: "<< std::setprecision(9) << chrmoZ[m] << endl;
                                if(!(std::abs(chrmoJ[m] -  chrmoZ[m]) < 0.000000001)){
                                    ++count;
                                    //cout << "size*getN: " << sizeRate * algorithm.getN() << endl;
                                    if(count >= sizeRate * algorithm.getN())
                                        break;
                                }

                            }
                            if(count < sizeRate * algorithm.getN()){
                                for(unsigned k = 0; k < n; ++k) { (algorithm.getPopulation(i))(j, k) = rng.rand(); }

                            }
                        }   
                    }

                }
            }
                elapsed_secs = double(clock()- begin) / CLOCKS_PER_SEC;
        } while (generation < MAX_GENS && elapsed_secs < TIMELIMIT);

        // print the fitness of the top 10 individuals of each population:
        std::cout << "Fitness of the top 10 individuals of each population:" << std::endl;
        const unsigned bound = std::min(p, unsigned(10));	// makes sure we have 10 individuals
        for(unsigned i = 0; i < K; ++i) {
            std::cout << "Population #" << i << ":" << std::endl;
            for(unsigned j = 0; j < bound; ++j) {
                std::cout << "\t" << j << ") "
                    << algorithm.getPopulation(i).getFitness(j) << std::endl;
            }
        }
        std::clock_t end = clock();
        elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        cout << "Time: " << elapsed_secs << endl;

        std::cout << "Best solution found has objective value = "
            << algorithm.getBestFitness() << std::endl;
    } else {
        std::cout << "Usage: brkga <instance_file> <p> <pe> <pm> <rhoe> <K> <MAXT> <X_INTVL> <X_NUMBER> <MAX_GENS>" << std::endl;
    }




    return 0;
}
