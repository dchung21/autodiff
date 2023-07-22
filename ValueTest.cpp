#include "ValueTest.h"
#include <random>
#include <cassert>
#include <iostream>
#define ll long long  

#include <queue>

int main() {
    double lower_bound = -100000;
    double upper_bound = 100000;
    std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
    std::default_random_engine re;
    double rand_a = unif(re);
    double rand_b = unif(re);
    double rand_c = unif(re);
    double rand_d = unif(re);

    Value rand_a_val(rand_a);
    Value rand_b_val(rand_b);
    Value rand_c_val(rand_c);
    Value rand_d_val(rand_d);

    printf("a=%f, b=%f, c=%f, d=%f\n", rand_a, rand_b, rand_c, rand_d);
    Value addFx = rand_a_val + rand_b_val;
    Value multFx = (Value(4.32) * addFx) + rand_c_val;
    printf("addFx=%f, multFx=%f\n", addFx.val(), multFx.val());


    // OPERATIONS TEST BEGIN
    
    assert(testAdd(rand_a_val, rand_b_val));
    assert(testSubtract(rand_a_val, rand_b_val));
    assert(testMult(rand_a_val, rand_b_val));
    assert(testDivide(rand_a_val, rand_b_val));
    // END OPERATIONS TEST 

    // COMPARISON TEST BEGIN
    assert(testGreaterThan(rand_a_val, rand_b_val));
    assert(testLessThan(rand_a_val, rand_b_val));
    // END COMPARISON TESTS
     
    assert(testMultiOp(rand_a_val, rand_b_val, rand_c_val, rand_d_val));
    assert(gradTest(rand_a_val, rand_b_val, rand_c_val));
    std::cout << "Value: Test cases passed" << std::endl;
}

bool testInitialize() {
    int64_t a = 42;
    float b = 32.4;
    double c = 12.5;
    ll d = 555555555;
    Value valA(a);
    Value valB(b);
    Value valC(c);
    Value valD(d);
    
    return valA.val() == a && valB.val() == b &&
           valC.val() == c && valD.val() == d;
}

bool testAdd(Value<double>& lhs, Value<double>& rhs) {
    Value sum = lhs + rhs;
    return sum.val() == (lhs.val() + rhs.val());
}

bool testSubtract(Value<double>& lhs, Value<double>& rhs) {
    Value diff = lhs - rhs;
    return diff.val() == (lhs.val() - rhs.val());
}

bool testMult(Value<double>& lhs, Value<double>& rhs) {
    Value prod = lhs * rhs;
    return prod.val() == (lhs.val() * rhs.val());
}

bool testDivide(Value<double>& lhs, Value<double>& rhs) {
    Value quot = lhs / rhs;
    return quot.val() == (lhs.val() / rhs.val());
}


bool testEqual(Value<double>& lhs, Value<double>& rhs) {
    return lhs == rhs;
}

bool testGreaterThan(Value<double>& lhs, Value<double>& rhs) {
    return (lhs > rhs) == (lhs.val() > rhs.val());
}

bool testLessThan(Value<double>& lhs, Value<double>& rhs) {
    return (lhs < rhs) == (lhs.val() < rhs.val()); 
}

bool testMultiOp(Value<double>& a, Value<double>& b, Value<double>& c, Value<double>& d) {
    Value res = a + b + (c * d);
    printf("yep cock\n");
    return res == ( (a.val() + b.val()) + (c.val() * d.val()) );
}

bool gradTest(Value <double>& a, Value<double>& b, Value<double>& c) {
    printf("a=%f, b=%f, c=%f\n", a.val(), b.val(), c.val());
    Value addFx = a+b;
    Value z(4.32);
    Value multFx = (z * addFx) + c;

    double seed = 1.0f;
    multFx.back(seed);
    std::cout << "df/da= " << a.grad() << std::endl;
    std::cout << "df/db= " << b.grad() << std::endl;
    std::cout << "df/dc= " << c.grad() << std::endl;
    return true;
}
