#include "../exercise.h"
#include <memory>

// READ: `std::shared_ptr` <https://zh.cppreference.com/w/cpp/memory/shared_ptr>
// READ: `std::weak_ptr` <https://zh.cppreference.com/w/cpp/memory/weak_ptr>

// TODO: 将下列 `?` 替换为正确的值
int main(int argc, char **argv) {
    auto shared = std::make_shared<int>(10);
    std::shared_ptr<int> ptrs[]{shared, shared, shared};

    std::weak_ptr<int> observer = shared;
    ASSERT(observer.use_count() == 4, "");

    ptrs[0].reset();
    ASSERT(observer.use_count() == 3, "");

    ptrs[1] = nullptr;
    ASSERT(observer.use_count() == 2, "");

    ptrs[2] = std::make_shared<int>(*shared);   //指向新对象xx，引用为1；原有的ptrs[2]对应shared原始对象的引用计数减1到1
    ASSERT(observer.use_count() == 1, "");

    ptrs[0] = shared;       //2                 //shared原始对象的引用计数增1到2
    ptrs[1] = shared;       //3                 //shared原始对象的引用计数增1到3
    ptrs[2] = std::move(shared);                //ptrs[2]放弃原有对象xx，引用从1清零并释放内存；接管shared变量-实际对象强引用计数不增不减为3，shared变量已清空
    ASSERT(observer.use_count() == 3, "");      
    //ASSERT(shared.use_count() == 0, "");        //test
    //ASSERT(shared  == nullptr, "");             //test
    
    std::ignore = std::move(ptrs[0]);           //？？ptrs[0]也清零，放弃指针引用，实际变量强引用计数变为：2
    //ASSERT(observer.use_count() == 3, "");    //test---
    //ASSERT(ptrs[0].use_count() == 3, "");     //test---
    //ASSERT(ptrs[0].get() != nullptr, "");     //test---
    ptrs[1] = std::move(ptrs[1]);               //首先减1清空，然后赋值增1，综合无影响
    //ASSERT(observer.use_count() == 3, "");    //test---
    ptrs[1] = std::move(ptrs[2]);               //ptrs[2]指针清空，强引计数减1到2；ptrs[1]指向同一对象接收控制权、不影响强计数
    ASSERT(observer.use_count() == 2, "");
    // ASSERT(ptrs[2].use_count() == 0, "");    //test
    // ASSERT(ptrs[2]  == nullptr, "");         //test

    shared = observer.lock();                   //实际对象强计数增加1变为3，shared变量重新有效，共享指向一个变量
    ASSERT(observer.use_count() == 3, "");

    shared = nullptr;                           //实际对象强计数减1变为2
    for (auto &ptr : ptrs) ptr = nullptr;       //所有对象强制清空，引用计数逐次减少至清零释放内存
    ASSERT(observer.use_count() == 0, "");

    shared = observer.lock();                   //
    ASSERT(observer.use_count() == 0, "");

    return 0;
}
