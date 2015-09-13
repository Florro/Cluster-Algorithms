#include "dbscan.hpp"

using namespace clusterAlg;

DBSCAN::DBSCAN(const real_t* dataArray, const real_t* distMatArray, unsigned int dataSize, unsigned int dataDim, int minPts, real_t epsilon) 
: dataSize(dataSize), dataDim(dataDim), startIdx(0), newStartIdx(false), minPts(minPts), epsilon(epsilon), currentClusterID(0), num_iters(0)
{
  distMatP = distMatArray;
  calcDistMatB = distMatArray == NULL;

  for ( unsigned int i = 0; i < this->dataSize; i++ )
  {
    Data.push_back(dataP::Datapoint(&dataArray[i*this->dataDim], dataDim));
  }

}

DBSCAN::~DBSCAN(void) 
{
  dataP::Datapoint::resetClassCounter();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
// Distance matrix methods 								  //
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //

real_t DBSCAN::getDist(unsigned int i, unsigned int j)
{
  return ( i <= j ) ? distMatP[i * dataSize + j] : distMatP[j * dataSize + i];
}

real_t DBSCAN::calcDist(unsigned int i, unsigned int j)
{
  real_t l2dist = 0.0;
  for( unsigned int k = 0; k < dataDim; k++ )
  {
      l2dist += pow(Data[i].getFeatures()[k] - Data[j].getFeatures()[k], 2);
  }
  return sqrt(l2dist);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
// Main methods 								  //
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //

std::vector< unsigned int > DBSCAN::QR(unsigned int i)
{ 
  std::vector< unsigned int > seedList;
  for( unsigned int j = 0; j < dataSize; j++ )
  {
    //std::cout << "i: " << i << " j: " << j << "  dist: " << getDist(i, j) << " epsilon: " << epsilon <<  " idx: " << i * dataSize + j << std::endl;
    bool d = false;
    if ( !calcDistMatB ) {
      d = getDist(i, j) <= epsilon and i != j;
    }
    else {
      d = calcDist(i, j) <= epsilon and i != j;
    }
    
    if ( d ) seedList.push_back(j);

  }
  return seedList;
}

void DBSCAN::setStartIdx(const unsigned int & idx)
{
  if ( idx < 0 || idx >= dataSize ) {
    fprintf(stderr, "%s", "DBSCAN error: invalid start idx!\n");
    exit(-1);
  }
  startIdx = idx;
  newStartIdx = true;
}


void DBSCAN::run(void)
{
  if ( newStartIdx ) {
    clusterDatapoint(Data[startIdx]);
  }
  for(auto & d : Data)
  {
    clusterDatapoint(d);
    /*
    if ( d.getState() == dataP::UNVISITED ) {
      
      std::vector< unsigned int > seedList = QR(d.getID());	// Get neighbour-list of current datapoint
      
      if ( seedList.size() + 1 < minPts ) {
	// Mark datapoint as NOISE
	d.setState(dataP::NOISE);
      }
      else {
	// d is a core-point and defines a new cluster
	d.setState(dataP::CLUSTERED);
	d.setClusterID(currentClusterID);
	
	expandCluster(seedList);
	currentClusterID++;
      }
      
      num_iters++;
      
    }
    */
  }
}

void DBSCAN::clusterDatapoint( dataP::Datapoint & d )
{
  if ( d.getState() == dataP::UNVISITED ) {

  std::vector< unsigned int > seedList = QR(d.getID());	// Get neighbour-list of current datapoint

  if ( seedList.size() + 1 < minPts ) {
    // Mark datapoint as NOISE
    d.setState(dataP::NOISE);
  }
  else {
    // d is a core-point and defines a new cluster
    d.setState(dataP::CLUSTERED);
    d.setClusterID(currentClusterID);
    
    expandCluster(seedList);
    currentClusterID++;
  }

  num_iters++;
  }
}

void DBSCAN::expandCluster(std::vector< unsigned int > & seedList)
{
  // Add all neighbours in the seedList to the new Cluster
  for ( auto & j : seedList )
  {
    Data[j].setState(dataP::CLUSTERED);
    Data[j].setClusterID(currentClusterID);
  }
  
  for ( unsigned int i = 0; i < seedList.size(); i++ )
  {
    unsigned int nPos = seedList[i];

    std::vector< unsigned int > seedList2 = QR(nPos);  	// Get new seedList of current neighbour
    if ( seedList2.size() + 1 >= minPts ) {		// Current neighbour is also a core-point:

      for ( auto & j : seedList2 )
      {
	dataP::ClusterState nPosState = Data[j].getState();
	
	if ( nPosState == dataP::UNVISITED || nPosState == dataP::NOISE ){
	  
	  // Add nPos to seed list if is is not clustered or marked as noise
	  // Datapoints already marked as "noise" are at the border of a cluster
	  if ( nPosState == dataP::UNVISITED ) seedList.push_back(j);		
	  
	  Data[j].setState(dataP::CLUSTERED);
	  Data[j].setClusterID(currentClusterID);
	  
	}
      }
    }
    
    num_iters++;
  }
    
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
// Show methods 								  //
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //

const void DBSCAN::showClustering(void) const
{
  for ( auto & i : Data )
  {
    std::cout << "Data ID: " << i.getID() << "\t,ClusterState: " << i.getState() << "\t,ClusterID: " <<  i.getClusterID() << std::endl;
  }
}

const void DBSCAN::showInfo(void) const
{
  printf("################# INFO #####################\n");
  printf("# minPts: %i\n", minPts);
  printf("# epsilon: %f\n", epsilon);
  printf("# External distance matrix: %s\n", !calcDistMatB ? "True" : "False" );
  printf("# Datasize: %i\n", dataSize);
  printf("# DataDim: %i\n", dataDim);
  printf("# Number of Clusters: %u\n", currentClusterID);
  printf("# Number of iterations: %u\n", num_iters);
  printf("################# END INFO #################\n");
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
// Getter methods 								  //
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //


const std::vector< dataP::Datapoint > & DBSCAN::getClustering( void ) const
{
  return Data;
}


std::vector< unsigned int > DBSCAN::getDataIDs(void) const
{
  std::vector< unsigned int > dataIDs;
  for ( auto & i : Data )
  {
    dataIDs.push_back(i.getID());
  }
  return dataIDs;
}
std::vector< int > DBSCAN::getClusterIDs(void) const
{
  std::vector< int > dataIDs;
  for ( auto & i : Data )
  {
    dataIDs.push_back(i.getClusterID());
  }
  return dataIDs;
}