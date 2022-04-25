#pragma once

#include <list>
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <vector>
#include <execution>
#include <omp.h>

#include "DataStructures/Graph/Graph.h"
#include "DataStructures/Utilities/OriginDestination.h"
#include "Stats/TrafficAssignment/AllOrNothingAssignmentStats.h"
#include "Tools/Simd/AlignedVector.h"
#include "Tools/Timer.h"

using ODPairs = std::vector<ClusteredOriginDestination>;

// Implementation of an iterative all-or-nothing traffic assignment. Each OD-pair is processed in
// turn and the corresponding OD-flow (in our case always a single flow unit) is assigned to each
// edge on the shortest path between O and D. Other O-D paths are not assigned any flow. The
// procedure can be used with different shortest-path algorithms.
template <typename ShortestPathAlgoT>
class AllOrNothingAssignment {
public:
	// Constructs an all-or-nothing assignment instance.
	AllOrNothingAssignment(Graph& graph,
						   std::vector<ClusteredOriginDestination> & odPairs,
						   const bool verbose = true, const bool elasticRebalance = false)
		: stats(odPairs.size()),
		  shortestPathAlgo(graph),
		  inputGraph(graph),
		  verbose(verbose),
		  elasticRebalance(elasticRebalance)
		{
			//std::cout<< "allornthing args this.odPairs[0] address " << &this->odPairs[0]<<std::endl;
			Timer timer;
			shortestPathAlgo.preprocess();
			stats.totalPreprocessingTime = timer.elapsed();
			stats.lastRoutingTime = stats.totalPreprocessingTime;
			stats.totalRoutingTime = stats.totalPreprocessingTime;
			if (verbose) std::cout << "  Prepro: " << stats.totalPreprocessingTime << "ms" << std::endl;
			paths = std::vector<std::vector<int>>(odPairs.size(), std::vector<int>());
			for(ClusteredOriginDestination cod : odPairs){
				this->odPairs.push_back(cod);
			}
		
		}

	// Assigns all OD-flows to their currently shortest paths.
	void run() {
		Timer timer;
		++stats.numIterations;
		if (verbose) std::cout << "Iteration " << stats.numIterations << ": " << std::endl;

		shortestPathAlgo.customize();
		//if (verbose) std::cout << "shortestPathAlgo.customize()" << std::endl;
		stats.lastCustomizationTime = timer.elapsed();
		//if (verbose) std::cout << "stats.lastCustomizationTime = timer.elapsed();" << std::endl;
		timer.restart();
		//if (verbose) std::cout << "timer.restart();" << std::endl;
		// assign initial flow of 0 to each edge
		trafficFlows.assign(inputGraph.numEdges(), 0);
		//if (verbose) std::cout << "trafficFlows.assign(inputGraph.numEdges(), 0);" << std::endl;
		stats.startIteration();
		//if (verbose) std::cout << "stats.startIteration();" << std::endl;
		//if (verbose) std::cout << "odPairs addres "<< &this->odPairs << std::endl;
		// find shortest path between each OD pair and collect flows
		if (elasticRebalance) // comptue for elastic AMoD
		{
			/*
				TODO:
				The current subroutine doesn't aggregate shortest path queries according to origin vertex, which may lead to large computation times.

				A better solution would be to have four different shortest path queries to each origin-destination pair. That is, the OD-pair file would specify for each (virtual) origin-destination the id of the real od-pair, and a number in {0,..,3} specifying the type of query it represents. 
			 */
			for (int i = 0; i < odPairs.size(); i++)
			{
				std::cout << "elastic\n"<<std::flush;
				std::vector<int> path_od, path_dr, path_or;
				double cost_od, cost_dr, cost_or = 0;
				
				cost_od = shortestPathAlgo.run(odPairs[i].origin, odPairs[i].destination, path_od); // passenger path from new origin to real destination
				cost_dr = shortestPathAlgo.run(odPairs[i].destination, odPairs[i].rebalancer, path_dr); // path for rebalancer

				path_or.push_back(odPairs[i].edge1);
				path_or.push_back(odPairs[i].edge2);

				cost_or = inputGraph.weight(odPairs[i].edge1) + inputGraph.weight(odPairs[i].edge2);
				
				if (cost_od + cost_dr < cost_or) 
				{ // real path used
					paths[i] = path_od;
					paths[i].insert(paths[i].end(), path_dr.begin(), path_dr.end());
				} else 
				{ // virtual path used
					paths[i] = path_or;
				}
			}
			for (int i = 0; i < odPairs.size(); i++){

				//if (verbose) std::cout << "for (int i = 0; i < odPairs.size(); i++){" << std::endl;
				for(int j=0;j<paths[i].size();j++){
					const auto& e =paths[i][j];
					//if (verbose) std::cout << "paths[i][j];" << std::endl;
					trafficFlows[e] += odPairs[i].volume;
					//if (verbose) std::cout << "trafficFlows[e] += odPairs[i].volume;" << std::endl;
				}
				
			}

		}
		else // compute for classic traffic assignment
		{
			//#pragma omp for
			for (int i = 0; i < odPairs.size(); i++)
			{	
				/*
				if (verbose) std::cout << "&odPairs[i] " << &odPairs[i] << std::endl;
				if (verbose) std::cout << "(odPairs[i].origin " << odPairs[i].origin << std::endl;
				if (verbose) std::cout << "(odPairs[i].destination " << odPairs[i].destination << std::endl;
				if (verbose) std::cout << "paths[i].size() " << paths[i].size() << std::endl;
				*/
				shortestPathAlgo.run(odPairs[i].origin, odPairs[i].destination, paths[i]);
			}
			//#pragma omp parallel
			//{
			//if (verbose) std::cout << "loop" << std::endl;
			for (int i = 0; i < odPairs.size(); i++){
				for(int j=0;j<paths[i].size();j++){
					//if (verbose) std::cout << "for(int j=0;j<paths[i].size();j++){;" << std::endl;
					const auto& e =paths[i][j];
					//if (verbose) std::cout << "paths[i][j];" << std::endl;
					trafficFlows[e] += odPairs[i].volume;
					//if (verbose) std::cout << "trafficFlows[e] += odPairs[i].volume;" << std::endl;
				}
				
			}
		}
		
		
		stats.lastQueryTime = timer.elapsed();
		stats.finishIteration();

		if (verbose) {
			std::cout << "  Checksum: " << stats.lastChecksum;
			std::cout << "  Custom: " << stats.lastCustomizationTime << "ms";
			std::cout << "  Queries: " << stats.lastQueryTime << "ms";
			std::cout << "  Routing: " << stats.lastRoutingTime << "ms\n";
			std::cout << std::flush;
		}
	}

	// Returns the traffic flow on edge e.
	double trafficFlowOn(const int e) const {
		assert(e >= 0); assert(e < inputGraph.numEdges());
		return (double)trafficFlows[e];
	}

	std::vector<std::vector<int>>& getPaths()
	{
		return paths;
	}

	AllOrNothingAssignmentStats stats; // Statistics about the execution.
	ODPairs odPairs;

private:

	ShortestPathAlgoT shortestPathAlgo; // Algo computing shortest paths between OD-pairs.
	Graph& inputGraph;					// The input graph.
	std::vector<int> trafficFlows;			// The traffic flows on the edges.
	std::vector<std::vector<int>> paths;	// paths of the individual od pairs
	const bool verbose;                 // Should informative messages be displayed?
	const bool elasticRebalance;		// if true, compute compute AMoD with elastic demand
	
};
