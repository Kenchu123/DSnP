/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   j.reset();
   assert(j._obj.empty());
   vector<string> lines;
   string line;
   while (getline(is,line)) lines.push_back(line);
   for (size_t i = 0; i < lines.size(); ++i) {
      if (lines[i].find('{') != string::npos || lines[i].find('}') != string::npos) continue;
      size_t l1 = lines[i].find('"');
      if (l1 == string::npos) continue;
      size_t l2 = lines[i].find('"', l1 + 1);
      string key = lines[i].substr(l1 + 1, l2 - l1 - 1);

      int val;
      size_t v1, v2;
      for (size_t j = l2 + 1; j < lines[i].size(); ++j) {
         if (lines[i][j] == '-' || (lines[i][j] <= '9' && lines[i][j] >= '0')) { v1 = j; break; }
      }
      for (size_t j = v1 + 1; j < lines[i].size(); ++j) {
         v2 = j;
         if (lines[i][j] > '9' || lines[i][j] < '0') { v2--; break; }
      }
      myStr2Int(lines[i].substr(v1, v2 - v1 + 1), val);
      j._obj.emplace_back(key, val);
   }
   j._isRead = true;
   lines.clear();
   is.clear();
   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   os << "{\n";
   for (size_t i = 0;i < j.size(); ++i) {
      os << "  " << j[i] << (i == j.size() - 1 ? "\n" : ",\n");
   }
   os << "}\nTotal JSON elements: " << j.size() << endl;
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
   _obj.clear();
   _isRead = false;
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   for (auto obj : _obj) { // check if key is repeated
      if (obj.key() == elm.key())
      return false;
   }
   _obj.push_back(elm); 
   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   if (_obj.empty()) return NAN; 
   return (float)this->sum() / _obj.size();
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   if (_obj.empty()) { idx = _obj.size(); return maxN; }
   for (size_t i = 0;i < _obj.size(); ++i) {
      if (_obj[i].value() > maxN) {
         maxN = _obj[i].value();
         idx = i;
      } 
   }
   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MAX
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   if (_obj.empty()) { idx = _obj.size(); return minN; }
   for (size_t i = 0;i < _obj.size(); ++i) {
      if (_obj[i].value() < minN) {
         minN = _obj[i].value();
         idx = i;
      } 
   }

   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   int s = 0;
   for (auto ele : _obj) {
      s += ele.value();
   }
   return s;
}
