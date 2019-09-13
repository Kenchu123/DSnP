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
   if (file.is_open()) while (getline(file, line)) lines.push_back(line);
   else return false; // check the file exist
   for (size_t j = 1; j < lines.size() - 1; j++) {
      line = lines[j];
      size_t kl, kr, vl, vr;
      kl = kr = vl = vr = 0;
      if (line[0] == '{' || line[0] == '}') continue;
      for (size_t i = 0;i < line.size(); i++) {
         if (line[i] == '"') { kl = i; break; }
      }
      for (size_t i = kl + 1; i < line.size(); i++) {
         if (line[i] == '"') { kr = i; break; }
      }
      for (size_t i = kr + 1; i < line.size(); i++) {
         if (line[i] == '-' || (line[i] <= '9' && line[i] >= '0')) { vl = i; break; }
      }
      for (size_t i = vl + 1; i < line.size(); i++) {
         if (line[i] > '9' || line[i] < '0') { vr = i - 1; break; }
      }
      if (kr == 0) continue;
      if (vr == 0) vr = line.size() - 1;
      string key = line.substr(kl + 1, kr - kl - 1);
      int val = stoi(line.substr(vl, vr - vl + 1), nullptr);
      _obj.emplace_back(key, val);
      _sum += val;
   }
   return true; // TODO
}

bool Json::checkArgs(const string& ar, const size_t& num) const {
   bool flag = 1;
   stringstream ss(ar);
   string arg = "";
   vector<string> args;
   while (ss >> arg) args.push_back(arg);
   if (args.size() > num) { 
      size_t ind = ar.find(args[num]);
      cerr << "Error: Extra argument \"" << ar.substr(ind) << "\"!!" << endl; flag = 0;
   }
   else if (args.size() < num) {
      if (args.empty()) { cerr << "Missing argument!!" << endl; flag = 0; }
      else { cerr << "Missing argument after \"" << args.back() << "\"!!" << endl; flag = 0; }
   }
   return flag;
}

void Json::printAll() const {
   cout << "{\n";
   for (size_t i = 0;i < _obj.size(); i++) {
      cout << "  " << _obj[i] << (i == _obj.size() - 1 ? "\n" : ",\n");
   }
   cout << "}" << endl;
}

void Json::printMin() const {
   JsonElem mi = _obj[0];
   for (auto ele : _obj) if (mi._value > ele._value) mi = ele;
   cout << "{ " << mi << " }.\n";
}

void Json::printMax() const {
   JsonElem ma = _obj[0];
   for (auto ele : _obj) if (ma._value < ele._value) ma = ele;
   cout << "{ " << ma << " }.\n";
}

void Json::add(const string& args) {
   string key, valStr;
   int val;
   stringstream ss(args);
   ss >> key >> valStr;
   for (auto ch : key) {
      if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'z') && (ch < 'A' || ch > 'Z') && ch != '_') {
         cerr << "Illegal argument \"" << key << "\"!!" << endl;
         return;
      }
   }
   try {
      val = stoi(valStr);
      _obj.emplace_back(key, val);
      _sum += val;
   }
   catch (const invalid_argument& a) {
      cerr << "Illegal argument \"" << valStr << "\"!!" << endl;
   }
}

ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}


