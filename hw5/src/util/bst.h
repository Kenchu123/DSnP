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
      // _root = new BSTreeNode<T>(T());
      // _root->_l = _root->_r = 0;
      _root = 0;
      _dummy = new BSTreeNode<T>(T("dummy"));
      _dummy->_l = _dummy->_r = _root;
   }
   ~BSTree() { clear(); delete _root; }

   class iterator
   {
      friend class BSTree;

   public:
      iterator():_own(0), _node(0) {}

      iterator(const BSTree<T>* b, BSTreeNode<T>* n = 0): _own(b), _node(n) {
         _trace.push_back({b->_dummy, LEFT});
         if (_node == 0) _node = b->_root;
         _addTrace(_node, b->_root);
      }

      iterator(const iterator& i) : _own(i._own), _node(i._node), _trace(i._trace) {}
      ~iterator() { _trace.clear(); }

      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {
         // cout << "iterator++ trace: " << endl;
         // for (auto i : _trace) {
         //    cout << i.first->_data << " " << i.second << endl;
         // }
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
            else { // iterator to end() // maybe not used
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
         _trace.clear();
         if (_own->_root == 0) return end();
         _trace.push_back({_own->_dummy, LEFT});
         _node = _own->_root;
         _findMin(_node);
         return (*this);
      }

      iterator& end() {
         _trace.clear();
         _node = _own->_dummy;
         _trace.push_back({_node, CUR});
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
      // cout << "Inserting " << x << endl;
      if (_size == 0) {
         _root = new BSTreeNode<T>(x);
         _dummy->_l = _dummy->_r = _root;
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

   void pop_front() {
      if (_size == 0) return;
      erase(begin());
   }
   void pop_back() {
      if (_size == 0) return;
      erase(--end());
   }

   bool erase(iterator pos) {
      BSTreeNode<T>* tar = pos._node; // target node
      // cout << endl << "iterator erase " << pos._node->_data << endl;
      // for (auto i : pos._trace) cout << i.first->_data << " " << i.second << endl;
      if (tar == _dummy || tar == 0) return false;
      if (tar->_r) { // if have right child
         pos._trace.back().second = RIGHT;
         pos._node = pos._node->_r;
         pos._findMin(pos._node);
         tar->_data = pos._node->_data; // exchange the data with right subtree's min
         tar = pos._node;
         // for (auto i : pos._trace) cout << i.first->_data << " " << i.second << endl;
      }
      pos._trace.pop_back();
      pair<BSTreeNode<T>*, TrStat> par = pos._trace.back(); // get parent
      if (par.second == LEFT) par.first->_l = tar->_l == 0 ? tar->_r : tar->_l;
      else if (par.second == RIGHT) par.first->_r = tar->_l == 0 ? tar->_r : tar->_l;
      if (tar == _root) {
         _root = tar->_l;
         _dummy->_l = _dummy->_r = _root;
      }
      delete tar;
      _size -= 1;
      return true;
   }
   bool erase(const T& x) {
      iterator found = find(x);
      if (found._node != _dummy) return erase(found);
      return false;
   }

   iterator find(const T& x) {
      return find(x, _root);
   }

   iterator find(const T& x, BSTreeNode<T>* com) {
      // cout << "Finding " << x << endl;
      if (com == 0) return end();
      if (x < com->_data) return find(x, com->_l);
      else if (x > com->_data) return find(x, com->_r);
      else return iterator(this, com);
   }

   void clear() {
      while (_root != 0) pop_back();
   }  

   void sort() const { } // do nothing because BST!

   void print() const { // for verbose

   }

private:
   BSTreeNode<T>*  _root;
   BSTreeNode<T>*  _dummy; // for end()
   size_t _size = 0;
};

#endif // BST_H
