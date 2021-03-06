#include "optics.h"

using namespace clusterAlg;

OPTICS::OPTICS(const real_t* dataArray, const real_t* distMatArray, unsigned int dataSize, unsigned int dataDim, int minPts, real_t epsilon) 
: dataSize(dataSize), dataDim(dataDim), startIdx(0), useStartIdx(false), numProcessedIDs(0), clusterValley(false), currentClusterID(0),
  minPts(minPts), epsilon(epsilon), clusterThreshold(std::numeric_limits<real_t>::max())
{
  distMatP = distMatArray;
  calcDistMatB = distMatArray == NULL;
    
  for ( unsigned int i = 0; i < this->dataSize; i++ )
  {
    Data.push_back(dataP::OpticsDatapoint(&dataArray[i*this->dataDim], dataDim));
  }

  minSortedHeap.reserve(dataSize);
  
}

void OPTICS::setStartIdx(const unsigned int & start)
{
  if ( start >= dataSize ) {
    fprintf(stderr, "OPTICS: start index excceeds bounds!\n");
    exit(-1);
  }
  startIdx = start;
  useStartIdx = true;
}

OPTICS::~OPTICS(void)
{
  dataP::OpticsDatapoint::resetClassCounter();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
// distance matrix methods 							  //
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //

real_t OPTICS::readDist(const unsigned int &i, const unsigned int &j)
{
  return ( i <= j ) ? distMatP[i * dataSize + j] : distMatP[j * dataSize + i];
}

real_t OPTICS::calcDist(const unsigned int &i, const unsigned int &j)
{
  real_t l2dist = 0.0;
  for( unsigned int k = 0; k < dataDim; k++ )
  {
      l2dist += pow(Data[i].getFeatures()[k] - Data[j].getFeatures()[k], 2);
  }

  return sqrt(l2dist);
}

real_t OPTICS::getDistance(const unsigned int &i, const unsigned int &j)
{
  if ( !calcDistMatB ) {
    return readDist(i, j);
  }
  else {
    return calcDist(i, j);
  }
}

std::vector< dataP::OpticsDatapoint* > OPTICS::RQ(unsigned int i)
{
  std::vector< dataP::OpticsDatapoint* > seedList;
    
  for( unsigned int j = 0; j < dataSize; j++ )
  {
    if ( i == j ) {
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

void OPTICS::run(void)
{
    int ss = 0;
    std::cout << "unvisite passesd " << Data.size() << std::endl;
    if ( useStartIdx ) {
         expandClusterOrder(Data[startIdx]);
    }
    for ( auto & d : Data )
    {
      if ( d.getState() == dataP::UNVISITED ) {
	
	  ss++;
          expandClusterOrder(d);
      }
    }
    std::cout << "unvisite passesd " << ss << std::endl;
}

void OPTICS::expandClusterOrder(dataP::OpticsDatapoint & d)
{
    minSortedHeap.push(&d);
    while( !minSortedHeap.empty() )
    {
        // Add current datapoint to clusterorder
        dataP::OpticsDatapoint* currPt = minSortedHeap.pop_top();	
        clusterOrder.push_back(*currPt);
        addToCluster(*currPt);

	// find neighbours and calculate coredistance
        std::vector< dataP::OpticsDatapoint* > neighbours = RQ(currPt->getID());

	currPt->setCoreDist( calculateCoreDistance(*currPt, neighbours) );

	// Expand the cluster
        for ( auto & i : neighbours )
        {
            if ( i->getState() != dataP::UNVISITED ) {
                continue;
            }
            real_t nrdist = getDistance(currPt->getID(), i->getID());
            if ( currPt->getCoreDist()  > nrdist ) {
                nrdist = currPt->getCoreDist();
            }
            if (i->getReachabilityDist() == dataP::OpticsDatapoint::INF and i->getHeapPushCounter() == 0 ) {
                i->setReachabilityDist(nrdist);
                minSortedHeap.push(i);
		i->incrementHeapPushCounter();
            }
            else if (nrdist < i->getReachabilityDist()) {
                i->setReachabilityDist(nrdist);
                minSortedHeap.update(i);
            }
        }
        numProcessedIDs++;
    }
}

real_t OPTICS::calculateCoreDistance( dataP::OpticsDatapoint & d, std::vector< dataP::OpticsDatapoint* > & ns )
{
  if ( ns.size() < minPts - 1 ) {
    // d is no core point --> coreDist is undefined
    return dataP::OpticsDatapoint::INF;
  }
  for ( auto & n : ns )
  {
    // Calculate dist(d,n) for all ns
    n->setTmpDist(getDistance(d.getID(), n->getID()));
  }
  // Sort neighbours vector by increasing reachability distance
  sort( ns.begin(), ns.end(), dataP::compareOpticsDatapoints_by_tmpDist() );
  return ns[minPts - 2]->getTmpDist();
}

void OPTICS::addToCluster(const dataP::OpticsDatapoint & d)
{   
  if ( d.getReachabilityDist()  <= clusterThreshold && numProcessedIDs > 0 ) {
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

void OPTICS::updateClusterIDs(const real_t & threshold)
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

void OPTICS::showClustering(void) const
{
  for ( unsigned int i = 0; i < clusterOrder.size(); ++i )
  {
    std::cout << "Data ID: " << clusterOrder.ids[i] << 
    "\t, ClusterState: " << Data[clusterOrder.ids[i]].getState() << 
    "\t, ClusterID: " << Data[clusterOrder.ids[i]].getClusterID() <<
    "\t, reachability distance: " <<  clusterOrder.reachabilityDists[i] << //std::endl;
    "\t, setStateCounter: " << Data[clusterOrder.ids[i]].getSetStateCounter() << // std::endl;
    "\t, heapPushCounter: " << Data[clusterOrder.ids[i]].getHeapPushCounter() << std::endl;
    //"\t, core-distance: " << Data[clusterOrder.ids[i]].getCoreDist() <<std::endl;
  }
}

void OPTICS::showInfo(void) const
{
  printf("################# INFO #####################\n");
  printf("# minPts: %i\n", minPts);
  printf("# epsilon: %f\n", epsilon);
  printf("# External distance matrix: %s\n", !calcDistMatB ? "True" : "False" );
  printf("# Datasize: %i\n", dataSize);
  printf("# DataDim: %i\n", dataDim);
  printf("# Heap size: %u\n", minSortedHeap.size());
  printf("# Cluster order size: %u\n", clusterOrder.size());
  printf("# Numer of processed Datapoints: %u\n", numProcessedIDs);
  printf("# Number of found clusters: %u\n", numProcessedIDs > 0 ? currentClusterID+1 : currentClusterID);
  printf("################# END INFO #################\n");
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
// Getter methods 								  //
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //

const dataP::ClusterOrdering & OPTICS::getClusterOrder(void) const
{
  return clusterOrder;
}

const std::vector< unsigned int > & OPTICS::getClusterOrderIDs(void) const
{
  return clusterOrder.ids;
}
const std::vector< real_t > & OPTICS::getClusterOrderDistances(void) const
{
  return clusterOrder.reachabilityDists;
}

const std::vector< int > & OPTICS::getClusterIDs(void) const
{
  return clusterIDs;
}

template <typename T> std::vector< T > OPTICS::getLabels(void) const
{
  std::vector< T > labels;
  for ( auto & d : Data )
  {
    labels.push_back(static_cast< T >(d.getClusterID()));
  }

  return labels;
}

template std::vector<int> OPTICS::getLabels<int>(void) const;
template std::vector<real_t> OPTICS::getLabels<real_t>(void) const;

