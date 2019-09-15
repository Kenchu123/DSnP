/****************************************************************************
  FileName     [ p2Json.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Json JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_JSON_H
#define P2_JSON_H

#include <vector>
#include <string>
#include <unordered_set>
#include <iomanip>
#include <limits>

using namespace std;

class JsonElem
{
public:
   // TODO: define constructor & member functions on your own
   JsonElem() {}
   JsonElem(const string& k, int v): _key(k), _value(v) {}

   friend ostream& operator << (ostream&, const JsonElem&);
   friend class Json;
private:
   string  _key;   // DO NOT change this definition. Use it to store key.
   int     _value; // DO NOT change this definition. Use it to store value.
};

class Json
{
public:
   // TODO: define constructor & member functions on your own
   Json(): _sum(0) {};
   bool read(const string&);
   bool checkArgs(const string&, const size_t&) const;
   void printAll() const;
   void printMin() const;
   void printMax() const;
   void printSum() const { cout << "The summation of the values is: " << _sum << ".\n"; }
   void printAvg() const { cout << "The average of the values is: " << fixed << setprecision(1) << (double)_sum / _obj.size() << ".\n"; }
   bool isEmpty() const { if(_obj.empty()) cerr << "Error: No element found!!\n"; return _obj.empty(); }
   void add(const string&);

private:
   vector<JsonElem>       _obj;  // DO NOT change this definition.
                                 // Use it to store JSON elements.
   int _sum;
};

#endif // P2_TABLE_H
