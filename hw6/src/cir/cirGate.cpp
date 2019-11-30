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
#include <map>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
   cout << "==================================================" << endl;
   string s = "= " + getTypeStr() + "(" + to_string(_var) + "), line " + _symbo + to_string(_lineNo);
   cout << s << setw(50 - s.size()) << "=" << endl;
   cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
}

 void 
 CirGate::connect(map<unsigned, CirGate*>& gatelist) {
    for (size_t i = 0;i < _fanin.size(); ++i) {
      size_t inVar = *(size_t*)_fanin[i];
      //  delete in;
      cout << _var << " is Connecting to " << inVar << endl;
       if (gatelist.find(inVar) == gatelist.end()) {
          // floating
          cout << "floating " << inVar << endl;
          CirGate* floatGate = new CirGate(inVar, 0, UNDEF_GATE);
          gatelist[inVar] = floatGate;
       }
      // set _fanin and _fanout
      _fanin[i] = gatelist[inVar];
      _fanin[i]->_fanout.push_back(this);
    }
 }
