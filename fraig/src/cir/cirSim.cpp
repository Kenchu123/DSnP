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
}

void
CirMgr::fileSim(ifstream& patternFile)
{
  string pattern = "";
  _cnt = 0, _logCnt = 0;
  vector<size_t> vec; vec.resize(_I);
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
      _genLog(vec);
      vec.clear(); vec.resize(_I);
    }
  }
  if (_cnt % 64) _simPattern(vec);
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