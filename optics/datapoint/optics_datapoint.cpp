#include "optics_datapoint.h"

using namespace dataP;

const real_t OpticsDatapoint::INF = std::numeric_limits<real_t>::max();

OpticsDatapoint::OpticsDatapoint(const real_t* featureArray, unsigned int feature_size)
: Datapoint(featureArray, feature_size), reachabilityDist( INF ), coreDist( INF ), isSeed( false ){}

OpticsDatapoint::~OpticsDatapoint(void) {}

// Getter methods
const real_t & OpticsDatapoint::getReachabilityDist(void) const
{
  return reachabilityDist;
}

const real_t & OpticsDatapoint::getCoreDist(void) const
{
  return coreDist;
}

const real_t & OpticsDatapoint::getTmpDist(void) const
{
  return tmpDist;
}

const bool & OpticsDatapoint::getSeedState(void) const
{
  return isSeed;
}

// Setter methods
void OpticsDatapoint::setReachabilityDist(const real_t &d)
{
  reachabilityDist = d;
}

void OpticsDatapoint::setCoreDist(const real_t &d)
{
  coreDist = d;
}

void OpticsDatapoint::setSeedState(const bool & state)
{
  isSeed = state;
}

void OpticsDatapoint::setTmpDist(const real_t & d)
{
  tmpDist = d;
}


bool compareOpticsDatapoints::operator()(const OpticsDatapoint * lhs, const OpticsDatapoint * rhs) const
{ 
  // The comarator is not compatible with the std library !!!!!!!!!!!!!!!!!!!!!!!!
  // Only use with boost-data-heap class !!!!!!!!!!!!!!!!!!!!!!!!
  return lhs->getReachabilityDist() > rhs->getReachabilityDist();
}

bool compareOpticsDatapoints_by_tmpDist::operator()(const OpticsDatapoint * lhs, const OpticsDatapoint * rhs) const
{ 
  return lhs->getTmpDist() < rhs->getTmpDist();
}

