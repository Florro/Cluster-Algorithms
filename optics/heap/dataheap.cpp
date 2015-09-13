#include "dataheap.h"

Heap::Heap(void)
{
}

Heap::Heap(const unsigned int & dataSize)
{
  handles.reserve(dataSize);
}

Heap::~Heap(void)
{
}

void Heap::reserve(unsigned int & dataSize)
{
    handles.reserve(dataSize);
}

void Heap::push( dataP::OpticsDatapoint * d )
{
  d->setSeedState(true);
  handles[d->getID()] = minHeap.push(d);
}

void Heap::clear(void)
{
  minHeap.clear();
}

void Heap::increase( dataP::OpticsDatapoint * d )
{
  minHeap.increase(handles[d->getID()]);
}

void Heap::decrease( dataP::OpticsDatapoint * d )
{
  minHeap.decrease(handles[d->getID()]);
}

void Heap::update( dataP::OpticsDatapoint * d )
{
  minHeap.update(handles[d->getID()]);
}

void Heap::pop(void)
{
  minHeap.top()->setSeedState(false);
  minHeap.pop();
}

unsigned int Heap::size(void) const
{
  return minHeap.size();
}

bool Heap::empty(void) const
{
  return minHeap.empty();
}

dataP::OpticsDatapoint * const & Heap::top(void) const
{
  return minHeap.top();
}

dataP::OpticsDatapoint * Heap::pop_top(void)
{
  minHeap.top()->setSeedState(false);
  dataP::OpticsDatapoint * tmp = minHeap.top();
  minHeap.pop();
  
  return tmp;
}

