/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include <vector>
#include <utility>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   public:
      friend class BSTree<T>;
      friend class BSTree<T>::iterator;

      BSTreeNode(const T& d,  BSTreeNode<T>* l = 0,  BSTreeNode<T>* r = 0):
         _data(d), _l(l), _r(r) {}
   private:
      BSTreeNode<T>* _l;
      BSTreeNode<T>* _r;
      T _data;
};

enum TrStat { // trace Status
   LEFT = 0, CUR, RIGHT
};


template <class T>
class BSTree
{
   public:
   BSTree() {
      _root = new BSTreeNode<T>(T());
      _root->_l = _root->_r = 0;
      _dummy = new BSTreeNode<T>(T());
      _dummy->_l = _dummy->_r = _root;
   }
   ~BSTree() { clear(); delete _root; }

   class iterator
   {
      friend class BSTree;

   public:
      iterator():_own(0), _node(0) {}

      iterator(const BSTree<T>* b, BSTreeNode<T>* n = 0): _own(b), _node(n) {
         if (_node == 0) _node = b->_root;
         _addTrace(_node, b->_root);
      }

      iterator(const iterator& i) : _own(i._own), _node(i._node), _trace(i._trace) {}
      ~iterator() { _trace.clear(); }

      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {
         _trace.pop_back();
         if (_node->_r != 0) { // trace right
            _trace.emplace_back(_node, RIGHT);
            _node = _node->_r;
            _findMin(_node);
         }
         else { // trace up
            while (!_trace.empty() && _trace.back().second != LEFT) _trace.pop_back();
            if (!_trace.empty()) {
              _node = _trace.back().first;
               _trace.back().second = CUR;
            }
            else { // iterator to end()
               return this->end();
            }
         }
         return (*this);
      }
      iterator operator ++ (int) { iterator ret(*this); ++(*this); return ret; }
      iterator& operator -- () {
         _trace.pop_back();
         if (_node->_l != 0) { // trace left
            _trace.emplace_back(_node, LEFT);
            _node = _node->_l;
            _findMax(_node);
         }
         else { // trace up
            while (!_trace.empty() && _trace.back().second != RIGHT)
               _trace.pop_back();
            if (!_trace.empty()) {
               _node = _trace.back().first;
               _trace.back().second = CUR;
            }
            else { // iterator to begin()
               return this->begin();
            }
         }
         return (*this);
      }
      iterator operator -- (int) { iterator ret(*this); --(*this); return ret; }

      iterator& operator = (const iterator& i) {
         _own = i._own;
         _node = i._node;
         _trace = i._trace;
         return *(this);
      }
      bool operator != (const iterator& i) const { return _node != i._node; }
      bool operator == (const iterator& i) const { return _node == i._node; }

      iterator& begin() {
         _node = _own->_root;
         _trace.clear();
         _findMin(_node);
         return (*this);
      }

      iterator& end() {
         _trace.clear();
         _node = _own->_dummy;
         return (*this);
      }

   private:
      BSTreeNode<T>* _node;
      vector< pair<BSTreeNode<T>*, TrStat> > _trace;
      const BSTree* _own;

      inline void _addTrace(BSTreeNode<T>* n, BSTreeNode<T>* par) {
         if (par == 0) return;
         if (n->_data < par->_data) { // left
            _trace.emplace_back(par, LEFT);
            _addTrace(n, par->_l);
         }
         else if (n->_data > par->_data) {
            _trace.emplace_back(par, RIGHT);
            _addTrace(n, par->_r);
         }
         else {
            _trace.emplace_back(par, CUR);
         }
      }

      inline void _findMin(BSTreeNode<T>*& n) {
         while (n->_l != 0) {
            _trace.emplace_back(n, LEFT);
            n = n->_l;
         }
         _trace.emplace_back(n, CUR);
      }

      inline void _findMax(BSTreeNode<T>*& n) {
         while (n->_r != 0) {
            _trace.emplace_back(n, RIGHT);
            n = n->_r;
         }
         _trace.emplace_back(n, CUR);
      }
   };

   iterator begin() const { return iterator(this).begin(); }
   iterator end() const { return iterator(this).end(); }
   bool empty() const { return _size == 0; }
   size_t size() const { return _size; }

   void insert(const T& x) {
      if (_size == 0) {
         _root->_data = x;
         _size += 1;
         return;
      }
      BSTreeNode<T>* newNode = new BSTreeNode<T>(x);
      insert(newNode, _root);
      _size += 1;
   }

   inline void insert(BSTreeNode<T>*& ch, BSTreeNode<T>*& par) {
      if (ch->_data < par->_data) {
         if (par->_l) insert(ch, par->_l);
         else par->_l = ch;
      }
      else {
         if (par->_r) insert(ch, par->_r);
         else par->_r = ch;
      }
   }

   void pop_front() { }
   void pop_back() { }

   bool erase(iterator pos) { return false; }
   bool erase(const T& x) { return false; }

   iterator find(const T& x) {

   }

   void clear() { }  

   void sort() const { } // do nothing because BST!

   void print() const { // for verbose

   }

private:
   BSTreeNode<T>*  _root;
   BSTreeNode<T>*  _dummy; // for end()
   size_t _size = 0;
};

#endif // BST_H
