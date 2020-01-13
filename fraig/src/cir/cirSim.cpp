/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <bitset>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#define PATTERN_SIZE 64
#define toPatSize(t) (t % PATTERN_SIZE ? t + PATTERN_SIZE - t % PATTERN_SIZE : t)

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
  if (!_initfec) _initfecGrp();
}

void
CirMgr::fileSim(ifstream& patternFile)
{
  if (!_initfec) _initfecGrp();
  string pattern = "";
  _cnt = 0, _logCnt = 0;
  vector<size_t> vec; vec.resize(_I);
  // size_t* vec = new size_t(_I);
  // for (size_t i = 0;i < _I; ++i) vec[i] = (size_t)0;
  while (patternFile >> pattern) {
    if (pattern.size() == 0) break;

    if (pattern.size() != _I) {
      cerr << "Error: Pattern(" << pattern << ") length(" << pattern.size()\
        << ") does not match the number of inputs(" << _I << ") in a circuit!!" << endl;
      return;
    }
    for (size_t i = 0;i < _I; ++i) {
      if (pattern[i] != '0' && pattern[i] != '1') {
        cerr << "Error: Pattern(" << pattern << ") contains a non-0/1 character(\'" << pattern[i] << "\')." << endl;
        return;
      }
      if (pattern[i] == '1') vec[i] |= ((size_t)1 << (_cnt % 64));
    }
    ++_cnt;
    if (_cnt % 64 == 0) {
      _simPattern(vec);
      _genfecGrp();
      _genLog(vec);
      // for (size_t i = 0;i < _I; ++i) vec[i] &= 0;
      vec.clear(); vec.resize(_I);
    }
  }
  if (_cnt % 64) _simPattern(vec);
  _genfecGrp();
  _genLog(vec);
  cout << "Simulates " << _cnt << " patterns" << endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/

void CirMgr::_simPattern(vector<size_t>& pattern) {
  // change pi simvalue
  assert(pattern.size() == _I);
  for (size_t i = 0;i < pattern.size(); ++i) {
    _pilist[i]->setSimVal(pattern[i]);
  }
  // cout << "Simulating with pattern: ";
  // for (auto& g : _pilist) {
  //   cout << g->_simVal << ", valCh: " << g->_valCh << " ";
  // }
  // cout << endl;

  // for (auto& g : _dfslist) g->reset_sim();

  // for (auto& g : _dfslist) {
  //   if (g->getType() == PO_GATE) g->sim();
  // }
  for (auto& g: _dfslist) g->sim();
  // gen FEC list
  // cout << "Generating FEC List..." << endl;

}

void CirMgr::_genLog(vector<size_t>& pattern) {
  if (_simLog == 0) return;
  for (size_t i = 0;i < 64 && _logCnt < _cnt; ++i, ++_logCnt) {
    for (auto& pat : pattern) {
      (*_simLog) << ((pat >> i) & 1);
    }
    (*_simLog) << " ";
    for (auto& g : _polist) {
      (*_simLog) << ((g->getSimVal() >> i) & 1);
    }
    (*_simLog) << endl;
  }
}

void CirMgr::_initfecGrp() {
  assert(_initfec == 0);
  _initfec = 1;
  FecGrp* newFecGrp = new FecGrp(Const0);
  // FecGrp* InewFecGrp = new FecGrp(Const0, 1);
  _fecGrps.push_back(newFecGrp);
  // _IfecGrps.push_back(InewFecGrp);
  for (auto& g: _dfslist) {
    if (g->getType() == AIG_GATE) {
      _fecGrps[0]->add(g);
      // _IfecGrps[0]->add(g, 1);
    }
  }
  // cout << "Initial Fec Group: ";
  // for (auto& fecgrp : _fecGrps) {
  //   for (auto it = fecgrp->_child.begin(); it != fecgrp->_child.end(); ++it) {
  //     cout << it->second.gate()->getVar() << " ";
  //   }
  //   cout << endl;
  // }
}

void CirMgr::_genfecGrp() {
  size_t size = _fecGrps.size();
  for (size_t i = 0;i < size; ++i) {
    FecGrp* fecGrp = _fecGrps[i];

    map<size_t, FecGrp*> mp;  // simVal to fecGrp
    map<size_t, CirGateV>& ch = fecGrp->_child;

    assert(!ch.empty());
    size_t val = ch.begin()->second.gate()->getSimVal();
    if (ch.begin()->second.inv()) val = ~val; // IFec
    mp[val] = fecGrp;

    for (map<size_t, CirGateV>::iterator it = ++ch.begin(); it != ch.end();) {
      size_t chVal = it->second.gate()->getSimVal();
      if (it->second.inv()) chVal = ~chVal; // IFec
      if (chVal != val && ~chVal != val) {
        auto foundFe = mp.find(chVal);
        auto foundIFec = mp.find(~chVal);

        if (foundFe == mp.end() && foundIFec == mp.end()) {
          // cout << "Add new FecGrp for " << it->second.gate()->getVar() << endl;
          FecGrp* newFecGrp = new FecGrp(it->second.gate(), it->second.inv());
          _fecGrps.push_back(newFecGrp);
          mp[chVal] = _fecGrps.back();
        }

        else {
          auto found = (foundFe == mp.end()) ? foundIFec : foundFe;
          bool inv = (foundFe == mp.end());
          // cout << "Add " << it->second.gate()->getVar() << ", to old FecGrp" << endl;
          found->second->add(it->second.gate(), inv);
        }
        it = ch.erase(it);
      }
      else {
        if (~chVal == val) {
          // cout << "IFEC: " << it->second.gate()->getVar() << endl;
          it->second._inv = 1;
        }
        ++it;
      }
    }
  }
}
