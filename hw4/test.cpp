#include <iostream>
using namespace std;

class A {
    int a, b[10];
    char c;
};

int main() {
    A* a1 = new A;
    A* a2 = new A;

    A* _first = a1;
    cout << _first << endl;

    *(A**)a2 = _first;
    _first = a2;

    cout << _first << " " << *(A**)_first << endl << a2 << endl;

    
}