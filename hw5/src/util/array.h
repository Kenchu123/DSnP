/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0), _isSorted(0) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { ++_node; return (*this); }
      iterator operator ++ (int) { iterator ret = *this; ++(*this); return ret; }
      iterator& operator -- () { --_node; return (*this); }
      iterator operator -- (int) { iterator ret = *this; --(*this); return ret; }

      iterator operator + (int i) const { iterator ret = *this; ret._node += i; return ret; }
      iterator& operator += (int i) { *this = (*this) + i; return (*this); }

      iterator& operator = (const iterator& i) {
         this->_node = i._node;
         return (*this);
      }

      bool operator != (const iterator& i) const { return this->_node != i._node; }
      bool operator == (const iterator& i) const { return this->_node == i._node; }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { iterator ret(_data); return ret; }
   iterator end() const { return begin() + _size; }
   bool empty() const { return _size == 0; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) {
      if (_capacity == _size) { // increase the capacity
         _capacity = (_capacity == 0) ? 1 : _capacity * 2;
         T* tmp = new T[_capacity];
         for (size_t i = 0;i < _size; ++i) tmp[i] = _data[i];
         delete []_data;
         _data = tmp;
      }
      _data[_size++] = x;
   }

   void pop_front() {
      if (_size == 0) return;
      _data[0] = _data[--_size];
   }

   void pop_back() {
      if (_size == 0) return;
      --_size;
   }

   bool erase(iterator pos) {
      iterator e = end();
      if (pos == e) return false;
      *pos = *(--e);
      --_size;
      return true;
   }

   bool erase(const T& x) {
      iterator found = find(x);
      if (found == end()) return false;
      return erase(found);
   }

   iterator find(const T& x) {
      for (iterator i = begin(); i != end(); ++i) {
         if (x == *i) return i;
      }
      return end();
   }

   void clear() {
      _size = 0;
   }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
