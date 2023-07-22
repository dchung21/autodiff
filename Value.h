#ifndef VALUE_H
#define VALUE_H

#include <cmath>
#include <vector>
#include <memory>
#include <queue>

template <typename T>
class Value;

#define ValPtr std::shared_ptr<Value<T>>
#define OpPtr std::shared_ptr<Operation<T>>

template <typename T>
struct Operation
{
    ValPtr lhs;
    ValPtr rhs;
    virtual void back(T &seed) = 0;
    virtual std::shared_ptr<Operation<T>> clone() = 0;
    Operation(ValPtr lhs = nullptr, ValPtr rhs = nullptr)
    {
        this->lhs = lhs;
        this->rhs = rhs;
    }
    Operation(const Operation &copy)
    {
        this->lhs = copy.lhs;
        this->rhs = copy.rhs;
    }
};

template <typename T>
struct Add : Operation<T>
{
    Add(ValPtr lhs, ValPtr rhs) : Operation<T>(lhs, rhs) {}
    Add(const Add &copy) : Operation<T>(copy.lhs, copy.rhs) {}
    std::shared_ptr<Operation<T>> clone()
    {
        return std::make_shared<Add>(*this);
    }
    void back(T &seed) override
    {
        this->lhs->data->grad += seed;
        this->rhs->data->grad += seed;
    }
};

template <typename T>
struct Subtract : Operation<T>
{
    Subtract(ValPtr lhs, ValPtr rhs) : Operation<T>(lhs, rhs) {}
    Subtract(const Subtract &copy) : Operation<T>(copy.lhs, copy.rhs) {}
    std::shared_ptr<Operation<T>> clone()
    {
        return std::make_shared<Subtract>(*this);
    }
    void back(T &seed) override
    {
        this->lhs->data->grad += seed;
        this->rhs->data->grad += seed;
    }
};

template <typename T>
struct Multiply : Operation<T>
{
    Multiply(ValPtr lhs, ValPtr rhs) : Operation<T>(lhs, rhs) {}
    Multiply(const Multiply &copy) : Operation<T>(copy.lhs, copy.rhs)
    {
    }
    std::shared_ptr<Operation<T>> clone()
    {
        return std::make_shared<Multiply>(*this);
    }
    void back(T &seed) override
    {
        this->lhs->data->grad += this->rhs->data->val * seed;
        this->rhs->data->grad += this->lhs->data->val * seed;
    }
};

template <typename T>
struct Pow : Operation<T>
{
    float k;
    Value<T> *_k;

    Pow(float k)
    {
        this->k = k;
        _k = new Value<T>(k);
    }
    ~Pow()
    {
        delete _k;
    }
    Value<T> back(Value<T> &x) override
    {
        return *(_k)*x.pow(k - 1);
    }
};

template <typename T>
class Value
{
private:
    std::vector<std::shared_ptr<Operation<T>>> children;

    struct Data
    {
        T val;
        T grad = 0;

        Data &operator=(const Data &other)
        {
            this->val = other.val;
            this->grad = other.grad;
        }
    };

    std::shared_ptr<Data> data;

public:
    friend struct Add<T>;
    friend struct Subtract<T>;
    friend struct Multiply<T>;

    Value(T val)
    {
        this->data = std::make_shared<Data>();
        this->data->val = val;
    }

