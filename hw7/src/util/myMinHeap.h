/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) {
      size_t t = _data.size();
      _data.push_back(d);
      while (t > 0) {
         size_t p = (t - 1) / 2;
         if (_data[p] < d) break;
         _data[t] = _data[p];
         t = p;
      }
      _data[t] = d;
   }
   void delMin() {
      int p = 0, t = 1;
      while (t < _data.size()) {
         if (t < _data.size() - 1 && _data[t + 1] < _data[t]) ++t;
         if (_data.back() < _data[t]) break;
         _data[p] = _data[t];
         p = t;
         t = 2 * p + 1;  
      }
      _data[p] = _data.back();
      _data.pop_back();
   }
   void delData(size_t i) {
      int p = i, t = p * 2 + 1;
      while (t < _data.size()) {
         if (t < _data.size() - 1 && _data[t + 1] < _data[t]) ++t;
         if (_data.back() < _data[t]) break;
         _data[p] = _data[t];
         p = t;
         t = 2 * p + 1;  
      }
      _data[p] = _data.back();
      _data.pop_back();
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
