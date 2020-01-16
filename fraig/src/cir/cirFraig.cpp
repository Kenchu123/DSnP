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
#include <unordered_set>
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
  unordered_set<unsigned> inQue;
  _satCnt = 0;
  
  _fecToMerge.clear();
  for (size_t i = 0, n = _pilist.size(); i < n; ++i) {
    if (_dfsSet.find(_pilist[i]->_var) != _dfsSet.end()) {
      fgQue.push(_pilist[i]->_var);
      inQue.insert(_pilist[i]->_var);
    }
  }
  while (!fgQue.empty()) {
    if (_fecGrps.empty()) break;
    unsigned gateVar = fgQue.front();
    fgQue.pop();
    inQue.erase(gateVar);

    auto found = _gatelist.find(gateVar);
    CirGate* g = 0;
    if (found == _gatelist.end()) { continue; }
    else g = found->second;
    if (g == 0) { continue; }
    assert(g != 0);
    // push g's fanout to queueu (BFS)
    for (auto outV : g->_fanout) {
      if (outV._gate->_gateType == AIG_GATE && inQue.count(outV._gate->_var) == 0) {
        fgQue.push(outV._gate->_var);
        inQue.insert(outV._gate->_var);
      }
    }
    if (g->_fecGrp == 0) { continue; }
    
    assert(g->_fecGrp != 0);
    // cout << "Running " << g->_var << "'s fecGrp" << endl;
    if (_fecToMerge.count(g->_fecGrp)) { continue; }
    if (!_fraigFec(g->_fecGrp, solver)) {
      fgQue.push(g->_var);
      inQue.insert(g->_var);
    }
  }
  if (_satCnt != 0) {
    _simPattern(_pat);
    _genfecGrp();
    _pat.clear();
    _satCnt = 0;
    cout << "Updating by SAT... Total #FEC Group = " << _fecGrps.size() << endl;
  }
  if (!_fecGrps.empty()) {
    for (auto fg : _fecGrps) _fecToMerge.insert(fg);
  }
  if (!_fecToMerge.empty()) {
    for (auto fg = _fecToMerge.begin(); fg != _fecToMerge.end(); ++fg) {
      _mergeFecGrp(*fg);
    }
    genDFSList();
  }
  _fecToMerge.clear();
  
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
  // cout << "Generating Proof Model" << endl;
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

bool CirMgr::_fraigFec(FecGrp* fg, SatSolver& solver) {
  // if (_fecToMerge.count(fg)) return true;

  unsigned A = fg->_child.begin()->second._gate->getVar();
  const Var& va = fg->_child.begin()->second._gate->getSatVar();
  bool fa = fg->_child.begin()->second._inv;

  bool doSim = 0;
  bool allUnSat = 1;

  size_t satLimit = 64;
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

    if (re) { // SAT
      // cout << "Find a SAT, SatCnt = " << _satCnt << endl;
      allUnSat = 0;
      // Collect Pattern
      _collectPattern(solver);
      if (_satCnt == satLimit) {
        doSim = 1;
        break;
      }
    }
  }
  
  if (doSim) {
    // cout << "Simulate with new pattern!!" << endl;
    _simPattern(_pat);
    _genfecGrp();
    _pat.clear();
    _satCnt = 0;
    _duPat.clear();
    cout << "Updating by SAT... Total #FEC Group = " << _fecGrps.size() << endl;

    // merge fecGrp that is Fraig
    for (auto it = _fecToMerge.begin(), itl = _fecToMerge.end(); it != itl; ++it) {
      _mergeFecGrp(*it);
    }
    _fecToMerge.clear();
    genDFSList();
    return false;
  }

  if (allUnSat) {
    // cout << "all Un Sat" << endl;
    _mergeFecGrp(fg);
    genDFSList();
    return true;
  }
  _fecToMerge.insert(fg); // not all sat and don't need to simulate

  return false;
}

void CirMgr::_collectPattern(const SatSolver& solver) {
  string patS = "";
  if (_pat.empty()) for (size_t i = 0;i < _I; ++i) _pat.push_back(0);

  assert(_pat.size() == _I);
  for (auto& g : _pilist) {
    size_t re = solver.getValue(g->_satVar);
    patS += (re == 0 ? "0" : "1");
  }
  // cout << "Get Pattern: " << patS << endl;
  if (_duPat.count(patS) == 0) { // no duplicate
    // cout << "it's a new Pattern: ";
    for (size_t i = 0;i < _I; ++i) {
      if (patS[i] == '1') _pat[i] |= ((size_t)1 << _satCnt);
      // cout << _pat[i] << " ";
    }
    // cout << endl;
    _duPat.insert(patS);
    ++_satCnt;
  }
  // cout << "Check Const Value: " << solver.getValue(Const0->_satVar) << endl;
  return;
}

void CirMgr::_mergeFecGrp(FecGrp* fg) {
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
  cout << "Updating by UNSAT... Total #FEC Group = " << _fecGrps.size() << endl;
}