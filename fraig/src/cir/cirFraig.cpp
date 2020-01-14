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
   SatSolver solver;
   solver.initialize();
   _genProofModel(solver);

   for (auto& g : _dfslist) {
     FecGrp* fg = g->_fecGrp;
     if (fg == 0) continue;
     assert(fg != 0);
     _fraigFec(fg, solver);
   }

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

void CirMgr::_genProofModel(SatSolver& s) {
  cout << "Generating Proof Model" << endl;
  for (auto& g : _dfslist) {
    switch (g->getType()) {
      case PI_GATE:
      case AIG_GATE:
      case CONST_GATE:
      case UNDEF_GATE:
        Var v = s.newVar(); g->setVar(v); break;
    }

    if (g->getType() == AIG_GATE) {
      assert(g->_fanin.size() == 2);
      Var& vf = g->_satVar,
          va = g->_fanin[0]._gate->_satVar,
          vb = g->_fanin[1]._gate->_satVar;
      bool fa = g->_fanin[0]._inv,
           fb = g->_fanin[1]._inv;
      s.addAigCNF(vf, va, fa, vb, fb);
    }
  }
}

void CirMgr::reportResult(const SatSolver& solver, bool& result)
{
  //  solver.printStats();
   cout << (result? "SAT" : "UNSAT") << endl;
  //  if (result) {
  //     for (size_t i = 0, n = _dfslist.size(); i < n; ++i)
  //        cout << _dfslist[i]->getVar() << " " << solver.getValue(_dfslist[i]->getSatVar()) << ", ";
  //  }
  //  cout << endl;
}

void CirMgr::_fraigFec(FecGrp*& fg, SatSolver& solver) {
  if (fg->_isFraig) return;

  unsigned A = fg->_child.begin()->second._gate->getVar();
  const Var& va = fg->_child.begin()->second._gate->getSatVar();
  bool fa = fg->_child.begin()->second._inv;

  for (auto it = ++fg->_child.begin(), itl = fg->_child.end(); it != itl; ++it) {
    bool re;
    const Var& vb = it->second._gate->getSatVar();
    unsigned B = it->second._gate->getVar();
    bool fb = it->second._inv; 

    cout << "Sat..." << "(" << (fa ? "!" : "") << A << "," << (fb ? "!" : "") << B <<  ")" << endl;
    Var newV = solver.newVar();
    solver.addXorCNF(newV, va, fa, vb, fb);
    solver.assumeRelease();
    solver.assumeProperty(newV, true);
    re = solver.assumpSolve();
    reportResult(solver, re);
  }
  
  fg->_isFraig = true;
  
}