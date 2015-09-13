#include "minioptics.h"

using namespace clusterAlg;

MiniOPTICS::MiniOPTICS(const real_t* dataArray, const real_t* distMatArray, unsigned int dataSize, unsigned int dataDim, real_t epsilon) 
: dataSize(dataSize), dataDim(dataDim), startIdx(0), useStartIdx(false), numProcessedIDs(0), clusterValley(false), currentClusterID(0),
  epsilon(epsilon), clusterThreshold(std::numeric_limits<real_t>::max())
{
  distMatP = distMatArray;
  calcDistMatB = distMatArray == NULL;
    
  for ( unsigned int i = 0; i < this->dataSize; i++ )
  {
    Data.push_back(dataP::OpticsDatapoint(&dataArray[i*this->dataDim], dataDim));
  }

  minSortedHeap.reserve(dataSize);
  
}

void MiniOPTICS::setStartIdx(const unsigned int & start)
{
  if ( start >= dataSize ) {
    fprintf(stderr, "MiniOPTICS: start index excceeds bounds!\n");
    exit(-1);
  }
  startIdx = start;
  useStartIdx = true;
}

MiniOPTICS::~MiniOPTICS(void)
{
  dataP::OpticsDatapoint::resetClassCounter();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
// distance matrix methods 							  //
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //

real_t MiniOPTICS::readDist(const unsigned int &i, const unsigned int &j)
{
  return ( i <= j ) ? distMatP[i * dataSize + j] : distMatP[j * dataSize + i];
}

real_t MiniOPTICS::calcDist(const unsigned int &i, const unsigned int &j)
{
  real_t l2dist = 0.0;
  for( unsigned int k = 0; k < dataDim; k++ )
  {
      l2dist += pow(Data[i].getFeatures()[k] - Data[j].getFeatures()[k], 2);
  }
  return sqrt(l2dist);
}

real_t MiniOPTICS::getDistance(const unsigned int &i, const unsigned int &j)
{
  if ( !calcDistMatB ) {
    return readDist(i, j);
  }
  else {
    return calcDist(i, j);
  }
}

std::vector< dataP::OpticsDatapoint* > MiniOPTICS::RQ(const unsigned int & i)
{
  std::vector< dataP::OpticsDatapoint* > seedList;
  
  for( unsigned int j = 0; j < dataSize; j++ )
  {
    if ( i == j || Data[j].getState() != dataP::UNVISITED ) {
      continue;
    }
    if ( getDistance(i ,j) <= epsilon ) {
      seedList.push_back( &Data[j] );
    }
  }
  
  return seedList;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
// Main methods 								  //
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //

void MiniOPTICS::run(void)
{
  if ( useStartIdx ) {
    expandClusterOrder(Data[startIdx]);
  }
  for ( auto & d : Data )
  {
    if ( d.getState() == dataP::UNVISITED ) {
      expandClusterOrder(d);
    }
  }
}

void MiniOPTICS::expandClusterOrder(dataP::OpticsDatapoint & d)
{
  minSortedHeap.push(&d);
  while ( !minSortedHeap.empty() )
  {
    // Add current datapoint to clusterorder
    dataP::OpticsDatapoint* currPoint = minSortedHeap.pop_top();
    clusterOrder.push_back(*currPoint);
    addToCluster(*currPoint);
    
    for ( auto & n : RQ(currPoint->getID()) )
    {
      if ( n->getState() != dataP::UNVISITED ) {
	continue;
      }
      real_t newDist = calcDist(currPoint->getID(), n->getID());
      if ( n->getSeedState() && newDist < n->getReachabilityDist() ) {
	n->setReachabilityDist(newDist);
	minSortedHeap.update(n);
      }
      else if ( !n->getSeedState() ) {
	n->setReachabilityDist(newDist);
	minSortedHeap.push(n);
      }
    }
    numProcessedIDs++;
  }         
}

void MiniOPTICS::addToCluster(const dataP::OpticsDatapoint & d)
{
  if ( d.getReachabilityDist() <= clusterThreshold && numProcessedIDs > 0 ) {
     // Add datapoint to current cluster
    clusterValley = true;
        
    // Make sure the last point also belongs to the current cluster
    clusterIDs.back() = currentClusterID;
    Data[clusterOrder.ids[clusterIDs.size()-1]].setClusterID(currentClusterID);
    Data[clusterOrder.ids[clusterIDs.size()-1]].setState(dataP::CLUSTERED);
    
    clusterIDs.push_back(currentClusterID);
    Data[d.getID()].setClusterID(currentClusterID);
    Data[d.getID()].setState(dataP::CLUSTERED);    
  }
  else {
    // Mark as noise and increment clusterID
    if ( clusterValley ) currentClusterID++;
    clusterValley = false;
    
    clusterIDs.push_back(-999);
    Data[d.getID()].setClusterID(-999);
    Data[d.getID()].setState(dataP::NOISE);
  }
}

void MiniOPTICS::updateClusterIDs(const real_t & threshold)
{
  clusterIDs.clear();
  clusterThreshold = threshold;
  currentClusterID = 0;
  clusterValley = false;
  
  for ( unsigned int i = 0; i < clusterOrder.size(); ++i )
  {
    addToCluster(Data[clusterOrder.ids[i]]);
  }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
// Show methods 								  //
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //

void MiniOPTICS::showClustering(void) const
{
  for ( unsigned int i = 0; i < clusterOrder.size(); ++i )
  {
    std::cout << "Data ID: " << clusterOrder.ids[i] << 
    "\t,ClusterState: " << Data[clusterOrder.ids[i]].getState() << 
    "\t,ClusterID: " << Data[clusterOrder.ids[i]].getClusterID() <<
    "\t,reachability distance: " <<  clusterOrder.reachabilityDists[i] << std::endl;
  }
}

void MiniOPTICS::showInfo(void) const
{
  printf("################# INFO #####################\n");
  printf("# epsilon: %f\n", epsilon);
  printf("# External distance matrix: %s\n", !calcDistMatB ? "True" : "False" );
  printf("# Datasize: %i\n", dataSize);
  printf("# DataDim: %i\n", dataDim);
  printf("# Heap size: %u\n", minSortedHeap.size());
  printf("# Cluster order size: %u\n", clusterOrder.size());
  printf("# Number of processed IDs: %u\n", numProcessedIDs);
  printf("# Number of found clusters: %u\n", numProcessedIDs > 0? currentClusterID+1 : currentClusterID);
  printf("################# END INFO #################\n");
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
// Getter methods 								  //
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //

const dataP::ClusterOrdering & MiniOPTICS::getClusterOrder(void) const
{
  return clusterOrder;
}

const std::vector< unsigned int > & MiniOPTICS::getClusterOrderIDs(void) const
{
  return clusterOrder.ids;
}
const std::vector< real_t > & MiniOPTICS::getClusterOrderDistances(void) const
{
  return clusterOrder.reachabilityDists;
}

const std::vector< int > & MiniOPTICS::getClusterIDs(void) const
{
  return clusterIDs;
}

template <typename T> std::vector< T > MiniOPTICS::getLabels(void) const
{
  std::vector< T > labels;
  for ( auto & d : Data )
  {
    labels.push_back(static_cast< T >(d.getClusterID()));
  }

  return labels;
}

template std::vector<int> MiniOPTICS::getLabels<int>(void) const;
template std::vector<real_t> MiniOPTICS::getLabels<real_t>(void) const;
