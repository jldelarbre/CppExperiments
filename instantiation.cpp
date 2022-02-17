#include <iostream>
#include <string>

#include <memory>

using std::cout, std::endl,
std::string,
std::move;

////////////////////////////////////////////////////////////////////////////////////////////////////
// unique_ptr

void uniquePtr() {
	std::unique_ptr<string> a = std::make_unique<string>("aa");
	cout << *a << " valid: " << (bool)a << endl;
	std::unique_ptr<string> b = move(a);
	cout << "valid: " << (bool)a << endl;
	// std::unique_ptr<string> c = b; // impossible thanks to move semantic

	cout << endl;
}

// output:
// aa valid: 1
// valid: 0

////////////////////////////////////////////////////////////////////////////////////////////////////
// A default constructor is a constructor which can be called with no arguments (either defined
// with an empty parameter list, or with default arguments provided for every parameter). A type
// with a public default constructor is DefaultConstructible.
// It calls default constructor of inherited classes and class members. It is also used for
// value-initialization

class D1 {
	// implicit public default constructor
};

struct D2 {
	D2() {} // Explicit default constructor (must be public to be accessible)
};

struct D3 {
	int a;
	D3(int a = 0): a(a) {} // Default constructor using default values
};

struct D4 {
	int b;
	D4(int b): b(b) {}
	// same as: D4() {} // explicitly defaulted
	// Must be declared since another constructor is defined to let the default constructor exists
	D4() = default;
};

struct D5 {
	int b;
	// Default constructor implicitly deleted, since another constructor exists: D5 d5; does not compile
	D5(int b): b(b) { cout << "D4 constructor" << endl; }
};

struct D6 {
	D6() = delete; // Default constructor explicitly deleted: D6 d6; does not compile
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Peer constructor delegation

struct P : D1
{
	int a;
	int b;

	P(int a, int b) : D1(),
		a(a),
		b(a)
	{}

