#include <cmath>
#include <vector>
#include <memory>

template <typename T> class Value;

template<typename T>
struct Operation
{
    const Value<T> *lhs;
    const Value<T> *rhs;
    // really shouldn't be ran. Should be pure virtual
    virtual Value<T> back(const Value<T> &x) = 0;
    Operation(const Value<T> *lhs = nullptr, const Value<T> *rhs = nullptr)
    {
        this->lhs = lhs;
        this->rhs = rhs;
    }
};

template<typename T>
struct Add : Operation<T>
{
    Add(const Value<T> *lhs, const Value<T> *rhs) : Operation<T>(lhs, rhs)
    {
    }
    Value<T> back(const Value<T> &x) override
    {
        return (x == *(this->lhs)) + (x == *(this->rhs));
    }
};

template<typename T>
struct Subtract : Operation<T>
{
    Subtract(const Value<T> *lhs, const Value<T> *rhs) : Operation<T>(lhs, rhs)
    {
    }
    Value<T> back(const Value<T> &x) override
    {
        return (x == *(this->lhs)) - (x == *(this->rhs));
    }
};

template<typename T>
struct Multiply : Operation<T>
{
    Multiply(const Value<T> *lhs, const Value<T> *rhs) : Operation<T>(lhs, rhs)
    {
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

    Value &operator=(const Value &rhs)
    {
        this->val = rhs.val;
        this->children = rhs.children;
        return *this;
    }

    Value &operator+=(const Value &rhs)
    {
        this->val += rhs.val;
        std::shared_ptr<Add<T>> op(new Add<T>(this, &rhs));
        this->children.push_back(op);
        return *this;
    }

    Value &operator-=(const Value &rhs)
    {
        this->val -= rhs.val;
        std::shared_ptr<Subtract<T>> op(new Subtract<T>(this, &rhs));
        this->children.push_back(op);
        return *this;
    }

    Value &operator*=(const Value &rhs)
    {
        this->val *= rhs.val;
        std::shared_ptr<Multiply<T>> op(new Multiply<T>(this, &rhs));
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
};

