#include "../exercise.h"
const int  cLen   =   128;
struct Fibonacci {
    unsigned long long cache[cLen]={0,1};
    int cached=1;

    // TODO: 实现正确的缓存优化斐波那契计算
    // unsigned long long get(int i) {
    //     for (; false; ++cached) {
    //         cache[cached] = cache[cached - 1] + cache[cached - 2];
    //     }
    //     return cache[i];
    // }
    unsigned long long get(int i) {
        if(i>=0 && i<=cached)     return  cache[i];
        int loccached  =   cached+1;
        for (; loccached <cLen && loccached <=i; ++loccached) {
            cache[loccached] = cache[loccached - 1] + cache[loccached - 2];
            cached    =   loccached;
        }
        if(i<0) return 0;
        return cache[i];
    }
};

int main(int argc, char **argv) {
    // TODO: 初始化缓存结构体，使计算正确
    Fibonacci fib;
    ASSERT(fib.get(10) == 55, "fibonacci(10) should be 55");
    std::cout << "fibonacci(10) = " << fib.get(10) << std::endl;
    return 0;
}
