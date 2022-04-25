#pragma once

#include <cstdint>
#include <vector>
#include "DataStructures/Pickleable.h"

// Statistics about an iterative all-or-nothing assignment, including checksums and running times.
struct AllOrNothingAssignmentStats {
  // Constructs a struct collecting statistics about an iterative all-or-nothing assignment.
  AllOrNothingAssignmentStats(const int numODPairs)
      : lastChecksum(0),
        totalChecksum(0),
        lastDistances(numODPairs, -1),
        maxChangeInDistances(0),
        avgChangeInDistances(0),
        lastCustomizationTime(0),
        lastQueryTime(0),
        lastRoutingTime(0),
        totalPreprocessingTime(0),
        totalCustomizationTime(0),
        totalQueryTime(0),
        totalRoutingTime(0),
        numIterations(0) {}

  // Resets the values from the last iteration.
  void startIteration() {
    lastChecksum = 0;
    maxChangeInDistances = 0;
    avgChangeInDistances = 0;
  }

  void reset() {
    lastDistances = std::vector<int>(lastDistances.size(),-1);
    numIterations=0;
  }

  // Adds the values from the last iteration to the totals.
  void finishIteration() {
    lastRoutingTime = lastCustomizationTime + lastQueryTime;
    totalChecksum += lastChecksum;
    totalCustomizationTime += lastCustomizationTime;
    totalQueryTime += lastQueryTime;
    totalRoutingTime += lastRoutingTime;
  }

  int64_t lastChecksum;  // The sum of the distances computed in the last iteration.
  int64_t totalChecksum; // The total sum of distances computed.

  std::vector<int> lastDistances; // The OD-distances from the last iteration.
  double maxChangeInDistances;    // The max change in the OD-distances between the last iterations.
  double avgChangeInDistances;    // The avg change in the OD-distances between the last iterations.

  int lastCustomizationTime; // The time spent on customization in the last iteration.
  int lastQueryTime;         // The time spent on queries in the last iteration.
  int lastRoutingTime;       // The time spent on routing in the last iteration.

  int totalPreprocessingTime; // The total time spent on preprocessing.
  int totalCustomizationTime; // The total time spent on customization.
  int totalQueryTime;         // The total time spent on queries.
  int totalRoutingTime;       // The total time spent on routing.

  int numIterations; // The number of iterations performed.
};
