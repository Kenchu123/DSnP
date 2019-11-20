#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <algorithm>
using namespace std;

int main() {
    vector<string> vec;
    srand(7122);
    int num = 1000;
    for (int j = 0;j < num; ++j) {
        string s = "";
        for (int i = 0;i < 5; ++i) {
            int r = rand() % 26;
            s += ('a' + r);
        }
        vec.push_back(s);
    }
    for (auto i : vec) cout << "adta -s " << i << endl;
    cout << "usage" << endl;
    random_shuffle(vec.begin(), vec.end());
    for (int i = 0; i < vec.size() && i < 10000; ++i) cout << "adtq " << vec[i] << endl;
    cout << "usage" << endl;
    for (int i = 0; i < vec.size() && i < 10000; ++i) cout << "adtd -s " << vec[i] << endl;
    cout << "usage" << endl;
    cout << "q -f" << endl;
}