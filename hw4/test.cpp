#include <iostream>
using namespace std;

class A {
    public:
    A(): a(1), b(2), c(0) {}
    int a, b, c, d;
    ~A(){}
};

int main() {
    A* a = new A[100];
    // for (int i = 0;i < 10; ++i) a[i] = i + 1;
    // void* hel = (void*) a;
    // size_t* hell = (size_t*) hel;
    size_t* hel = (size_t*)a - 1;
    cout << a << endl << hel << endl;
    cout << (*hel) << " " << sizeof(size_t) << " " << sizeof(A) << endl;
    cout << (double)((*hel - sizeof(size_t))) / sizeof(size_t) << endl;
}