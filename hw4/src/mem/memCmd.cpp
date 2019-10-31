/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   // Use try-catch to catch the bad_alloc exception
   //    cerr << "Requested memory (" << t << ") is greater than block size"
   //         << "(" << _blockSize << "). " << "Exception raised...\n";
   
   vector<string> options;
   if (!CmdExec::lexOptions(option, options)) return CMD_EXEC_ERROR;
   if (options.empty()) return CmdExec::errorOption(CMD_OPT_MISSING, "");

   string numObjectsStr = "", arraySizeStr = "";
   int numObjects, arraySize;
   bool doArray = 0, hasNum = 0;
   // check cmd
   for (size_t i = 0;i < options.size(); ++i) {
      if (myStrNCmp("-Array", options[i], 2) == 0) {
         if (doArray) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
         doArray = 1;
         if (i + 1 < options.size()) {
            arraySizeStr = options[i + 1];
            if (!myStr2Int(arraySizeStr, arraySize)) return CmdExec::errorOption(CMD_OPT_ILLEGAL, arraySizeStr);
            if (arraySize <= 0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, arraySizeStr);
            ++i;
         }
         else return CmdExec::errorOption(CMD_OPT_MISSING, "");
      }
      else { // numObjects
         if (hasNum) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
         numObjectsStr = options[i];
         if (!myStr2Int(numObjectsStr, numObjects)) return CmdExec::errorOption(CMD_OPT_ILLEGAL, numObjectsStr);
         if (numObjects <= 0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, numObjectsStr);
         hasNum = 1;
      }
   }
   // if no tell numObjects
   if (numObjectsStr == "") return CmdExec::errorOption(CMD_OPT_MISSING, "");

   try {
      if (doArray) mtest.newArrs(numObjects, arraySize);
      else mtest.newObjs(numObjects);
   }
   catch (bad_alloc e) {
      return CMD_EXEC_ERROR;
   }

   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   if (!CmdExec::lexOptions(option, options)) return CMD_EXEC_ERROR;
   if (options.empty()) return CmdExec::errorOption(CMD_OPT_MISSING, "");

   bool doArray = false, doIndex = false, doRandom = false;
   string numStr;
   int num;
   // check cmd illegal
   for (size_t i = 0;i < options.size(); ++i) {
      if (myStrNCmp("-Index", options[i], 2) == 0) {
         if (doIndex || doRandom) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
         doIndex = 1;
         if (i + 1 < options.size()) {
            numStr = options[i + 1];
            if (!myStr2Int(numStr, num)) return CmdExec::errorOption(CMD_OPT_ILLEGAL, numStr);
            ++i;
         }
         else return CmdExec::errorOption(CMD_OPT_MISSING, "");
      }
      else if (myStrNCmp("-Random", options[i], 2) == 0) {
         if (doIndex || doRandom) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
         doRandom = 1;
         if (i + 1 < options.size()) {
            numStr = options[i + 1];
            if (!myStr2Int(numStr, num)) return CmdExec::errorOption(CMD_OPT_ILLEGAL, numStr);
            ++i;
         }
         else return CmdExec::errorOption(CMD_OPT_MISSING, "");
      }
      else if (myStrNCmp("-Array", options[i], 2) == 0) {
         if (doArray) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
         doArray = 1;
      }
      else {
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      }
   }

   if (!doIndex && !doRandom) return CmdExec::errorOption(CMD_OPT_MISSING, ""); 
   
   
   // check num illegal
   if (doIndex) {
      if (num < 0) {
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, numStr);
      }
      if (doArray && num >= mtest.getArrListSize()) {
         cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << num << "!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, numStr);
      }
      else if (!doArray && num >= mtest.getObjListSize()) {
         cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << num << "!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, numStr);
      }
   }

   if (doRandom) {
      if (num < 0) {
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, numStr);
      }
      if (doArray && mtest.getArrListSize() == 0) {
         cerr << "Size of array list is 0!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, "-r");
      }
      if (!doArray && mtest.getObjListSize() == 0) {
         cerr << "Size of object list is 0!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, "-r");
      }
   }

   // generate random
   // vector<int> nums;
   // if (doRandom) for (size_t i = 0; i < num; ++i) {
   //    nums.push_back(rnGen(num));
   // }
   size_t arrSize = mtest.getArrListSize();
   size_t objSize = mtest.getObjListSize();

   // call function
   if (doRandom) {
      for (size_t i = 0;i < num; ++i) {
         if (doArray) mtest.deleteArr(rnGen(arrSize));
         else mtest.deleteObj(rnGen(objSize));
      }
   }
   else {
      if (doArray) mtest.deleteArr(num);
      else mtest.deleteObj(num);
   }

   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


