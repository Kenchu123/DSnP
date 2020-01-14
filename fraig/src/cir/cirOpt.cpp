/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
  for (map<unsigned, CirGate*>::iterator it = _gatelist.begin(); it != _gatelist.end();) {
    // cout << it->second->getVar() << " " << it->second->_inDFSlist << " " << it->second->getTypeStr() << endl;
    if (!it->second->_inDFSlist && (it->second->getType() == AIG_GATE || it->second->getType() == UNDEF_GATE)) {
      cout << "Sweeping: " << it->second->getTypeStr() << "(" << it->second->getVar() << ") removed..." << endl;
      _removeGate(it->second->getVar(), &it);
    }
    else ++it;
  }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
  for (auto& g : _dfslist) {
    if (g->getType() != AIG_GATE) continue;
    assert(g->_fanin.size() == 2);
    CirGateV& A = g->_fanin[0];
    CirGateV& B = g->_fanin[1];
    if(A.gate()->getType() == CONST_GATE) {
      if (A.inv())  // CONST 1
        _replaceGateTo(g, B);
      else          // CONST 0
        _replaceGateTo(g, A);
    }
    else if (B.gate()->getType() == CONST_GATE) {
      if (B.inv())  // CONST 1
        _replaceGateTo(g, A);
      else          // CONST 0
        _replaceGateTo(g, B);
    }
    else if (A.gate() == B.gate()) {
      if (A.inv() == B.inv()) // two same fanin
        _replaceGateTo(g, A);
      else                    // two inv fanin
        _replaceGateTo(g, CirGateV(Const0, 0));
    }
  }
  // _dfslist.clear();
  genDFSList();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
void CirMgr::_removeGate(unsigned var, map<unsigned, CirGate*>::iterator* mapIt) {
  CirGate* g = _gatelist[var];
  assert(g->getType() == AIG_GATE || g->getType() == UNDEF_GATE);
  // remove g from it's fanin's out
  for (auto in : g->_fanin) {
    CirGate* inGate = in.gate();
    for (vector<CirGateV>::iterator it = inGate->_fanout.begin(); it != inGate->_fanout.end();) {
      if ((*it).gate() == g) {
        inGate->_fanout.erase(it);
        // clear undefined gate if it's fanout is empty
        if (inGate->_fanout.empty() && inGate->getType() == UNDEF_GATE) {
          _removeGate(inGate->getVar());
        }
      }
      else ++it;
    }
  }
  // remove g from aiglist
  if (g->getType() == AIG_GATE) {
    for (vector<CirAigGate*>::iterator it = _aiglist.begin(); it != _aiglist.end();) {
      if (*it == g) {
        _aiglist.erase(it);
      }
      else ++it;
    }
  }
  // remove g in _gatelist
  if (mapIt != NULL) *mapIt = _gatelist.erase(*mapIt);
  else _gatelist.erase(var); 
  // cout << "Removing: " << g->getTypeStr() << " " << var << endl;
  delete g;
}

// replace Gate A to it's fanin Gate B
void CirMgr::_replaceGateTo(CirGate* A, CirGateV B) {
  // change A's fanout's fanin
  cout << "Simplifying: " << B.gate()->getVar() << " merging " << (B.inv() ? "!" : "") << A->getVar() << "..." << endl;
  for (auto& Aout : A->_fanout) {
    bool phase = B.inv();
    for (auto& Aoutin : Aout.gate()->_fanin) {
      if (Aoutin.gate() == A) {
        Aoutin._gate = B._gate;
        Aoutin._inv = B._inv ^ Aoutin._inv;
        phase = Aoutin._inv;
        break;
      }
    }
    // Add A to B's fanout
    B.gate()->_fanout.emplace_back(Aout.gate(), phase);
  }
  _removeGate(A->getVar());
}