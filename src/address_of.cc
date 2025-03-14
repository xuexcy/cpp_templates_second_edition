/**
@question: operator& 用于获取object的的地址，但是 & 被重载后如何获取 object 的地址
*/

#include <cassert>
#include <memory>
#include <print>

#include "utils/main_decorator.h"

class nonaddressable {
public:
    typedef double useless_type;
private:
    useless_type operator&() const;
};

template <class T>
T* addressof(T& v) {
    // https://zhuanlan.zhihu.com/p/718233847
    // T& -> const volatile char& -> char& -> T*
    return reinterpret_cast<T*>(& const_cast<char&>(
        reinterpret_cast<const volatile char&>(v)
    ));
}
int main() {
    utils::MainDecorator::Access();
    nonaddressable na;
    // 编译错误，因为 operator& is private
    // 即使 operator& 不是 private，返回值也不是 object 的地址
    // nonaddressable* na_ptr = &na;
    std::print("address of {} is {}\n", "na", (void*)addressof(na));

    // c++11
    std::print("address of {} is {}\n", "na", (void*)std::addressof(na));
    assert(addressof(na) == std::addressof(na));

}
