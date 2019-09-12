/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "p2Json.h"

using namespace std;

// Implement member functions of class Row and Table here
bool
Json::read(const string& jsonFile)
{
   fstream file(jsonFile, fstream::in);
   vector<string> lines;
   string line;
   if (file.is_open()) {
      while (getline(file, line)) lines.push_back(line);
   }
   for (int j = 1; j < lines.size() - 1; j++) {
      line = lines[j];
      int kl, kr, vl, vr;
      kl = kr = vl = vr = 0;
      if (line[0] == '{' || line[0] == '}') continue;
      for (int i = 0;i < line.size(); i++) {
         if (line[i] == '"') { kl = i; break; }
      }
      for (int i = kl + 1; i < line.size(); i++) {
         if (line[i] == '"') { kr = i; break; }
      }
      for (int i = kr + 1; i < line.size(); i++) {
         if (line[i] == '-' || (line[i] <= '9' && line[i] >= '0')) { vl = i; break; }
      }
      for (int i = vl + 1; i < line.size(); i++) {
         if (line[i] > '9' || line[i] < '0') { vr = i - 1; break; }
      }
      if (kr == 0) continue;
      if (vr == 0) vr = line.size() - 1;
      string key = line.substr(kl + 1, kr - kl - 1);
      int val = stoi(line.substr(vl, vr - vl + 1), nullptr);
      JsonElem ele(key, val);
      _sum += val;
      _obj.push_back(ele);
   }
   return true; // TODO
}

void Json::printAll() {
   cout << "{\n";
   for (int i = 0;i < _obj.size(); i++) {
      cout << "  " << _obj[i] << (i == _obj.size() - 1 ? "\n" : ",\n");
   }
   cout << "}" << endl;
}

void Json::printMin() {
   JsonElem mi = _obj[0];
   for (auto ele : _obj) if (mi._value > ele._value) mi = ele;
   cout << "{ " << mi << " }.\n";
}

void Json::printMax() {
   JsonElem ma = _obj[0];
   for (auto ele : _obj) if (ma._value < ele._value) ma = ele;
   cout << "{ " << ma << " }.\n";
}

void Json::add() {
   string content = "";
   string key, val;
   getline(cin, content);
   stringstream ss(content);
   ss >> key >> val;
   cout << key << " " << val;
}

ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}


