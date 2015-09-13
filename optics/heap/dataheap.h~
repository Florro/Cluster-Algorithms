#ifndef DATAHEAP_H
#define DATAHEAP_H

#include "optics_datapoint.h"
#include <boost/heap/fibonacci_heap.hpp>

using BoostHeap = boost::heap::fibonacci_heap< dataP::OpticsDatapoint *, boost::heap::compare< dataP::compareOpticsDatapoints > >;

struct Heap{
  
  Heap(void);
  Heap(const unsigned int & dataSize);
  virtual ~Heap(void);
  
  void reserve(unsigned int & dataSize);
  
  void push( dataP::OpticsDatapoint * );
  void clear(void);
  void increase( dataP::OpticsDatapoint * d );
  void decrease( dataP::OpticsDatapoint * d );
  void update( dataP::OpticsDatapoint * d );
  void pop(void);
  
  unsigned int size(void) const;
  bool empty(void) const;
  dataP::OpticsDatapoint * const & top(void) const;
  dataP::OpticsDatapoint * pop_top(void);
    
private:
  
  BoostHeap minHeap;
  std::vector< BoostHeap::handle_type > handles;
  
};


#endif
