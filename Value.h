#include <cmath>
#include <vector>
#include <memory>
#include <queue>

#define ValPtr std::shared_ptr<Value<T>>

template <typename T> class Value;

template<typename T>
struct Operation
{
    ValPtr lhs;
    ValPtr rhs;
    virtual Value<T> back(const Value<T> &x) = 0;
    virtual std::shared_ptr<Operation<T>> clone() = 0;
    Operation(ValPtr lhs = nullptr, ValPtr rhs = nullptr)
    {
        this->lhs = lhs;
        this->rhs = rhs;
    }

    Operation(const Operation& copy) {
        this->lhs = copy.lhs;
        this->rhs = copy.rhs;
    }
};

template<typename T>
struct Add : Operation<T>
{
    Add(ValPtr lhs, ValPtr rhs) : Operation<T>(lhs, rhs)
    {
    }
    Add(const Add& copy) : Operation<T>(copy.lhs, copy.rhs) 
    {
    }

    std::shared_ptr<Operation<T>> clone() {
        return std::make_shared<Add>(*this);
    }
    Value<T> back(const Value<T> &x) override
    {
        return (x == *(this->lhs)) + (x == *(this->rhs));
    }
};

template<typename T>
struct Subtract : Operation<T>
{
    Subtract(ValPtr lhs, ValPtr rhs) : Operation<T>(lhs, rhs)
    {
    }
    Subtract(const Subtract& copy) : Operation<T>(copy.lhs, copy.rhs)
    {
    }
    std::shared_ptr<Operation<T>> clone() {
        return std::make_shared<Subtract>(*this);
    }
    Value<T> back(const Value<T> &x) override
    {
        return (x == *(this->lhs)) - (x == *(this->rhs));
    }
};

template<typename T>
struct Multiply : Operation<T>
{
    Multiply(ValPtr lhs, ValPtr rhs) : Operation<T>(lhs, rhs)
    {
    }
    Multiply(const Multiply& copy) : Operation<T>(copy.lhs, copy.rhs)
    {
    }
    std::shared_ptr<Operation<T>> clone() {
        return std::make_shared<Multiply>(*this);
    }
    Value<T> back(const Value<T> &x) override
    {
        return (x == *(this->lhs)) ? *(this->rhs) : *(this->lhs);
    }
};

template<typename T>
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
public:
    T val;
    std::vector<std::shared_ptr<Operation<T>>> children;

    Value(T val)
    {
        this->val = val;
    }

    Value(const Value& copy) {
        this->val = copy.val;
        std::vector<std::shared_ptr<Operation<T>>> copyChildren;
        for (auto& x : copy.children) {
            this->children.push_back(x->clone());
        }
    }

    Value &operator=(const Value &rhs)
    {
        this->val = rhs.val;
        this->children.clear();
        for (auto& x : rhs.children) {
            this->children.push_back(x->clone());
        }
        return *this;
    }

    Value &operator+=(const Value &rhs)
    {
        ValPtr _rhs = std::make_shared<Value<T>>(rhs);
        Value copy(*this);
        ValPtr self = std::make_shared<Value<T>>(copy);
        this->val += rhs.val;
        std::shared_ptr<Add<T>> op = std::make_shared<Add<T>>(Add<T>(self, _rhs));
        this->children.push_back(op);
        return *this;
    }

    Value &operator-=(const Value &rhs)
    {
        ValPtr _rhs = std::make_shared<Value<T>>(rhs);
        Value copy(*this);
        ValPtr self = std::make_shared<Value<T>>(copy);
        this->val -= rhs.val;
        std::shared_ptr<Subtract<T>> op(new Subtract<T>(self, _rhs));
        this->children.push_back(op);
        return *this;
    }

    Value &operator*=(const Value &rhs)
    {
        ValPtr _rhs = std::make_shared<Value<T>>(rhs);
        Value copy(*this);
        ValPtr self = std::make_shared<Value<T>>(copy);
        this->val *= rhs.val;
        std::shared_ptr<Multiply<T>> op(new Multiply<T>(self, _rhs));
        this->children.push_back(op);
        return *this;
    }

    Value &operator/=(const Value &rhs)
    {
        this->val /= rhs.val;
        return *this;
    }

    friend Value operator+(Value lhs, const Value &rhs)
    {
        lhs += rhs;
        return lhs;
    }

    friend Value operator-(Value lhs, const Value &rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    friend Value operator/(Value lhs, const Value &rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    friend Value operator*(Value lhs, const Value &rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    // only support float values for now
    inline Value &pow(float k)
    {
        this->val = std::pow(this->val, k);
        return *this;
    }

    inline Value &pow(double k)
    {
        this->val = std::pow(this->val, k);
        return *this;
    }

    inline Value &pow(int k)
    {
        this->val = std::pow(this->val, k);
        return *this;
    }

    inline Value &pow(long double k)
    {
        this->val = std::pow(this->val, k);
        return *this;
    }

    inline bool operator==(const Value &rhs) const
    {
        return this->val == rhs.val;
    }

    inline bool operator!=(const Value &rhs)
    {
        return !(this->val == rhs);
    }

    inline bool operator<(const Value &rhs)
    {
        return this->val < rhs.val;
    }

    inline bool operator>(const Value &lhs)
    {
        return this->val > lhs.val;
    }

    inline bool operator<=(const Value &rhs)
    {
        return !(this->val > rhs);
    }

    inline bool operator>=(const Value &lhs)
    {
        return !(lhs < this->val);
    }

    Value& back(const Value &target) {
        // construct DAG 
        std::queue<Value*> q;
        for (std::shared_ptr<Operation<T>> &x : this->children) {
            if (x->lhs == target) 
                q.push(x->lhs);
            else if (x->rhs == target)
                q.push(x->rhs);
        }

        while (!q.empty()) {
            Value* fx = q.front();
            q.pop();
            // take derivative 
            fx->back();

            for (std::shared_ptr<Operation<T>>& x : fx->children) {
                if (x->lhs == target)
                    q.push(x->lhs);
                else if (x->rhs == target)
                    q.push(x->rhs);
            }
        }

        return Value(0);
    }
};

