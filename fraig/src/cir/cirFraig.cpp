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
      cout << "Strashing: " << found->second->getVar() << " merging " << g->getVar() << "..." << endl;
      _mergeGate(found->second, g);
      // _removeGate(g->getVar());
    }
  }
  // _dfslist.clear();
  genDFSList();
}

void
CirMgr::fraig()
{
   SatSolver solver;
   solver.initialize();
   _genProofModel(solver);

  while (!_fecGrps.empty()) {
    cout << "fraig()#68: " << _fecGrps.size() << endl;
    bool reGenDfs = 0;
    for (auto& fg : _fecGrps) fg->_isFraig = 0; // reset isFraig
    for (auto& g : _dfslist) {
      cout << "fraig()#72" << endl;
      FecGrp* fg = g->_fecGrp;
      if (fg == 0) continue;
      assert(fg != 0);
      reGenDfs = _fraigFec(fg, solver);
      if (reGenDfs) break;
    }
    if (reGenDfs) genDFSList();

    // For Debug
    if (_fecGrps.size() == 1) {
      printFECPairs();
      break;
    }
    if (_pat.size() == _I) {
      cout << "Simulating #87" << endl;
      _simPattern(_pat);
      _genfecGrp();
    }
  }
  // strash();

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

void CirMgr::_mergeGate(CirGate* A, CirGate* B, bool inv) {
  for (auto& Bout : B->_fanout) {
    A->_fanout.push_back(Bout);
    A->_fanout.back()._inv ^= inv;
    for (auto& BoutIn : Bout.gate()->_fanin) {
      if (BoutIn.gate() == B) {
        BoutIn._gate = A;
        BoutIn._inv ^= inv;
      }
    }
  }
  _removeGate(B->_var);
}

void CirMgr::_genProofModel(SatSolver& s) {
  s.reset();
  cout << "Generating Proof Model" << endl;
  Var  vf = s.newVar();
  Const0->setVar(vf);
  Var  va = s.newVar();
  Var  vb = va;
  bool fa = 0;
  bool fb = 1;
  s.addAigCNF(vf, va, fa, vb, fb);

  for (auto& g : _dfslist) {
    switch (g->getType()) {
      case PI_GATE:
      case AIG_GATE:
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

bool CirMgr::_fraigFec(FecGrp*& fg, SatSolver& solver) {
  cout << "_fraigFEC#163: " << endl;
  if (fg->_isFraig) return false;

  unsigned A = fg->_child.begin()->second._gate->getVar();
  const Var& va = fg->_child.begin()->second._gate->getSatVar();
  bool fa = fg->_child.begin()->second._inv;

  bool allUnSat = 1, doSim = 0;
  for (auto it = ++fg->_child.begin(), itl = fg->_child.end(); it != itl; ++it) {
    bool re;
    const Var& vb = it->second._gate->getSatVar();
    unsigned B = it->second._gate->getVar();
    bool fb = it->second._inv; 

    cout << "#178Proving..." << "(" << (fa ? "!" : "") << A << "," << (fb ? "!" : "") << B <<  ")" << endl;
    Var newV = solver.newVar();
    solver.addXorCNF(newV, va, fa, vb, fb);
    solver.assumeRelease();
    solver.assumeProperty(newV, true);
    re = solver.assumpSolve();
    // reportResult(solver, re);

    if (re) { // SAT
      cout << "Find a SAT, SatCnt = " << _satCnt << endl;
      allUnSat = 0;
      // Collect Pattern
      _collectPattern(solver);
      ++_satCnt;
      if (_satCnt == 64) {
        doSim = 1;
        _satCnt = 0;
        break;
      }
    }
    else cout << "UNSAT" << endl;
  }

  if (allUnSat) { // Merge this fgGrp
    cout << "Merging this group!!" << endl;
    _mergeFecGrp(fg);
    // genDFSList();
    cout << "Updating by UNSAT...#Total FEC Group = " << _fecGrps.size() << endl;
    return true;
  }
  if (doSim) { // sim with new pattern
    cout << "Simulate with new pattern!!" << endl;
    for (size_t i = 0; i < _pat.size(); ++i) cout << _pat[i] << " ";
    cout << endl;
    _simPattern(_pat);
    _genfecGrp();
    _pat.clear();
    cout << "Updating by SAT...#Total FEC Group = " << _fecGrps.size() << endl;
    return false;
  }

  if (!allUnSat) fg->_isFraig = true;
  return false;
}

void CirMgr::_collectPattern(const SatSolver& solver) {
  cout << "collectattern#209" << endl;
  if (_pat.empty()) for (size_t i = 0;i < _I; ++i) _pat.push_back(0);
  assert(_pat.size() == _I);
  for (size_t i = 0;i < _I; ++i) {
    // cout << _pat[i] << " ";
    _pat[i] |= ((size_t)solver.getValue(_pilist[i]->_satVar) << _satCnt);
  }
  cout << "Check Const Value: " << solver.getValue(Const0->_satVar) << endl;
  // cout << endl;
}

void CirMgr::_mergeFecGrp(FecGrp*& fg) {
  assert(fg != 0);
  CirGate* base = fg->_child.begin()->second._gate;
  for (auto it = ++fg->_child.begin(); it != fg->_child.end();) {
    cout << "Fraig: " << base->_var << " merging... "<< (it->second._inv ? "!" : "") << it->second._gate->_var << endl;
    // it->second._gate->setFecGrp(0);
    _mergeGate(base, it->second._gate, it->second._inv);
    it = fg->_child.erase(it);
  }
  base->setFecGrp(0);
  fg->_child.clear();

  for (size_t i = 0;i < _fecGrps.size(); ++i) {
    if (_fecGrps[i] == fg) {
      cout << "Erasing fg from _fecGrps" << endl;
      _fecGrps.erase(_fecGrps.begin() + i); break;
    }
  }
  delete fg;
  fg = 0;
}