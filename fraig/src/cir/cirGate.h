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
#include <map>
#include "cirDef.h"
#include "sat.h"

using namespace std;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate;
class CirPiGate;
class CirPoGate;
class CirAigGate;

class CirGateV {
  public:
  friend class CirMgr;
  friend class FecGrp;
  friend class CirGate;
  friend class CirPiGate;
  friend class CirPoGate;
  friend class CirAigGate;

  CirGateV() {}
  CirGateV(CirGate* g, bool inv): _gate(g), _inv(inv) {}
  CirGate* gate() const { return _gate; }
  bool inv() const { return _inv; }

  protected:
  CirGate* _gate;
  bool _inv;
};

class CirGate
{
public:
  CirGate() {}
  CirGate(int var, int lineNo, GateType gateType) {
    _var = var; _lineNo = lineNo; _gateType = gateType;
    _gateInit();
  }
  

  virtual ~CirGate() { reset(); }

  friend class CirMgr;

  // Basic access methods
  string getTypeStr() const {
    switch(_gateType) {
      case UNDEF_GATE: return "UNDEF";
      case PI_GATE: return "PI";
      case PO_GATE: return "PO";
      case AIG_GATE: return "AIG";
      case CONST_GATE: return "CONST";
      case TOT_GATE: return "TOT_GATE";
      default: return "";
    }
  }
  unsigned getLineNo() const { return _lineNo; }
  virtual bool isAig() const { return false; }

  // Printing functions
  // virtual void printGate() const = 0;
  void printGate() {} // this is I add
  void reportGate() const;
  void reportFanin(int level);
  void reportFanout(int level);

  unsigned getVar() { return _var; }
  GateType getType() { return _gateType; }

  void connect(map<unsigned, CirGate*>&);

  void addSymbol(const string& symb) {
    _symbo = symb;
  }

  bool isGlobalRef() { return _ref == _globalRef; }
  void setToGlobalRef() { _ref = _globalRef; }
  static void setGlobalRef() { ++_globalRef; }

  void reset();

  void setSimVal(size_t& val) {
    _valCh = (val == _simVal ? 0 : 1);
    _simVal = val;
  }
  size_t getSimVal() { return _simVal; }
  bool doSim () { return _doSim; }
  bool valCh () { return _valCh; }
  void reset_sim() { _doSim = 0; }
  bool sim();
  void printSim() const;
  void printFECs() const;
  void setFecGrp(FecGrp* fec) { _fecGrp = fec; }
  FecGrp* getFecGrp() { return _fecGrp; }

  void _gateInit() {
    _inDFSlist = false;
    _fecGrp = 0;
    _simVal = 0;
    _valCh = 0;
    _doSim = 0;
  }

  Var getSatVar() const { return _satVar; }
  void setVar(const Var& v) { _satVar = v; }

private:
  static unsigned _globalRef;
  unsigned _ref;

  void _dfsFanin(CirGate*, unsigned, bool, int);
  void _dfsFanout(CirGate*, unsigned, bool, int);

protected:
  GateType _gateType;
  unsigned _var;
  unsigned _lineNo;
  vector<CirGateV> _fanin;
  vector<CirGateV> _fanout;
  string _symbo;

  size_t _simVal;
  bool _valCh;
  bool _doSim;
  FecGrp* _fecGrp;

  bool _inDFSlist;

  Var _satVar;
};

class CirPiGate : public CirGate
{
public:
  CirPiGate() {}
  CirPiGate(int lit, unsigned lineNo) {
    _gateType = PI_GATE;
    _lineNo = lineNo;
    _var = lit / 2;
    _symbo = "";
    _gateInit();
  }
  ~CirPiGate() { reset(); }
};

class CirPoGate : public CirGate
{
public:
  CirPoGate() {}
  CirPoGate(int srclit, int var, unsigned lineNo) {
    _gateType = PO_GATE;
    _lineNo = lineNo;
    _var = var;

    size_t srcVar = (size_t)(srclit / 2);
    _fanin.emplace_back((CirGate*)srcVar, srclit % 2 == 1 ? 1 : 0);
    _symbo = "";
    _gateInit();
  }
  ~CirPoGate() { reset(); }
};

class CirAigGate : public CirGate
{
public:
  CirAigGate() {}
  CirAigGate(int lit, int src1, int src2, unsigned lineNo) {
    _gateType = AIG_GATE;
    _lineNo = lineNo;
    _var = lit / 2;
     _gateInit();

    size_t var1 = (size_t)(src1 / 2);
    size_t var2 = (size_t)(src2 / 2);
    _fanin.emplace_back((CirGate*)var1, src1 % 2 == 1 ? 1 : 0);
    _fanin.emplace_back((CirGate*)var2, src2 % 2 == 1 ? 1 : 0);
    _symbo = "";
  }
  ~CirAigGate() { reset(); }
};


#endif // CIR_GATE_H
