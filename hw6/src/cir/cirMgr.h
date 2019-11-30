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
  ~CirMgr() {}

  // Access functions
  // return '0' if "gid" corresponds to an undefined gate.
  CirGate* getGate(unsigned gid) const { return 0; }

  // Member functions about circuit construction
  bool readCircuit(const string&);


  // Member functions about circuit reporting
  void printSummary() const;
  void printNetlist() const;
  void printPIs() const;
  void printPOs() const;
  void printFloatGates() const;
  void writeAag(ostream&) const;

private:
  bool readPI(int);
  bool readPO(int, int);
  bool readAIG(int, int, int);
  bool readSymbI(int , const string&);
  bool readSymbO(int, const string&);

  vector<CirPiGate*> _pilist;
  vector<CirPoGate*> _polist;
  vector<CirAigGate*> _aiglist;
  map<unsigned, CirGate*> _gatelist;
};

#endif // CIR_MGR_H
