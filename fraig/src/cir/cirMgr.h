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

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirGate.h"
#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr(): _M(0), _I(0), _L(0), _O(0), _A(0), _doComment(0), _comment(""), _type("") {}
   ~CirMgr() {} 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
      if (_gatelist.find(gid) == _gatelist.end()) return 0;
      return _gatelist.find(gid)->second;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

   // Member functions about circuit DFS
   void genDFSList();

   // CONST 0 gate
   static CirGate* Const0;

   // reseting
   void reset();

private:
   ofstream           *_simLog;

   // for parsing
  bool _readInitial(fstream&);
  bool _readPI(fstream&);
  bool _readPO(fstream&);
  bool _readAIG(fstream&);
  bool _readSymb(fstream&);
  bool _notSpace(char);
  bool _beSpace(char);
  bool _readNum(string&, int&, string);
  bool _doComment;
  string _comment;
  string _type;
  int _M, _I, _L, _O, _A;

  // for circuit
  void _buildConnect();
  void _dfs(CirGate*);

  vector<CirPiGate*> _pilist;
  vector<CirPoGate*> _polist;
  vector<CirAigGate*> _aiglist;
  vector<CirGate*> _dfslist;
  map<unsigned, CirGate*> _gatelist;

  // for optimize
  void _removeGate(unsigned, map<unsigned, CirGate*>::iterator* = 0);

};

#endif // CIR_MGR_H
