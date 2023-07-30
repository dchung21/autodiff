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
        ldVal term = (y[i] - yhat[i]).pow(2);
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

    /*
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
    */
    ldVal b0(2.0);
    ldVal b1(4.0);
    ldVal a(12.0);
    ldVal b(2.0);
    ldVal c(3.0);
    ldVal y1 = (32.0);
    ldVal y2 = (13.0);
    ldVal y3 = (26.0);
    //Y = {y1, y2, y3};
    //X = {a, b, c};
    Y = {y1};
    X = {a};
    std::vector<ldVal> ypred = pred(X, b0, b1);
    ldVal sse = loss(Y, ypred);
    ld seed0 = 1.0f;
    sse.back(seed0);
    printf("Loss: %Lf\n", sse.val());
    
    
    
    for (int i = 0; i < 100; i++) {
        ld seed = 1.0f;
        ypred = pred(X, b0, b1);
        sse = loss(Y, ypred);
        sse.back(seed);
        std::cout << b0.grad() << " " << b1.grad() << std::endl;
        b0.setVal(b0.val() - (0.0001 * b0.grad()));
        b1.setVal(b1.val() - (0.0001 * b1.grad()));
    }
    printf("Loss: %Lf\n", sse.val());
    
}