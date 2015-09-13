#include "datapoint.h"

using namespace dataP;

int Datapoint::class_counter;	// Allocate memory for static class member, initialized to 0 by default

Datapoint::Datapoint(const real_t* featureArray, unsigned int feature_size) : state(UNVISITED), ID(class_counter++), clusterID(-999), mfeature_size(feature_size)
{
  mfeatureP = featureArray;
}

const ClusterState & Datapoint::getState(void) const
{
  return state;
}

const unsigned int & Datapoint::getID(void) const
{
  return ID;
}

const int & Datapoint::getClusterID(void) const
{
  return clusterID;
}

const unsigned int & Datapoint::getSize(void) const
{
  return mfeature_size;
}

const real_t* Datapoint::getFeatures(void) const
{
  return mfeatureP;
}

unsigned int Datapoint::getLatestID(void)
{
  return class_counter;
}

void Datapoint::setState(ClusterState new_state)
{
  state = new_state;
}
 
void Datapoint::setClusterID(int cID)
{
  clusterID = cID;
}

void Datapoint::resetClassCounter(void)
{
  class_counter = 0;
}

Datapoint::~Datapoint(void){}