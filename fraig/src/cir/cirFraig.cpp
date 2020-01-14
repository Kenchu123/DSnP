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
#include <queue>
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
    }
  }
  genDFSList();
}

void
CirMgr::fraig()
{
   SatSolver solver;
   solver.initialize();
   _genProofModel(solver);

  queue<unsigned> fgQue;
  for (size_t i = 0, n = _pilist.size(); i < n; ++i) {
    if (_pilist[i]->_inDFSlist)
      fgQue.push(_pilist[i]->_var);
  }
  while (!fgQue.empty()) {
    if (_fecGrps.empty()) break;
    unsigned gateVar = fgQue.front();
    fgQue.pop();
    auto found = _gatelist.find(gateVar);

    CirGate* g = 0;
    if (found == _gatelist.end()) { continue; }
    else g = found->second;
    if (g == 0) { fgQue.pop(); continue; }
    assert(g != 0);
    // push g's fanout to queueu (BFS)
    for (auto outV : g->_fanout) {
      if (outV._gate->_inDFSlist)
        fgQue.push(outV._gate->_var);
    }
    if (g->_fecGrp == 0) { continue; }
    
    assert(g->_fecGrp != 0);
    // cout << "Running " << g->_var << "'s fecGrp" << endl;
    if (!_fraigFec(g->_fecGrp, solver)) fgQue.push(g->_var);
  }
  strash();
  _initfec = false;
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
}

bool CirMgr::_fraigFec(FecGrp* fg, SatSolver& solver) {
  // cout << "_fraigFEC#163: " << endl;
  if (fg->_isFraig) {
    // cout << "Merging this group!!" << endl;
    _mergeFecGrp(fg);
    genDFSList();
    cout << "Updating by UNSAT...Total #FEC Group = " << _fecGrps.size() << endl;
    return true;
  }

  unsigned A = fg->_child.begin()->second._gate->getVar();
  const Var& va = fg->_child.begin()->second._gate->getSatVar();
  bool fa = fg->_child.begin()->second._inv;

  bool doOnlySim = 0;
  _satCnt = 0;
  // size_t satLimit = fg->_child.size() / 2 < 63 ? fg->_child.size() : 63;
  size_t satLimit = 62;
  for (auto it = ++fg->_child.begin(), itl = fg->_child.end(); it != itl; ++it) {
    bool re;
    const Var& vb = it->second._gate->getSatVar();
    unsigned B = it->second._gate->getVar();
    bool fb = it->second._inv; 

    // cout << "Proving..." << "(" << (fa ? "!" : "") << A << "," << (fb ? "!" : "") << B <<  ")" << endl;
    Var newV = solver.newVar();
    solver.addXorCNF(newV, va, fa, vb, fb);
    solver.assumeRelease();
    solver.assumeProperty(newV, true);
    re = solver.assumpSolve();
    // reportResult(solver, re);

    if (re) { // SAT
      // cout << "Find a SAT, SatCnt = " << _satCnt << endl;
      // Collect Pattern
      _collectPattern(solver);
      ++_satCnt;
      if (_satCnt == satLimit) {
        doOnlySim = 1;
        break;
      }
    }
    // else cout << "UNSAT" << endl;
  }
  if (_satCnt != 0) {
    // cout << "Simulate with new pattern!!" << endl;
    _simPattern(_pat);
    if (!doOnlySim) fg->_isFraig = 1;
    _genfecGrp();
    _pat.clear();
    _satCnt = 0;
    cout << "Updating by SAT...Total #FEC Group = " << _fecGrps.size() << endl;
    return false;
  }

  if (_satCnt == 0) { // Merge this fgGrp
    // cout << "Merging this group!!" << endl;
    _mergeFecGrp(fg);
    genDFSList();
    cout << "Updating by UNSAT...Total #FEC Group = " << _fecGrps.size() << endl;
    return true;
  }

  return false;
}

void CirMgr::_collectPattern(const SatSolver& solver) {
  // cout << "collectattern#209" << endl;
  if (_pat.empty()) for (size_t i = 0;i < _I; ++i) _pat.push_back(0);
  assert(_pat.size() == _I);
  for (size_t i = 0;i < _I; ++i) {
    // cout << _pat[i] << " ";
    _pat[i] |= ((size_t)solver.getValue(_pilist[i]->_satVar) << _satCnt);
  }
  // cout << "Check Const Value: " << solver.getValue(Const0->_satVar) << endl;
  // cout << endl;
}

void CirMgr::_mergeFecGrp(FecGrp*& fg) {
  if (fg == 0) return;
  assert(fg != 0);
  CirGate* base = fg->_child.begin()->second._gate;
  for (auto it = ++fg->_child.begin(); it != fg->_child.end();) {
    cout << "Fraig: " << base->_var << " merging "<< (it->second._inv ? "!" : "") << it->second._gate->_var << "..." << endl;
    it->second._gate->setFecGrp(0);
    _mergeGate(base, it->second._gate, it->second._inv);
    it = fg->_child.erase(it);
  }
  base->setFecGrp(0);
  fg->_child.clear();

  for (size_t i = 0;i < _fecGrps.size(); ++i) {
    if (_fecGrps[i] == fg) {
      // cout << "Erasing fg from _fecGrps" << endl;
      _fecGrps.erase(_fecGrps.begin() + i); break;
    }
  }
  delete fg;
  fg = 0;
}