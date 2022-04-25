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
//#include "Launchers/Garbage.cpp"
#include "Launchers/GarbageList.cpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

//PYBIND11_MAKE_OPAQUE(std::map<std::string, std::vector<int>>)


using ShortestPathAlgoT=DijkstraAdapter;
using base= FrankWolfeAssignment<UserEquilibrium,BprFunction,ShortestPathAlgoT>;
using AllOrNothing = AllOrNothingAssignment<ShortestPathAlgoT>;
namespace py = pybind11;


class AssignTrafficPython{
	public:
	AssignTrafficPython(){

	}
	AssignTrafficPython(int flag): flag(flag){

	}

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
		std::vector<ClusteredOriginDestination> ODPairs =importODPairsFrom(demand);
		bool verbose =false;
		bool elasticRebalance=false;
		FrankWolfeAssignment<UserEquilibrium,BprFunction,DijkstraAdapter> assign(graph,ODPairs,verbose,elasticRebalance);

		return assign.runPython(numIterations);
	}

	int flag;

};


int add(int i, int j) {
    return i + j;
}

FrankWolfeAssignment<UserEquilibrium,BprFunction,DijkstraAdapter> getFWAssignment(std::map<std::string,std::vector<int>> demand, std::map<std::string,std::vector<int>> edges){
	Graph graph(edges,0.0,100.0);
	std::vector<ClusteredOriginDestination> ODPairs =importODPairsFrom(demand);
	bool verbose =false;
	bool elasticRebalance=false;
	FrankWolfeAssignment<UserEquilibrium,BprFunction,DijkstraAdapter> assign(graph,ODPairs,verbose,elasticRebalance);
	return assign;
}


PYBIND11_MODULE(frankwolfe, m) {
    m.doc() = "pybind11  plugin"; // optional module docstring
	m.def("importODPairsFrom",&importODPairsFrom); //importODPairsFrom(std::map<std::string,std::vector<int>> demand)
    m.def("add", &add, "A function that adds two numbers");
	m.def("getFWAssignment",&getFWAssignment, "return object that calculates flow");
	py::class_<AssignTrafficPython> (m, "AssignTrafficPython")
		.def("flow", &AssignTrafficPython::flow)
		.def(py::init([](){
			return new AssignTrafficPython();
		}))
		.def(py::pickle([](const AssignTrafficPython & atp){ // __getstate__
			std::map<std::string,int> dict;
			dict["flag"]=atp.flag;
			return dict;
		},[](std::map<std::string,int> dict){ // __setstate__
			int flag=dict["flag"];
			return new AssignTrafficPython(flag);
		}));
	py::class_<ClusteredOriginDestination> (m,"ClusteredOriginDestination")
		.def_readwrite("origin",&ClusteredOriginDestination::origin)
		.def_readwrite("destination",&ClusteredOriginDestination::destination)
		.def_readwrite("volume",&ClusteredOriginDestination::volume)
		.def_readwrite("rebalancer",&ClusteredOriginDestination::rebalancer)
		.def_readwrite("edge1",&ClusteredOriginDestination::edge1)
		.def_readwrite("edge2",&ClusteredOriginDestination::edge2)
		.def(py::init([](const int o, const int d, const int r, const int e1, const int e2, const int v){
			return new ClusteredOriginDestination(o,d,r,e1,e2,v);
		}))
		.def(py::pickle([](const ClusteredOriginDestination & cod){ // __getstate__
			std::map<std::string,int> dict;
			dict["origin"]=cod.origin;
			dict["destination"]=cod.destination;
			dict["volume"]=cod.volume;
			dict["rebalancer"]=cod.rebalancer;
			dict["edge1"]=cod.edge1;
			dict["edge2"]=cod.edge2;
			return dict;
		},[](std::map<std::string,int> dict){ // __setstate__
			int origin=dict["origin"];
			int destination=dict["destination"];
			int volume=dict["volume"];
			int rebalancer=dict["rebalancer"];
			int edge1=dict["edge1"];
			int edge2=dict["edge2"];
			return new ClusteredOriginDestination(origin,destination,rebalancer, edge1, edge2, volume);
		}));
	py::class_<Graph> (m, "Graph") //std::map<std::string,std::vector<int>> edges,const double ceParameter, const double constParameter)
		.def("updateEdges",&Graph::updateEdges)
		.def_readwrite("constParameter",&Graph::constParameter)
		.def_readwrite("ceParameter",&Graph::ceParameter)
		.def_readwrite("edges",&Graph::edges)
		.def_readwrite("edgeTail",&Graph::edgeTail)
		.def_readwrite("edgeHead",&Graph::edgeHead)
		.def_readwrite("edgeCapacity",&Graph::edgeCapacity)
		.def_readwrite("edgeLength",&Graph::edgeLength)
		.def_readwrite("edgeFreeTravelTime",&Graph::edgeFreeTravelTime)
		.def_readwrite("edgeWeight",&Graph::edgeWeight)
		.def(py::init([](std::map<std::string,std::vector<int>> edges,const double ceParameter, const double constParameter){
			return new Graph(edges, ceParameter,constParameter);
		}))
		.def(py::pickle([](const Graph& gr){ // __getstate__
			std::map<std::string,py::object> dict;
			dict["edges"]=py::cast(gr.edges);
			dict["ceParameter"]=py::cast(gr.ceParameter);
			dict["constParameter"]=py::cast(gr.constParameter);
			return dict;
		},[](std::map<std::string,py::object> dict){ // __setstate__
			std::map<std::string,std::vector<int>> edges=dict["edges"].cast<std::map<std::string,std::vector<int>>>();
			double ceParameter=dict["ceParameter"].cast<double>();
			double constParameter=dict["constParameter"].cast<double>();
			return new Graph(edges, ceParameter,constParameter);
		}));
	py::class_<base>(m, "FrankWolfeAssignment")
		.def("runPython", &base::runPython)
		.def("updateEdges",&base::updateEdges)
		.def(py::init([](Graph & graph, std::vector<ClusteredOriginDestination> & ODPairs, const bool verbose, const bool elasticRebalance) {
        	return new base(graph,ODPairs,verbose,elasticRebalance);
    	}))
		.def(py::pickle([](const base& fwa){ // __getstate__
			std::map<std::string,py::object> dict;
			dict["ODPairs"]=py::cast(fwa.ODPairs);
			dict["graph"]=py::cast(fwa.graph);
			dict["verbose"]=py::cast(fwa.verbose);
			dict["elasticRebalance"]=py::cast(fwa.elasticRebalance);
			return dict;
		},[](std::map<std::string,py::object> dict){ // __setstate__
			std::vector<ClusteredOriginDestination> ODPairs=dict["ODPairs"].cast<std::vector<ClusteredOriginDestination>>();
			Graph graph=dict["graph"].cast<Graph>();
			bool verbose =dict["verbose"].cast<bool>();
			bool elasticRebalance=dict["elasticRebalance"].cast<bool>();
			return new base(graph,ODPairs,verbose,elasticRebalance);
		}));
}
