#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <bits/stdc++.h> 
#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 

#include <boost/dynamic_bitset.hpp>
#include <routingkit/customizable_contraction_hierarchy.h>
#include <routingkit/nested_dissection.h>

#include "Algorithms/TrafficAssignment/Adapters/DijkstraAdapter.h"
#include "Algorithms/TrafficAssignment/Adapters/ConstrainedAdapter.h"
#include "Algorithms/TrafficAssignment/ObjectiveFunctions/SystemOptimum.h"
#include "Algorithms/TrafficAssignment/ObjectiveFunctions/UserEquilibrium.h"
#include "Algorithms/TrafficAssignment/ObjectiveFunctions/CombinedEquilibrium.h"
#include "Algorithms/TrafficAssignment/TravelCostFunctions/BprFunction.h"
#include "Algorithms/TrafficAssignment/TravelCostFunctions/ModifiedBprFunction.h"
#include "Algorithms/TrafficAssignment/FrankWolfeAssignment.h"
#include "DataStructures/Graph/Graph.h"
#include "DataStructures/Utilities/OriginDestination.h"
#include "Tools/CommandLine/CommandLineParser.h"
#include "Tools/Timer.h"

//PYBIND11_MAKE_OPAQUE(std::map<std::string, std::vector<int>>)

void printUsage() {
	std::cout <<
		"Usage: AssignTraffic [-obj <objective>] [-f <func>] [-a <algo>] [-n <num>] [-ce <num>] -i <file> -od <file> [-o <path>]  \n"
		"This program assigns OD-pairs onto a network using the Frank-Wolfe method. It\n"
		"supports different objectives, travel cost functions and shortest-path algos.\n"
		"  -obj	<objective>		objective function:\n"
		"							sys_opt (default), user_eq, combined_eq\n"
		"  -f <func>			travel cost function:\n"
		"							bpr (default) modified_bpr\n"
		"  -a <algo>			shortest-path algorithm:\n"
		"							dijkstra (default) constrained\n"
		"  -n <num>				number of iterations (default = 100)\n"
		"  -ce_param <num>		combined_eq interpolation parameter in [0,1]:\n"
		"						0 for UE, 1 for SO\n"
		"  -const_param <num>	distance multiplier for constrained search"
		"  -elastic				flag for elastic demand with rebalancing"
		"  -i <path>			input graph edge CSV file\n"
		"  -od <file>			OD-pair file\n"
		"  -o <path>			output path\n"
		"  -v					display informative messages\n"
		"  -help				display this help and exit\n";  
}


// Assigns all OD-flows onto the input graph.


/**
 * It takes in a map of demand, a map of edges, and a number of iterations. It then runs the assignment
 * algorithm on the graph with the given demand and edges.
 * 
 * @param demand a map of origin-destination pairs of the form {origin: [], destination: [], volume:[]}
 * @param edges a map of edges {edge_tail: [], "edge_head":[], "length":[], "capacity":[], "speed":[]}
 * @param numIterations the number of iterations to run the algorithm for.
 * 
 * @return A map of the form {tail: [], head: [],flow: []}
 */
std::map<std::string,std::vector<int>> flow(std::map<std::string,std::vector<int>> demand, std::map<std::string,std::vector<int>> edges, int numIterations){
	Graph graph(edges,0.0,100.0);
	std::vector<ClusteredOriginDestination> odPairs =importODPairsFrom(demand);
	const bool verbose =false;
	const bool elasticRebalance=false;
	std::ofstream stream;
	FrankWolfeAssignment<UserEquilibrium,BprFunction,DijkstraAdapter> assign(graph,odPairs,verbose,elasticRebalance);

	return assign.runPython(numIterations);
}
// Picks the shortest-path algorithm according to the command line options.

int main() {
	return EXIT_SUCCESS;
}
