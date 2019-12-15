/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator() {}
      iterator(const iterator& i) : _hashSet(i._hashSet), _ind(i._ind), _it(i._it) {}
      iterator(const HashSet<Data>* hashSet, size_t ind, typename vector<Data>::iterator it)
         :_hashSet(hashSet), _ind(ind), _it(it) {}
      // iterator(const HashSet<Data>* hashSet, Data d): _hashSet(hashSet) {
      // }
      const Data& operator * () const { return *_it; }
      iterator& operator ++ () {
         ++_it;
         if (_it == _hashSet->_buckets[_ind].end()) {
            for (size_t i = _ind + 1; i < _hashSet->_numBuckets; ++i) {
               if (!_hashSet->_buckets[i].empty()) {
                  _ind = i; _it = _hashSet->_buckets[i].begin();
                  break;
               }
            }
         }
         return (*this);
      }
      iterator operator ++ (int) { iterator ret(*this); ++(*this); return ret; }
      iterator& operator -- () {
         if (_it == _hashSet->_buckets[_ind].begin()) {
            for (size_t i = _ind - 1; i >= 0; --i) {
               if (!_hashSet->_buckets[i].empty()) {
                  _ind = i; _it = --(_hashSet->_buckets[i].end());
                  break;
               }
            }
         }
         else --_it;
         return (*this);
      }
      iterator operator -- (int) { iterator ret(*this); --(*this); return ret; }
      bool operator != (const iterator& i) const { return _it != i._it; }
      bool operator == (const iterator& i) const { return _it == i._it; }
      iterator& operator = (const iterator& i) const {
          _hashSet = i._hashSet;
          _ind = i._ind; _it = i._it;
          return (*this);
      }
   private:
      const HashSet<Data>* _hashSet;
      typename vector<Data>::iterator _it;
      size_t _ind;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      for (size_t i = 0; i < _numBuckets; ++i) {
         if (!_buckets[i].empty()) {
            return iterator(this, i, _buckets[i].begin());
         }
      }
      return iterator(this, 0, _buckets[0].begin());
   }
   // Pass the end
   iterator end() const {
      for (int i = _numBuckets - 1; i >= 0; i--) {
         if (!_buckets[i].empty()) {
            return iterator(this, i, _buckets[i].end());
         }
      }
      return begin();
   }
   // return true if no valid data
   bool empty() const { return size() == 0; }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      for (auto i : _buckets) s += i.size();
      return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      size_t hash = d() % _numBuckets;
      if (_buckets[hash].empty()) return false;
      for (auto data : _buckets[hash])
         if (data == d) return true;
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      size_t hash = d() % _numBuckets;
      if (_buckets[hash].empty()) return false;
      for (auto& data : _buckets[hash])
         if (data == d) { d = data; return true; }
      return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
      size_t hash = d() % _numBuckets;
      if (!_buckets[hash].empty()) {
         for (auto& data : _buckets[hash])
            if (data == d) { data = d; return true; }
      }
      _buckets[hash].push_back(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      if (check(d)) return false;
      _buckets[d() % _numBuckets].push_back(d);
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
      if (!check(d)) return false;
      size_t hash = d() % _numBuckets;
      for (size_t i = 0;i < _buckets[hash].size(); ++i) {
         if (_buckets[hash][i] == d) {
            swap(_buckets[hash][i], _buckets[hash].back());
            break;
         }
      }
      _buckets[hash].pop_back();
      return true;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
