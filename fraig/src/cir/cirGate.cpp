/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include <bitset>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
unsigned CirGate::_globalRef = 0;

void
CirGate::reportGate() const
{
   for (int i = 0;i < 80; ++i) cout << "=";
   cout << endl;
   string s = "= " + getTypeStr() + "(" + to_string(_var) + ")" \
      + (_symbo == "" ? "" : "\"" + _symbo + "\"") + ", line " + to_string(_lineNo);
   cout << s << endl;
   printFECs();
   printSim();
   for (int i = 0;i < 80; ++i) cout << "=";
   cout << endl;
}

void
CirGate::reportFanin(int level)
{
   assert (level >= 0);
   setGlobalRef();
   _dfsFanin(this, 0, 0, level);
}

void
CirGate::_dfsFanin(CirGate* g, unsigned spaces, bool inv, int level) {
   for (size_t i = 0; i < spaces; ++i) cout << " ";
   if (inv) cout << "!";
   cout << g->getTypeStr() << " " << g->_var;
   // check if need to add (*)
   if (g->isGlobalRef() && level > 0 && !g->_fanin.empty()) {
      cout << " (*)" << endl; return;
   }
   else cout << endl;
   // dfs next level
   if (level == 0) return;
   g->setToGlobalRef();
   for (size_t i = 0; i < g->_fanin.size(); ++i) {
      _dfsFanin(g->_fanin[i]._gate, spaces + 2, g->_fanin[i]._inv, level - 1);
   }
}

void
CirGate::reportFanout(int level)
{
   assert (level >= 0);
   setGlobalRef();
   _dfsFanout(this, 0, 0, level);
}

void
CirGate::_dfsFanout(CirGate* g, unsigned spaces, bool inv, int level) {
   for (size_t i = 0; i < spaces; ++i) cout << " ";
   if (inv) cout << "!";
   cout << g->getTypeStr() << " " << g->_var;
   // check if need to add (*)
   if (g->isGlobalRef() && level > 0 && !g->_fanout.empty()) {
      cout << " (*)" << endl; return;
   }
   else cout << endl;
   // dfs next level
   if (level == 0) return;
   g->setToGlobalRef();
   for (size_t i = 0; i < g->_fanout.size(); ++i) {
      _dfsFanout(g->_fanout[i].gate(), spaces + 2, g->_fanout[i].inv(), level - 1);
   }
}

 void 
 CirGate::connect(map<unsigned, CirGate*>& gatelist) {
    for (size_t i = 0;i < _fanin.size(); ++i) {
      size_t inVar = (size_t)(void*)_fanin[i]._gate;
       if (gatelist.find(inVar) == gatelist.end()) {
          CirGate* floatGate = new CirGate(inVar, 0, UNDEF_GATE);
          gatelist[inVar] = floatGate;
       }
      // set _fanin and _fanout
      _fanin[i]._gate = gatelist[inVar];
      _fanin[i]._gate->_fanout.emplace_back(this, _fanin[i]._inv);
    }
 }

void
CirGate::reset() {
   _fanin.clear();
   _fanout.clear();
   _fecGrp = 0;
   _simVal = 0;
   _valCh = 0;
   _doSim = 0;
   _inDFSlist = 0;
}

bool
CirGate::sim() {
   if (_gateType == PI_GATE) return _valCh;
   if (_gateType == UNDEF_GATE || _gateType == CONST_GATE) return false;

   size_t simVal = ~0;
   for (auto& g : _fanin) {
      // if (!g._gate->_doSim) g._gate->sim();
      simVal &= (g._inv ? ~g._gate->_simVal : g._gate->_simVal);
   }
   _doSim = true;
   setSimVal(simVal);
   // cout << "Simulating " << getVar() << ", Simulate result: " << simVal << endl;
   return _valCh;
}

void
CirGate::printSim() const {
   cout << "= Value: ";
   bitset<64> bit(_simVal);
   string bString = bit.to_string();
   for (size_t i = 0; i < 64; ++i) {
      cout << ((i % 8 == 0 && i != 0) ? "_" : "") << bString[i];
   }
   cout << endl;
}

void CirGate::printFECs() const {
   cout << "= FECs:";
   if (_fecGrp == 0) { cout << endl; return; }
   assert(_fecGrp != 0);
   bool MeInv = 0;
   if (_fecGrp->_child.find(_var) != _fecGrp->_child.end()) {
      MeInv = _fecGrp->_child[_var].inv();
   }
   for (auto it = _fecGrp->_child.begin(); it != _fecGrp->_child.end(); ++it) {
      if (it->second.gate() != this) {
         cout << " " << (MeInv ^it->second.inv() ? "!" : "") << it->second.gate()->getVar();
      }
   }
   cout << endl;
}