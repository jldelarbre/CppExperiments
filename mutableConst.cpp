#include <iostream>
#include <cstring>
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
// Move semantics with "this" pointer / rvalue reference for *this

class string {
    char *data;

public:

    string(const char *p) {
        size_t size = std::strlen(p) + 1;
        data = new char[size];
        std::memcpy(data, p, size);
    }

    ~string() {
        delete[] data;
    }

    string(const string &that) {
        cout << "constructor copy" << endl;
        size_t size = std::strlen(that.data) + 1;
        data = new char[size];
        std::memcpy(data, that.data, size);
    }

    string(string &&that) {
        cout << "constructor move" << endl;
        data = that.data;
        that.data = nullptr;
    }

    // Delete operator= for immutablility: "a = b;" forbidden
    string& operator=(const string& that) = delete;

    // This operator lets the object immutable but it is weird and should not be defined:
    // a = b;
    // let 'a' untouched but the expression return a copy of 'b'
//    string operator=(const string& that) const {
//        cout << "= copy" << endl;
//        return string(that);
//    }

    // string& operator=(string&& that) = delete;
    //
    // The object is no longer immutable (keep for the demo), operator= shall be deleted as above
    //
    string& operator=(string&& that) {
        cout << "= move" << endl;
        std::swap(data, that.data);
        return *this;
    }

    // Could NOT be implemented alone, shall come with: string append(const char *p) [const] &;
	// Incompatible with: string append(const char *p);
    string& append(const char *p) && {
        cout << "append move" << endl;
//        size_t size = std::strlen(data) + std::strlen(p) + 1;
//        char *tmpData = new char[size];
//        std::strcpy(tmpData, data);
//        std::strcat(tmpData, p);
//
//        delete[] data;
//
//        data = tmpData;

        string tmp(data, p);
        std::swap(data, tmp.data);

        return *this;
    }

    // Could be implemented alone
    // Incompatible with: string append(const char *p);
    string append(const char *p) const & {
        cout << "append copy" << endl;
        return string(data, p); // RVO
    }

private:

    string(const char *start, const char *end) {
        size_t size = std::strlen(start) + std::strlen(end) + 1;
        data = new char[size];
        std::strcpy(data, start);
        std::strcat(data, end);
    }
};

string f() {
    return string("s").append("1...").append("2...");
}

void moveSemanticsThis() {
    cout << "Move semantics with \"this\" pointer / rvalue reference for *this" << endl;

    string a("a");
    cout << "b:" << endl;
    string b = a.append("...");
    cout << "c:" << endl;
    string c = a.append("1...").append("2...");
    cout << "d:" << endl;
    string d = string("s").append("1...").append("2...");
    cout << "e:" << endl;
    string e("e");
    e = string("s").append("1...").append("2..."); // impossible if operator=(string&&) deleted
    cout << "f:" << endl;
    string ff = f();

    // a = b; // impossible if operator=(const string&) deleted

    // string g = string("g"); // do not use operator=

    cout << endl;
}

// output:
// b:
// append copy
// c:
// append copy
// append move
// constructor copy
// d:
// append move
// append copy
// e:
// append move
// append copy
// = move
// f:
// append move
// append copy

////////////////////////////////////////////////////////////////////////////////////////////////////

void mutableConst()
{
    mutableLambda();
    moveSemanticsThis();
}
