/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;
class CirPiGate;
class CirPoGate;
class CirAigGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
  CirGate() {}
  virtual ~CirGate() {}

  // Basic access methods
  string getTypeStr() const {
    switch(_gateType) {
      case UNDEF_GATE: return "UNDEF_GATE";
      case PI_GATE: return "PI_GATE";
      case PO_GATE: return "PO_GATE";
      case AIG_GATE: return "AIG_GATE";
      case CONST_GATE: return "CONST_GATE";
      case TOT_GATE: return "TOT_GATE";
      default: return "";
    }
  }
  unsigned getLineNo() const { return _lineNo; }

  // Printing functions
  // virtual void printGate() const = 0;
  virtual void printGate() {} // this is I add
  void reportGate() const;
  void reportFanin(int level) const;
  void reportFanout(int level) const;

  unsigned getVar() { return _var; }

private:

protected:
  GateType _gateType;
  unsigned _var;
  unsigned _lineNo;
  vector<CirGate*> _fanin;
  vector<CirGate*> _fanout;
  vector<bool> _inv;

};

class CirPiGate : public CirGate
{
public:
  CirPiGate() {}
  CirPiGate(int lit, unsigned lineNo) {
    _gateType = PI_GATE;
    _lineNo = lineNo;
    _var = lit / 2;
  }
};

class CirPoGate : public CirGate
{
public:
  CirPoGate() {}
  CirPoGate(int srclit, int var, unsigned lineNo) {
    _gateType = PO_GATE;
    _lineNo = lineNo;
    _var = var;

    _inv.push_back(srclit % 2 == 1 ? 1 : 0);

    size_t* srcVar = new size_t(srclit / 2);
    _fanin.push_back((CirGate*)srcVar);
  }
};

class CirAigGate : public CirGate
{
public:
  CirAigGate() {}
  CirAigGate(int lit, int src1, int src2, unsigned lineNo) {
    _gateType = AIG_GATE;
    _lineNo = lineNo;
    _var = lit / 2;

    _inv.push_back(src1 % 2 == 1 ? 1 : 0);
    _inv.push_back(src2 % 2 == 1 ? 1 : 0);

    size_t* var1 = new size_t(src1 / 2);
    size_t* var2 = new size_t(src2 / 2);
    _fanin.push_back((CirGate*)var1);
    _fanin.push_back((CirGate*)var2);
  }
};

#endif // CIR_GATE_H
