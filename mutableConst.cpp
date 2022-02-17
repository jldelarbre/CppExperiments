#include <iostream>
#include <mutex>

using std::cout, std::endl, std::mutex, std::lock_guard;

////////////////////////////////////////////////////////////////////////////////////////////////////
// const this & mutable

class A
{
public:
    mutable int a;
    mutable mutex m;

    void f() const {
        lock_guard<mutex> lk(m);
        a = 2;
    }

    void g() const {
        // A* aa = this; // error 'this' is const
        const A* aa = this;
    }

    void h() {
        A* aa = this;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// mutable lambda

void mutableLambda() {
    int n = 1;
    cout << "n = " << n << endl;
    [&](){n = 10;}();
    cout << "n = " << n << endl;
    auto myLambda = [=]() mutable {
        n += 20;
        cout << "[in lambda] n = " << n << endl;
    };
    myLambda();
    myLambda();
    cout << "n = " << n << endl;
    // [=](){n = 10;}(); // Error: a by-value capture cannot be modified in a non-mutable lambda
	cout << endl;
}

// output:
// n = 1
// n = 10
// [in lambda] n = 30
// [in lambda] n = 50
// n = 10

////////////////////////////////////////////////////////////////////////////////////////////////////

void mutableConst()
{
    mutableLambda();
}
