#include "../exercise.h"

// READ: 左值右值（概念）<https://learn.microsoft.com/zh-cn/cpp/c-language/l-value-and-r-value-expressions?view=msvc-170>
// READ: 左值右值（细节）<https://zh.cppreference.com/w/cpp/language/value_category>
// READ: 关于移动语义 <https://learn.microsoft.com/zh-cn/cpp/cpp/rvalue-reference-declarator-amp-amp?view=msvc-170#move-semantics>
// READ: 如果实现移动构造 <https://learn.microsoft.com/zh-cn/cpp/cpp/move-constructors-and-move-assignment-operators-cpp?view=msvc-170>

// READ: 移动构造函数 <https://zh.cppreference.com/w/cpp/language/move_constructor>
// READ: 移动赋值 <https://zh.cppreference.com/w/cpp/language/move_assignment>
// READ: 运算符重载 <https://zh.cppreference.com/w/cpp/language/operators>

class DynFibonacci {
    size_t *cache;
    int cached;

public:
    // TODO: 实现动态设置容量的构造器
    DynFibonacci(int capacity): cache(new size_t[capacity]{0,1}), cached(capacity) {}

    // TODO: 实现移动构造器
    DynFibonacci(DynFibonacci &&other) noexcept 
        : cache(other.cache), cached(other.cached) 
    {
        // 把源对象的指针置空，让源对象变成「安全空状态」
        other.cache = nullptr;
        other.cached = 0;
    }


    // TODO: 实现移动赋值
    // NOTICE: ⚠ 注意移动到自身问题 ⚠
    DynFibonacci &operator=(DynFibonacci &&other) noexcept {
        // 第一步：处理【移动到自身】的边界情况，直接返回*this
        if (this == &other) {
            return *this;
        }

        // 第二步：释放当前对象的已有资源，避免内存泄漏
        delete[] cache;

        // 第三步：「偷走」源右值对象的资源
        cache = other.cache;
        cached = other.cached;

        // 第四步：源对象置空，安全收尾
        other.cache = nullptr;
        other.cached = 0;

        return *this;
    }

    // TODO: 实现析构器，释放缓存空间
    ~DynFibonacci()
    {
        delete[] cache;
    };

    // TODO: 实现正确的缓存优化斐波那契计算
    size_t operator[](int i) {
        if(i<0 || i>=cached)    return  0;
        if(i<2 || cache[i]>0)   return cache[i];
        for (int loccached = 2 ; loccached <=i; ++loccached) {
            cache[loccached] = cache[loccached - 1] + cache[loccached - 2];
        }
        return cache[i];
    }

    // NOTICE: 不要修改这个方法
    size_t operator[](int i) const {
        ASSERT(i <= cached, "i out of range");
        return cache[i];
    }

    // NOTICE: 不要修改这个方法
    bool is_alive() const {
        return cache;
    }
};

int main(int argc, char **argv) {
    DynFibonacci fib(12);
    ASSERT(fib[10] == 55, "fibonacci(10) should be 55");

    DynFibonacci const fib_ = std::move(fib);
    ASSERT(!fib.is_alive(), "Object moved");
    ASSERT(fib_[10] == 55, "fibonacci(10) should be 55");

    DynFibonacci fib0(6);
    DynFibonacci fib1(12);

    fib0 = std::move(fib1);
    fib0 = std::move(fib0);
    ASSERT(fib0[10] == 55, "fibonacci(10) should be 55");

    return 0;
}