    // Deep copying of Data Struct or not
    Value(const Value &copy, bool deep = false)
    {
        if (deep)
        {
            this->data = std::make_shared<Data>();
            this->data->val = copy.data->val;
            this->data->grad = copy.data->grad;
        }
        else
            this->data = copy.data;
        for (auto &x : copy.children)
        {
            this->children.push_back(x->clone());
        }
    }
    Value(std::shared_ptr<Data> data)
    {
        this->data = data;
    }
    T val()
    {
        return this->data->val;
    }
    T grad()
    {
        return this->data->grad;
    }
    Value &operator=(const Value &rhs)
    {
        if (this == &rhs)
            return *this;
        this->val() = rhs->val();
        this->children.clear();
        for (auto &x : rhs.children)
        {
            this->children.push_back(x->clone());
        }
        return *this;
    }
    Value &operator+=(Value &rhs)
    {
        ValPtr _rhs = std::make_shared<Value>(rhs, false);
        ValPtr _lhs = std::make_shared<Value>(*this);
        std::shared_ptr<Add<T>> op = std::make_shared<Add<T>>(_lhs, _rhs);
        this->data->val += rhs.data->val;
        this->children.push_back(op);
        return *this;
    }
    Value &operator-=(Value &rhs)
    {
        ValPtr _rhs = std::make_shared<Value>(rhs.data);
        ValPtr _lhs = std::make_shared<Value>(*this);
        std::shared_ptr<Subtract<T>> op = std::make_shared<Subtract<T>>(_lhs, _rhs);
        this->data->val -= rhs.data->val;
        this->children.push_back(op);
        return *this;
    }
    Value &operator*=(Value &rhs)
    {
        ValPtr _rhs = std::make_shared<Value>(rhs.data);
        ValPtr _lhs = std::make_shared<Value>(*this);
        std::shared_ptr<Multiply<T>> op = std::make_shared<Multiply<T>>(_lhs, _rhs);
        this->data->val *= rhs.data->val;
        this->children.push_back(op);
        return *this;
    }
    Value &operator/=(Value &rhs)
    {
        this->data->val /= rhs.val();
        return *this;
    }
    friend Value operator+(Value lhs, Value &rhs)
    {
        ValPtr _rhs = std::make_shared<Value>(rhs);
        ValPtr _lhs = std::make_shared<Value>(lhs);
        Value res(lhs.val() + rhs.val());
        std::shared_ptr<Add<T>> op = std::make_shared<Add<T>>(_lhs, _rhs);
        res.children.push_back(op);
        return res;
    }
    friend Value operator+(Value lhs, Value &&rhs)
    {
        Value _rhs(std::move(rhs.data));
        _rhs.children = std::move(rhs.children);
        lhs += _rhs;
        return lhs;
    }
    friend Value operator-(Value lhs, Value &rhs)
    {
        ValPtr _rhs = std::make_shared<Value>(rhs);
        ValPtr _lhs = std::make_shared<Value>(lhs);
        Value res(lhs.val() - rhs.val());
        std::shared_ptr<Subtract<T>> op = std::make_shared<Subtract<T>>(_lhs, _rhs);
        res.children.push_back(op);
        return res;
    }
    friend Value operator/(Value lhs, Value &rhs)
    {
        ValPtr _rhs = std::make_shared<Value>(rhs);
        ValPtr _lhs = std::make_shared<Value>(lhs);
        Value res(lhs.val() / rhs.val());
        // std::shared_ptr<Subtract<T>> op = std::make_shared<Subtract<T>>(_lhs, _rhs);
        // res.children.push_back(op);
        return res;

        return lhs;
    }
    friend Value operator*(Value lhs, Value &rhs)
    {
        ValPtr _rhs = std::make_shared<Value>(rhs);
        ValPtr _lhs = std::make_shared<Value>(lhs);
        Value res(lhs.val() * rhs.val());
        std::shared_ptr<Multiply<T>> op = std::make_shared<Multiply<T>>(_lhs, _rhs);
        res.children.push_back(op);
        return res;
    }
    friend Value operator*(Value lhs, Value &&rhs)
    {
        Value _rhs(std::move(rhs.data));
        _rhs.children = std::move(rhs.children);
        lhs *= _rhs;
        return lhs;
    }
    // only support float values for now
    inline Value &pow(float k)
    {
        this->val() = std::pow(this->val(), k);
        return *this;
    }
    inline Value &pow(double k)
    {
        this->val() = std::pow(this->val(), k);
        return *this;
    }
    inline Value &pow(int k)
    {
        this->val() = std::pow(this->val(), k);
        return *this;
    }
    inline Value &pow(long double k)
    {
        this->val() = std::pow(this->val(), k);
        return *this;
    }
    inline bool operator==(const Value &rhs) const
    {
        return this->data->val == rhs.data->val;
    }
    inline bool operator!=(const Value &rhs)
    {
        return !(this->val() == rhs);
    }
    inline bool operator<(const Value &rhs)
    {
        return this->val() < rhs.data->val;
    }
    inline bool operator>(const Value &lhs)
    {
        return this->val() > lhs.data->val;
    }
    inline bool operator<=(const Value &rhs)
    {
        return !(this->val() > rhs.val());
    }
    inline bool operator>=(const Value &lhs)
    {
        return !(lhs < this->val());
    }
    void back(T &seed)
    {
        if (this->children.empty())
        {
            return;
        }

        OpPtr op = this->children.back();
        op->back(seed);
        const ValPtr lhs = op->lhs;
        const ValPtr rhs = op->rhs;
        if (lhs)
        {
            printf("Grad: %f at %f\n", lhs->val(), lhs->grad());
            seed = lhs->grad();
            lhs->back(seed);
        }

        if (rhs)
        {
            printf("Grad: %f at %f\n", rhs->val(), rhs->grad());
            seed = rhs->grad();
            rhs->back(seed);
        }
    }
};

#endif