/* 
 * Author: Florian
 * Date: 25.08.2015
 * 
 * Members:
 * 	vFeatures: data-vector
 * 	vDistances: vDistances to other datapoints
 * 	state: Current state in the Clustering-process
 * 	ID: Unique ID for the data-point
 * 	clusterID: ID for the Cluster, the data-point belongs to
 * 		 initialy set to -999
 * 	clusterName: Name of the Cluster, the data-poitn belongs to
 * 		intitaly set to ""
 */

#ifndef DATAPOINT_H
#define DATAPOINT_H

#include <vector>
#include <iostream>

#define real_t float

namespace dataP{
  
enum ClusterState {
  UNVISITED,
  VISITED,
  NOISE,
  CLUSTERED
};

inline std::ostream& operator<< (std::ostream & os, ClusterState osState)
{
  switch ( osState )
  {
    case UNVISITED : 
      os << "UNVISITED";
      return os;
    case VISITED : 
      os << "VISTIED";
      return os;
    case NOISE : 
      os << "NOISE";
      return os;
    case CLUSTERED : 
      os << "CLUSTERED";
      return os;
    default: os << "";
      return os;
  }
}

class Datapoint {

public:
  
  Datapoint(void) = default;
  Datapoint(const real_t* featureArray, unsigned int mfeature_size);
  virtual ~Datapoint(void);
  
  // Getter functions
  const ClusterState & getState(void) const;
  const unsigned int & getID(void) const;
  const int & getClusterID(void) const;
  const real_t* getFeatures(void) const;
  const unsigned int & getSize(void) const;
  static unsigned int getLatestID(void);
    
  // Setter functions:
  void setState(ClusterState new_state);
  void setClusterID(int cID);
  static void resetClassCounter(void);
  

protected:
  
  static int class_counter;

  ClusterState state;
  unsigned int ID;
  int clusterID;
  
  const real_t* mfeatureP;	//const pointer to array of features
  unsigned int mfeature_size;
  
};

}
#endif
