/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;
CirGate* CirMgr::Const0 = new CirGate(0, 0, CONST_GATE);

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   fstream file(fileName.c_str());
   if (!file) { cerr << "Cannot open design \"" << fileName << "\"!!" << endl; return false; }
   string type;
   int M, I, L, O, A;
   lineNo += 1;
   file >> type >> M >> I >> L >> O >> A; // read first line
   for (int i = 0;i < I; ++i) {
      lineNo += 1;
      int l; file >> l;
      _readPI(l);
   }
   for (int i = 0;i < O; ++i) {
      lineNo += 1;
      int l; file >> l;
      _readPO(l, M + i + 1);
   }
   for (int i = 0;i < A; ++i) {
      lineNo += 1;
      int l, s1, s2;
      file >> l >> s1 >> s2;
      _readAIG(l, s1, s2);
   }
   // symbol
   string ilo;
   while (file >> ilo) {
      lineNo += 1;
      if (ilo == "c") break;
      string symb; file >> symb;
      int pos = stoi(ilo.substr(1));
      if (ilo[0] == 'i') _readSymbI(pos, symb);
      else if (ilo[0] == 'o') _readSymbO(pos, symb);
      else if (ilo[0] == 'l') {}
   }
   // comment
   string comment;
   // todo
   file.close();

   // build connect
   cirMgr->_buildConnect();
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI" << setw(12) << _pilist.size() << endl;
   cout << "  PO" << setw(12) << _polist.size() << endl;
   cout << "  AIG" << setw(11) << _aiglist.size() << endl;
   cout << "------------------" << endl;
   cout << "  Total" << setw(9) << _pilist.size() + _polist.size() + _aiglist.size() << endl;
}

void
CirMgr::printNetlist() const
{
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (size_t i = 0;i < _pilist.size(); ++i) 
      cout << _pilist[i]->getVar() << (i == _pilist.size() - 1 ? "" : " ");
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for (size_t i = 0;i < _polist.size(); ++i) 
      cout << _polist[i]->getVar() << (i == _polist.size() - 1 ? "" : " ");
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   vector<unsigned> fltFanins;
   vector<unsigned> notUsed;
   for (map<unsigned, CirGate*>::const_iterator it = _gatelist.begin(); it != _gatelist.end(); ++it) {
      CirGate* gate = it->second;
      if (gate->getType() == CONST_GATE || gate->getType() == UNDEF_GATE) continue;
      for (auto i : gate->_fanin) {
         if (i->getType() == UNDEF_GATE) {
            fltFanins.push_back(gate->getVar());
            break;
         }
      }

      if ((gate->_fanout).empty() && gate->getType() != PO_GATE) notUsed.push_back(gate->getVar());
   }

   if (!fltFanins.empty()) {
      cout << "Gates with floating fanin(s): ";
      for (size_t i = 0;i < fltFanins.size(); ++i)
         cout << fltFanins[i] << (i == fltFanins.size() - 1 ? "\n" : " ");
   }
   if (!notUsed.empty()) {
      cout << "Gates defined but not used: ";
      for (size_t i = 0;i < notUsed.size(); ++i)
         cout << notUsed[i] << (i == notUsed.size() - 1 ? "\n" : " ");
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
}

bool 
CirMgr::_readPI(int lit) {
   cout << "line: " << lineNo << ", Reading PI " << lit << endl;
   CirPiGate* newPi = new CirPiGate(lit, lineNo);
   _pilist.push_back(newPi);
   _gatelist[newPi->getVar()] = newPi;
}

bool 
CirMgr::_readPO(int lit, int var) {
   cout << "line: " << lineNo << ", Reading PO " << lit << " " << var << endl;
    CirPoGate* newPo = new CirPoGate(lit, var, lineNo);
    _polist.push_back(newPo);
    _gatelist[newPo->getVar()] = newPo;
}

bool 
CirMgr::_readAIG(int lit, int src1, int src2) {
   cout << "line: " << lineNo << ", Reading AIG " << lit << " " << src1 << " " << src2 << endl;
   CirAigGate* newAig = new CirAigGate(lit, src1, src2, lineNo);
   _aiglist.push_back(newAig);
   _gatelist[newAig->getVar()] = newAig;
}

bool 
CirMgr::_readSymbI(int pos , const string& symb) {
   cout << "line: " << lineNo << ", Reading Symbo pi " << pos << " " << symb << endl;

}

bool 
CirMgr::_readSymbO(int pos, const string& symb) {
   cout << "line: " << lineNo << ", Reading Symbo po " << pos << " " << symb << endl;

}

void
CirMgr::_buildConnect() {
   _gatelist[0] = Const0;
   for (auto gate : _polist) gate->connect(_gatelist);
   for (auto gate : _aiglist) gate->connect(_gatelist);
}

