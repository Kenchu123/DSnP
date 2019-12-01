/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

#include "cirGate.h"
#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
  CirMgr(){}
  ~CirMgr() { reset(); }

  // Access functions
  // return '0' if "gid" corresponds to an undefined gate.
  CirGate* getGate(unsigned gid) const {
    if (_gatelist.find(gid) == _gatelist.end()) return 0;
    return _gatelist.find(gid)->second;
  }

  // Member functions about circuit construction
  bool readCircuit(const string&);

  // Member functions about circuit reporting
  void printSummary() const;
  void printNetlist() const;
  void printPIs() const;
  void printPOs() const;
  void printFloatGates() const;
  void writeAag(ostream&) const;

  // Member functions about circuit DFS
  void genDFSList();

  // CONST 0 gate
  static CirGate* Const0;

  // reseting
  void reset();

private:
  bool _readPI(int);
  bool _readPO(int, int);
  bool _readAIG(int, int, int);
  bool _readSymbI(int , const string&);
  bool _readSymbO(int, const string&);
  void _buildConnect();
  void _dfs(CirGate*);

  vector<CirPiGate*> _pilist;
  vector<CirPoGate*> _polist;
  vector<CirAigGate*> _aiglist;
  vector<CirGate*> _dfslist;
  map<unsigned, CirGate*> _gatelist;
};

#endif // CIR_MGR_H
