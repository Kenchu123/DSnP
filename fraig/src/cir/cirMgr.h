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
#include <set>
#include <unordered_set>
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirGate.h"
#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr(): _M(0), _I(0), _L(0), _O(0), _A(0), _doComment(0), _comment(""), _type("") {
      reset();
   }
   ~CirMgr() { reset(); } 

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
   void printFECPairs();
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*);

   // Member functions about circuit DFS
   void genDFSList();
   unordered_set<unsigned> _dfsSet;

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
  void _gateDfs(CirGate*);

  vector<CirPiGate*> _pilist;
  vector<CirPoGate*> _polist;
  vector<CirGate*> _dfslist;
  map<unsigned, CirGate*> _gatelist;
  vector<CirGate*> _gdfslist;

  // optimize
  void _removeGate(unsigned, map<unsigned, CirGate*>::iterator* = 0);
  void _replaceGateTo(CirGate*, CirGateV);

  // strash
  unsigned long long _hashFanin(CirGate*);
  void _mergeGate(CirGate*, CirGate*, bool inv = 0);

  // simulate
  size_t _cnt, _logCnt;
  void _simPattern(vector<size_t>&);
  void _genLog(vector<size_t>&);
  void _genfecGrp();
  void _initfecGrp();
  static bool _sortFecGrp(FecGrp*&, FecGrp*&);
  bool _initfec;
  vector<FecGrp*> _fecGrps;

  // fraig
  void _genProofModel(SatSolver&);
  bool _fraigFec(FecGrp*, SatSolver&);
  void _collectPattern(const SatSolver&);
  void _mergeFecGrp(FecGrp*);
  vector<size_t> _pat;           // pattern when doing fraig
  unordered_set<string> _duPat;   // check if pattern is duplicated
  size_t _satCnt;
  unordered_set<FecGrp*> _fecToMerge;
};


class FecGrp {
public:
   friend CirMgr;
   friend CirGate;
   FecGrp() {}
   FecGrp(CirGate* g, bool inv = 0) {
      _child[g->getVar()] = CirGateV(g, inv);
      g->setFecGrp(this);
   }
   void add(CirGate* g, bool inv = 0) {
      _child[g->getVar()] = CirGateV(g, inv);
      g->setFecGrp(this);
   }
   ~FecGrp() {
      _isFraig = 0;
      for (auto it = _child.begin(); it != _child.end(); ++it) 
         it->second.gate()->setFecGrp(0);
      _child.clear();
   }

private:
   bool _isFraig = 0;
   map<size_t, CirGateV> _child; // fec group child
};

#endif // CIR_MGR_H
