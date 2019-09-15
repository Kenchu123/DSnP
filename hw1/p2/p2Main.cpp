/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"

using namespace std;

int main()
{
   Json json;
   // Read in the csv file. Do NOT change this part of code.
   string jsonFile;
   cout << "Please enter the file name: ";
   cin >> jsonFile;
   if (json.read(jsonFile))
      cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }

   while (true) {
      cout << "Enter command: ";
      string cmd = "", args = "";
      cin >> cmd;
      getline(cin, args);
      if (cmd == "PRINT") { if (json.checkArgs(args, 0)) json.printAll(); }
      else if (cmd == "AVE") { if (!json.isEmpty() && json.checkArgs(args, 0)) json.printAvg(); }
      else if (cmd == "SUM") { if (!json.isEmpty() && json.checkArgs(args, 0)) json.printSum(); }
      else if (cmd == "MAX") { if (!json.isEmpty() && json.checkArgs(args, 0)) json.printMax(); }
      else if (cmd == "MIN") { if (!json.isEmpty() && json.checkArgs(args, 0)) json.printMin(); }
      else if (cmd == "ADD") { if (json.checkArgs(args, 2)) json.add(args); }
      else if (cmd == "EXIT") exit(1);
      else {
         cerr << "Error: unknown command: \"" << cmd << "\"" << endl;
      }
   }
}