	P(int a) : P(a, 5) {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// explicit constructor

struct A1
{
string a1;
string b1;

// Converting constructors (not explicit)
A1(string a)
	: a1(a), b1("defaultB") // <= mem-initializer-list
{
	cout << "A1 Constructor 1" << endl;
}

// Converting constructors (not explicit), allows copy-initialization
A1(string a, string b)
	: a1(a), b1(b)
{
	cout << "A1 Constructor 2" << endl;
}
};

void f1(A1 a)
{
	cout << a.a1 << " / " << a.b1 << endl;
}

struct A2
{
string a1;
string b1;

explicit A2(string a)
	: a1(a), b1("defaultB") // <= mem-initializer-list
{
	cout << "A2 Constructor 1" << endl;
}

explicit A2(string a, string b)
	: a1(a), b1(b)
{
	cout << "A2 Constructor 2" << endl;
}
};

void f2(A2 a)
{
	cout << a.a1 << " / " << a.b1 << endl;
}

void implicitInstantiation()
{
	f1(string("valA1_a_1")); // Implicit conversion of string to A1 (could be avoid by 'explicit' keyword)
	f1(A1("valA1_a_2", "valB1_b_2")); // temp unamed variable, classic constructor initialization
	f1(A1{"valA1_a_3", "valB1_b_3"}); // temp unamed variable, direct-list-initialization
	A1 a1 = {string("valA1_a_4"), string("valB1_b_4")}; // copy-initialization: impossible if constructor were marked explicit
	f1(a1);

	// f2(string("valA2_a_1")); //forbiden: explicit constructor
	f2(A2("valA2_a_2", "valB2_b_2")); // temp unamed variable, classic constructor initialization
	f2(A2{"valA2_a_3", "valB2_b_3"}); // temp unamed variable, direct-list-initialization
	// A2 a2 = {string("valA2_a_4"), string("valB2_b_4")}; //forbiden: explicit constructor

	cout << endl;
}

// output:
// A1 Constructor 1
// valA1_a_1 / defaultB 
// A1 Constructor 2     
// valA1_a_2 / valB1_b_2
// A1 Constructor 2     
// valA1_a_3 / valB1_b_3
// A1 Constructor 2     
// valA1_a_4 / valB1_b_4
// A2 Constructor 2
// valA2_a_2 / valB2_b_2
// A2 Constructor 2
// valA2_a_3 / valB2_b_3

////////////////////////////////////////////////////////////////////////////////////////////////////
// explicit keyword and conversion operator

struct Resource1 {
	// Define conversion operator
	operator bool() const {
		return true;
	}
};

struct Resource1b {
	operator double() const {
		return 10.;
	}
};

struct Resource2 {
	explicit operator bool() const {
		return true;
	}
};

void testResources() {
	Resource1 r1_1; Resource1 r1_2;

	if (r1_1) { cout << "r1_1 ok" << endl; } // use conversion operator
	if (r1_1 == r1_2) { cout << "unexpectedly equals" << endl; }

	Resource1b r1b;
	cout << "r1b size = " << (double)r1b << endl;

	Resource2 r2_1; Resource2 r2_2;

	if (r2_1) { cout << "r2_1 ok" << endl; } // use conversion operator
	// if (r2_1 == r2_2) { ... } // do not compile == operator shall be defined

	cout << endl;
}

// output:
// r1_1 ok
// unexpectedly equals
// r1b size = 10
// r2_1 ok

////////////////////////////////////////////////////////////////////////////////////////////////////
// Copy constructors & copy assignment operator

struct A
{
string a;

A(string a)
	: a(a)
{
	cout << "Constructor: " << this->a << endl;
}

// Copy constructor implicitly defined, if not user defined. Could be
// defaulted (generated by compiler) : A(const A& other) = default;
// deleted: A(const A& other) = delete;
A(const A& other) // reference required but not const
	: a(other.a)
{
	cout << "Copy constructor: " << this->a << endl;
}

//Copy assignable object
// defaulted (generated by compiler) : A& operator=(const A& t) = default;
// deleted: A& operator=(const A& t) = delete;
A& operator=(const A& other) // Argument const and reference not mandatory, using reference for return value not mandatory but better
{
	cout << "Copy assignment operator" << endl;
	if (this != &other) {
		this->a = other.a;
	}
	return *this;
}

};

void useCopyConstructor(A a)
{
	cout << "useCopyConstructor: " << a.a << endl;
}

void testCopy()
{
	A a1("someString");
	A a2(a1);
	cout << a1.a << endl;
	cout << a2.a << endl;

	a2.a = "UpdatedString";
	cout << a1.a << endl;
	cout << a2.a << endl;

	a1 = a2;
	a1.operator=(a2); // same as a1 = a2;
	cout << a1.a << endl;
	cout << a2.a << endl;

	A a3 = a1; // Call copy constructor, same as A a3(a1);
	cout << a3.a << endl;

	useCopyConstructor(a3);

	cout << endl;
}

// output:
// Constructor: someString
// Copy constructor: someString
// someString
// someString
// someString
// UpdatedString
// Copy assignment operator
// Copy assignment operator
// UpdatedString
// UpdatedString
// Copy constructor: UpdatedString
// UpdatedString
// Copy constructor: UpdatedString
// useCopyConstructor: UpdatedString

////////////////////////////////////////////////////////////////////////////////////////////////////
// Move constructors & move assignment operator

struct B
{
int* b;

B(int b) : b(new int)
{
	*this->b = b;
}

~B() // rule of 3: destructor, copy constructor, copy assignment operator shall be defined together
// rule of 5: add move constructor, move assignment operator
// law of the big 2: with RAII (use of smart pointers) destructor left undefined
// rule of 3 and half (redefine swap - copy and swap idiom)
{
	if (b != nullptr) {
		delete b;
		b = nullptr;
	}
}
	
B(B&& other) // reference to rvalue required, could be const
	: b(other.b)
{
	cout << "Move constructor: " << *this->b << endl;
	other.b = nullptr;
}

B& operator=(B&& other) // reference to rvalue required, could be const, using reference for return value not mandatory but better
{
	cout << "Move assignment operator" << endl;
	if (this->b != other.b) {
		if (b != nullptr) {
			delete b;
			b = nullptr;
		}
		this->b = other.b;
		other.b = nullptr;
	}
	return *this;
}
};

void testMove()
{
	B b1(5);
	cout << "b1: " << *b1.b << " / addr: " << b1.b << endl;
	B b2(move(b1));
	cout << "b2: " << *b2.b << " / addr: " << b2.b << endl;
	cout << "b1 addr: " << b1.b << endl;

	B b3(7);
	cout << "b3: " << *b3.b << " / addr: " << b3.b << endl;
	b3 = move(b2);
	cout << "b3: " << *b3.b << " / addr: " << b3.b << endl;
	cout << "b2 addr: " << b2.b << endl;

	cout << endl;
}

// output:
// b1: 5 / addr: 000001C27D4B6200
// Move constructor: 5
// b2: 5 / addr: 000001C27D4B6200
// b1 addr: 0000000000000000
// b3: 7 / addr: 000001C27D4BF830
// Move assignment operator
// b3: 5 / addr: 000001C27D4B6200
// b2 addr: 0000000000000000

////////////////////////////////////////////////////////////////////////////////////////////////////
// Play with reference to rvalue

void f_(int& a) {}
void f(int&& a) {
	a = 5; // a is a lvalue which 'points' to a rvalue
}

void f1(string& s) {}
void f2(string&& s) {}

void fd1(D1& d) {}
void fd2(D1&& d) {}

void playWithRValue()
{
	int a = 5;
	f_(a);
	// f_(2+2); // not ok

	f(3+3);
	// f(a); // not OK
	const int&& z = 2+2;
	const int& y = z;
	int x = y + z;
	// const T&& is weird cause an rvalue reference is made to allow safe use (change/move) of the referenced object,
	// since no side effects should happend on it, because it should not be accessible anywhere.

	string s = "";
	//f1(string("")); // not ok (but ok with MSVC, wrong compiler behavior)
	f2(string(""));
	f1(s);
	//f2(s); // not ok

	D1 d;
	//fd1(D1()); // not ok (but ok with MSVC, wrong compiler behavior)
	fd2(D1());
	fd1(d);
	//fd2(d); // not ok
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Virtual methods behavior

struct F
{
	virtual ~F() = 0;
};

F::~F() {cout << "dtor F" << endl;}

struct F1 final : F
{
	~F1() {cout << "dtor F1" << endl;} // override keyword usable but not meaningful
};

struct F2 final : F
{
	virtual ~F2() {cout << "dtor F2" << endl;}
};

struct F1b
{
	~F1b() = default;
};

struct F2b
{
	virtual ~F2b() = default;
};

struct H
{
	virtual ~H() = 0;
	virtual void f() = 0;
};

H::~H() = default;

struct H1 : H
{
	void f() override
	{
		cout << "H1" << endl;
	}
	void g()
	{
		cout << "gH1" << endl;
	}
};

struct H2 : H1
{
	void f() override
	{
		cout << "H2" << endl;
	}
	void g()
	{
		cout << "gH2" << endl;
	}
};

void virtualMethodsBehavior() {
	F* f = new F1;
	delete f;
	f = new F2;
	delete f;

	F1* f1 = new F1;
	delete f1;
	F2* f2 = new F2;
	delete f2;

	cout << sizeof(F) << " / " << sizeof(F1) << " / " << sizeof(F2) << " / " << sizeof(F1b) << " / " << sizeof(F2b)
	     << " // " << sizeof(std::unique_ptr<F>) << " // " << sizeof(std::shared_ptr<F>) << " /// " << sizeof(int*) << endl;

	H* h = new H2;
	h->f();
	H1* h1 = new H2;
	h1->f();
	h1->g();
	delete h; delete h1;

	cout << endl;
}

// output:
// dtor F1
// dtor F
// dtor F2
// dtor F
// dtor F1
// dtor F
// dtor F2
// dtor F
// 8 / 8 / 8 / 1 / 8 // 8 // 16 /// 8
// H2
// H2
// gH1

////////////////////////////////////////////////////////////////////////////////////////////////////
// Cascade move semantics

struct MS {
	MS() = default;

	MS(const MS& other) {
		cout << "MS_copy ";
	}

	MS(MS&& other) {
		cout << "MS_move ";
	}

	void f() {}
};

MS buildMS() {
	cout << "build ";
	return MS();
}

void inner(MS ms) {
	cout << "inner ";
	ms.f();
}

void innerRvalue(MS&& ms) {
	cout << "innerRvalue" << endl;
	ms.f();
}

void outer(MS ms) {
	cout << "outer ";
	inner(ms);
	inner(move(ms));
	// innerRvalue(ms); // ms is not an rvalue
	innerRvalue(move(ms));
}

void outerRvalue(MS&& ms) {
	cout << "outerRvalue ";
	inner(ms);
	inner(move(ms));
	// innerRvalue(ms); // ms is not an rvalue
	innerRvalue(move(ms));
}

void testCascadeMoveSemantics() {
	cout << "-- 1  -- ";
	MS ms1;
	outer(ms1);
	
	cout << "-- 2  -- ";
	MS ms2;
	outer(move(ms2));
	
	cout << "-- 3a -- ";
	outer(MS()); // copy elision
	cout << "-- 3b -- ";
	outer(move(MS())); // copy elision lost with move
	cout << "-- 3c -- ";
	outer(buildMS()); // copy elision
	cout << "-- 3d -- ";
	outer(move(buildMS())); // copy elision lost with move
	
	cout << "-- 4  -- ";
	MS ms5;
	outerRvalue(move(ms5));
	
	cout << "-- 5a -- ";
	outerRvalue(MS()); // copy elision
	cout << "-- 5b -- ";
	outerRvalue(move(MS())); // copy elision lost with move
	cout << "-- 5c -- ";
	outerRvalue(buildMS()); // copy elision
	cout << "-- 5d -- ";
	outerRvalue(move(buildMS())); // copy elision lost with move

	cout << endl;
}

// output (with better alignment):
// -- 1  --       MS_copy outer       MS_copy inner MS_move inner innerRvalue
// -- 2  --       MS_move outer       MS_copy inner MS_move inner innerRvalue
// -- 3a --               outer       MS_copy inner MS_move inner innerRvalue
// -- 3b --       MS_move outer       MS_copy inner MS_move inner innerRvalue
// -- 3c -- build         outer       MS_copy inner MS_move inner innerRvalue
// -- 3d -- build MS_move outer       MS_copy inner MS_move inner innerRvalue
// -- 4  --               outerRvalue MS_copy inner MS_move inner innerRvalue
// -- 5a --               outerRvalue MS_copy inner MS_move inner innerRvalue
// -- 5b --               outerRvalue MS_copy inner MS_move inner innerRvalue
// -- 5c -- build         outerRvalue MS_copy inner MS_move inner innerRvalue
// -- 5d -- build         outerRvalue MS_copy inner MS_move inner innerRvalue

////////////////////////////////////////////////////////////////////////////////////////////////////
// Builder

struct MyClass {
	MS a; // may be const

	// aIn is moved to a: Move constructor of MS is called
	MyClass(MS&& aIn) : a(move(aIn)) {cout << "constructor&&" << endl;}
	MyClass(MS& aIn) : a(move(aIn)) {cout << "constructor&" << endl;}
};

MyClass buildMyClass(MS a) {
	cout << "builder ";
	// Call MyClass constructor "MS&&" with move: Move constructor of MS not called, directly use buildMyClass local argument
	return MyClass(move(a)); // Explicitly indicate that 'a' will be invalidated in MyClass constructor
}

MyClass build2MyClass(MS a) {
	cout << "builder2 ";
	// Call MyClass constructor "MS&": directly use buildMyClass local argument
	return MyClass(a); // Not explicit that 'a' will be invalidated in MyClass constructor
}

void testBuild() {
	MyClass myClass  = buildMyClass(MS());
	MyClass myClass2 = build2MyClass(MS());
	cout << endl;
}

// output:
// builder MS_move constructor&&
// builder2 MS_move constructor&

////////////////////////////////////////////////////////////////////////////////////////////////////
// Return value optimization (copy elision)

class RVO {
public:
	RVO() = default;
	RVO(const RVO& other) {
		cout << "COPY RVO" << endl;
	}
};

RVO rvo1a() {
	RVO rvo;
	return rvo; // NRVO: Named Return Value Optimization
}

RVO rvo1b() {
	return RVO(); // RVO: Return Value Optimization
}

RVO rvo2a(bool cond) {
	RVO rvo1;
	RVO rvo2;
	if (cond) {
		return rvo1;
	} else {
		return rvo2;
	}
}

RVO rvo2b(bool cond) {
	if (cond) {
		return RVO();
	} else {
		return RVO();
	}
}

RVO r2() {
    cout << "r2" << endl;
    return RVO();
}

RVO r1() {
    cout << "r1" << endl;
    return r2();
}

void returnValueOptimization() {
	cout << "Exec rvo1a" << endl;
	RVO rvo_1a = rvo1a();
	cout << "Exec rvo1b" << endl;
	RVO rvo_1b = rvo1b();
	cout << "Exec rvo2a" << endl;
	RVO rvo_2a = rvo2a(true);
	cout << "Exec rvo2b" << endl;
	RVO rvo_2b = rvo2b(true);
	cout << "Exec r1" << endl;
	RVO r1Var = r1();
	cout << endl;
}

// output:
// Exec rvo1a
// COPY RVO
// Exec rvo1b
// Exec rvo2a
// COPY RVO
// Exec rvo2b
// Exec r1
// r1
// r2

////////////////////////////////////////////////////////////////////////////////////////////////////
// Factory on stack

class MyType {
public:
    virtual ~MyType() = 0;

    virtual void f() const = 0;
    virtual void g() const = 0;
};

inline MyType::~MyType() = default;

// Not a true mixin...
// Must be inherited privately
// virtual for 'mm' is not mandatory if inherited privately
class MyMixin {
private:
    int z;
public:
    virtual ~MyMixin() = 0;

    virtual void mm() const {}
};

inline MyMixin::~MyMixin() = default;

class MyTrait : public virtual MyType {
public:
    virtual ~MyTrait() = 0;

    virtual void g() const override {}
};

inline MyTrait::~MyTrait() = default;

class MyTrait2 {
public:
    virtual ~MyTrait2() = 0;

    virtual void h() const {}

protected:
    virtual int getVal() const = 0;
};

inline MyTrait2::~MyTrait2() = default;

// private inheritance avoid object slicing problem with 'MyMixin'
// MyTrait could be inherited publicly or privately (no slicing problem since Trait have no data)
class MyTypeImpl : public virtual MyType, private MyMixin, private MyTrait, public MyTrait2 {
private:
    int a;
public:
    virtual ~MyTypeImpl() = default;

    virtual void f() const override {
        mm();
    }

protected:
    virtual int getVal() const override {return a;}
};

class MyTypeSubImpl final : public MyTypeImpl {
private:
    int b;
public:
    virtual ~MyTypeSubImpl() = default;
};

const MyType& build() {
    // const is mandatory: the l-value 'm' and 'm1' shall be const
    // otherwise the r-value 'MyTypeImpl()' could be lost if we modify 'm' or 'm1'
    // m = somethingElse; // => original object referenced by 'm' is lost
    const MyType& m = MyTypeImpl();
    const MyTypeImpl& m1 = MyTypeImpl();

    // Impossible to allocate an object of abstract type
    // MyType m2 = MyTypeImpl();

    // Return type of build, could not be
    // - MyType (Impossible to allocate an object of abstract type)
    // - MyType& (if using RVO: return MyTypeImpl(); bind non const lvalue ref to an r-value)
    // Could be:
    // - const MyType& (when using RVO: return MyTypeImpl(); but return reference to a local variable !!!)

    // Possible return type of build
    // - MyTypeImpl but return the concrete type
    // - MyType&, MyType* or smart pointer to MyType but use allocation on heap

    MyTypeImpl m3 = MyTypeSubImpl(); // Object slicing: we loose integer 'b' (object slicing could be more tricky than that...)
    MyTypeImpl m4 = MyTypeSubImpl();

    // Forbidden with private inheritance 'Mixin' is not a super type of MyTypeImpl
    // MyMixin& mi = m3;
    // mi = m4; // now m3 contains a mixture of m3 and m4 !

    return MyTypeImpl(); // return reference to temporary
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void instantiationMain()
{
	uniquePtr();
	implicitInstantiation();
	testResources();
	testCopy();
	testMove();
	virtualMethodsBehavior();
	testCascadeMoveSemantics();
	testBuild();
	returnValueOptimization();
}
