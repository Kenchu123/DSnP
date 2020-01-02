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
        // cout << inGate->getVar() << " Found: " << g->getVar() << endl;
        inGate->_fanout.erase(it);
      }
      else ++it;
    }
  }
  // remove g from aiglist
  if (g->getType() == AIG_GATE) {
    for (vector<CirAigGate*>::iterator it = _aiglist.begin(); it != _aiglist.end();) {
      if (*it == g) {
        // cout << "Remove " << g->getVar() << " from aiglist" << endl;
        _aiglist.erase(it);
      }
      else ++it;
    }
  }
  // remove g in _gatelist
  if (mapIt != NULL) *mapIt = _gatelist.erase(*mapIt);
  else _gatelist.erase(var); 
  // // remove g from dfslist
  // if (g->_inDFSlist) {
  //   for (vector<CirGate*>::iterator it = _dfslist.begin(); it != _aiglist.end();) {
  //     if (*it == g) {
  //       _dfslist.erase(it);
  //     }
  //     else ++it;
  //   }
  // }
  cout << "Sweeping: " << g->getTypeStr() << "(" << var << ") removed..." << endl;
  delete g;
}