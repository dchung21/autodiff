#ifndef OP_H
#define OP_H

#include "Value.h"

template <typename T>
struct Operation
{
    const Value *lhs;
    // really shouldn't be ran. Should be pure virtual
    virtual Value back(Value &x)
    {
        return Value(0);
    };
    Operation();
    Operation(const Value *lhs)
    {
        this->lhs = lhs;
    }
};

template <typename T>
struct DualOperations : Operation
{
    const Value *rhs;
    DualOperations(const Value *lhs, const Value *rhs) : Operation(lhs)
    {
        this->rhs = rhs;
    }
};

template <typename T>
struct Add : DualOperations
{
    Add(const Value *lhs, const Value *rhs) : DualOperations(lhs, rhs)
    {
    }
    Value back(Value &x) override
    {
        return (x == *(this->lhs)) + (x == *(this->rhs));
    }
};

template <typename T>
struct Subtract : DualOperations
{
    Subtract(const Value *lhs, const Value *rhs) : DualOperations(lhs, rhs)
    {
    }
    Value back(Value &x) override
    {
        return (x == *(this->lhs)) - (x == *(this->rhs));
    }
};

template <typename T>
struct Multiply : DualOperations
{
    Multiply(const Value *lhs, const Value *rhs) : DualOperations(lhs, rhs)
    {
    }
    Value back(Value &x) override
    {
        return (x == *(this->lhs)) ? *(this->rhs) : *(this->lhs);
    }
};

template <typename T>
struct Pow : Operation
{
    float k;
    Value *_k;

    Pow(float k)
    {
        this->k = k;
        _k = new Value(k);
    }

    ~Pow()
    {
        delete _k;
    }

    Value back(Value &x) override
    {
        return *(_k)*x.pow(k - 1);
    }
};

#endif