/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next; return *(this); }
      iterator operator ++ (int) { iterator ret = *this; _node = _node->_next; return ret; }
      iterator& operator -- () { _node = _node->_prev; return *(this); }
      iterator operator -- (int) { iterator ret = *this; _node = _node->_prev; return ret; }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return _node != i._node; }
      bool operator == (const iterator& i) const { return _node == i._node; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head); }
   iterator end() const { return iterator(_head->_prev); } // node begin()'s prev
   bool empty() const { return _head == _head->_next; }
   size_t size() const {
      iterator i = begin(), j = end();
      size_t _s = 0;
      while (i++ != j) { ++_s; }
      return _s;
   }

   void push_back(const T& x) {
      if (_head->_next == _head) { // _head is a dummy node
         DListNode<T>* tmp = new DListNode<T>(x);
         tmp->_next = _head;
         tmp->_prev = _head;
         _head->_prev = tmp;
         _head->_next = tmp;
         _head = tmp;
         return;
      }
      DListNode<T>* end = _head->_prev;
      DListNode<T>* tmp = new DListNode<T>(x);
      end->_prev->_next = tmp;
      tmp->_prev = end->_prev;
      tmp->_next = end;
      end->_prev = tmp;
      if (tmp->_data < tmp->_prev->_data) _isSorted = false;
      return;
   }
   void pop_front() {
      if (_head->_next == _head) return;
      DListNode<T>* tmp = _head->_next;
      DListNode<T>* end = _head->_prev;
      end->_next = tmp;
      tmp->_prev = end;
      delete _head;
      _head = tmp;
      return;
   }
   void pop_back() {
      if (_head->_next == _head) return; // size = 0
      if (_head->_next == _head->_prev) return pop_front(); // size = 1
      DListNode<T>* end = _head->_prev;
      DListNode<T>* tmp = end->_prev->_prev;
      delete end->_prev;
      end->_prev = tmp;
      tmp->_next = end;
      return;
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      if (pos == end()) return false;
      DListNode<T> *cur = pos._node;
      cur->_prev->_next = cur->_next;
      cur->_next->_prev = cur->_prev;
      if (_head == cur) _head = cur->_next;
      delete cur;
      return true;
   }
   bool erase(const T& x) {
      iterator pos = find(x);
      if (pos != end()) return erase(pos);
      return false;
   }

   iterator find(const T& x) {
      iterator i = begin();
      iterator j = end();
      for (; i != j; ++i) if (x == *i) return i;
      return j;
   }

   void clear() {// delete all nodes except for the dummy node
      while (!empty()) pop_front();
      _isSorted = false;
      _head->_prev = _head->_next = _head;
   }  

   void sort() const {
      if (_isSorted) return;
      DListNode<T>* j = _head->_prev;
      size_t _s = size();
      if (_s == 0) return;
      iterator be = begin(), en = --end();
      for (; be != en; ++be, j = j->_prev) {
         for (DListNode<T>* i = _head; i->_next != j; i = i->_next) {
            if (i->_data > i->_next->_data) {
               T tmp = i->_data;
               i->_data = i->_next->_data;
               i->_next->_data = tmp;
            }
         }
      }
      _isSorted = true;
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
