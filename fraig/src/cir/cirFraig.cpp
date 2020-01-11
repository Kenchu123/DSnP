/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <functional>
#include <unordered_map>
#include <math.h>
#include <bitset>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
  unordered_map<unsigned long long, CirGate*> mp;
  for (auto& g : _dfslist) {
    if (g->getType() != AIG_GATE) continue;
    assert(g->_fanin.size() == 2);
    unsigned long long hash = _hashFanin(g);
    unordered_map<unsigned long long, CirGate*>::const_iterator found = mp.find(hash);
    if (found == mp.end()) { // don't need to merge
      mp[hash] = g;
    }
    else {
      _mergeGate(found->second, g);
    }
  }
  _dfslist.clear();
  genDFSList();
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

unsigned long long CirMgr::_hashFanin(CirGate* g) {
  assert(g->_fanin.size() == 2);
  CirGateV& A = g->_fanin[0];
  CirGateV& B = g->_fanin[1];
  unsigned x = A.gate()->getVar() * 2 + A.inv();
  unsigned y = B.gate()->getVar() * 2 + B.inv();
  // unorder paring function
  if (x > y) swap(x, y);
  unsigned long long hashFanin = ((unsigned long long)x << 32) | y;
  return hashFanin;
}

void CirMgr::_mergeGate(CirGate* A, CirGate* B) {
  cout << "Strashing: " << A->getVar() << " merging " << B->getVar() << "..." << endl;
  for (auto& Bout : B->_fanout) {
    A->_fanout.push_back(Bout);
    for (auto& BoutIn : Bout.gate()->_fanin) {
      if (BoutIn.gate() == B) BoutIn._gate = A;
    }
  }
  _removeGate(B->getVar());
}