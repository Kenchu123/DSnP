#include <iostream>
#include <vector>
#include <utility>
using namespace std;

int main() {
    vector< pair<string, int> > vec;
    vec.push_back({"Jizz", 7122});
    cout << vec[0].first << " " << vec[0].second << endl;
}