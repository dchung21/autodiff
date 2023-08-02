// Linear Regression training via gradient descent example

#include "Value.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define ld long double
#define ldVal Value<ld>

Value<ld> loss(std::vector<ldVal>& y, std::vector<ldVal> yhat)
{
    ldVal _loss(0);
    int N = y.size();
    for (int i = 0; i < N; i++)
    {
        ldVal term = (y[i] - yhat[i]).pow(2.0f);
        _loss += term;
    }
    return _loss;
}

// Two parameter model b0 (Intercept), b1 (Slope)
std::vector<ldVal> pred(std::vector<ldVal> X, ldVal &b0, ldVal &b1)
{
    int N = X.size();
    std::vector<ldVal> y(N);
    for (int i = 0; i < N; i++)
    {
        y[i] = b0 + (b1 * X[i]);
    }

    return y;
}

int main()
{
    std::ifstream inputFile("data.csv");
    if (!inputFile)
    {
        std::cout << "Error opening the file." << std::endl;
        return 1;
    }

    std::vector<ldVal> X;
    std::vector<ldVal> Y;
    
    std::string line;
    while (getline(inputFile, line))
    {
        std::stringstream ss(line);
        std::string value;

        // Read the first column value
        if (getline(ss, value, ','))
        { 
            ldVal val(std::stold(value));
            X.push_back(val);
        }

        // Read the second column value
        if (getline(ss, value, ','))
        {
            ldVal val(std::stold(value));
            Y.push_back(val);

        }
    }

    inputFile.close();
   
    
    ldVal b0(2.0f);
    ldVal b1(4.0f);
    std::vector<ldVal> ypred = pred(X, b0, b1);
    ldVal sse = loss(Y, ypred);
    ld seed0 = 1.0f;
    ld _sse = sse.val();

    for (int i = 0; i < 1000; i++) {
        ld seed = 1.0f;
        ypred = pred(X, b0, b1);
        sse = loss(Y, ypred);
        sse.zero_grad();
        sse.back(seed);
        if (sse.val() > _sse)
            break;
        _sse = sse.val();
        b0.setVal(b0.val() - (0.0001 * b0.grad()));
        b1.setVal(b1.val() - (0.0001 * b1.grad()));
    }
    printf("Loss: %Lf\n", _sse);
    
}