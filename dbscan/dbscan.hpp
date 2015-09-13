/* 
 * Author: Florian
 * Date: 30.06.2015
 * 
 * Members:
 * 	Data: 	Vector of Datapoint-class-objects
 * 	minPts:  number of direct-density-reachable points within "eposilon" to identify as core-point 
 * 	epsilon: radius
 * 	dataSize: Number of datapoints to cluster
 * 	dataDim : Number of features of each Datapoint
 * 	distMatP : Points to precalculated DistanceMatrix : dataSize X dataSize
 * 
 * Mehods:
 * 	run(): starts dbscan with constructed parameters
 * 	expandCluster(seedList): Expands a cluster around a start-point with it's neigbours in seedList
 * 	QR(i): returns the indices of datapoints wihtin the epsilon-range around point-i
 * 	getDist(i , j): returns the element dist[i * dataSize + j] from distMatP
 *
 */

#ifndef DBSCAN_HPP
#define DBSCAN_HPP


#include "datapoint.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <numeric>
#include <cassert>

#define real_t float

namespace clusterAlg{

class DBSCAN{
  
public:
  
  DBSCAN(const real_t* dataArray, const real_t* distMatArray, unsigned int dataSize, unsigned int dataDim, int minPts, real_t epsilon);
  virtual ~DBSCAN(void);
  
  DBSCAN(const DBSCAN & ) = delete;			// Delete Copy-constructor
  DBSCAN & operator= ( const DBSCAN & ) = delete;	// Delete assignment-operator
  DBSCAN & operator= ( DBSCAN && ) = delete; 		// Delete Move-assignment-operator


  // Main application methods
  void setStartIdx(const unsigned int & idx);
  void run(void);
    
  // show methods
  const void showClustering(void) const;
  const void showInfo(void) const;
  
  // Getter methods
  const std::vector< dataP::Datapoint > & getClustering( void ) const;
  std::vector< unsigned int > getDataIDs(void) const;
  std::vector< int > getClusterIDs(void) const;

private:
  // Calculate the QR distances
  std::vector< unsigned int > QR(unsigned int i);
  
  // DistanceMatrix methods
  real_t getDist(unsigned int i, unsigned int j);	// returns l2 distance between Data[i] and Data[j] from precalculated DistanceMatrix
  real_t calcDist(unsigned int i, unsigned int j);	// calculates l2 distance between Data[i] and Data[j]

  // main method
  void clusterDatapoint( dataP::Datapoint & d );
  void expandCluster(std::vector< unsigned int > & seedList);
  
  std::vector< dataP::Datapoint > Data;
  unsigned int dataSize;
  unsigned int dataDim;
  unsigned int startIdx;
  bool newStartIdx;
  
  unsigned int minPts;
  real_t epsilon;
  unsigned int currentClusterID;
  unsigned int num_iters;

  const real_t* distMatP;
  bool calcDistMatB;
  
};

}
#endif