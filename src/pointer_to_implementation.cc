/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/08 19:25:48
# Desc    :
########################################################################

pointer_to_implementation(pImpl, 又叫做 opaque point)

@question: 对于公开的 public.h 中的 class Public，用户只关心其中的小部分功能接口，如果功能的实现发生变化，导致
class Public 需要新增了一些成员变量/函数，那么 public.h 中声明的 class Public 也就需要发生变化,这会导致依赖 public.h
头文件的编译单元需要重新编译(比如当前这个 main.cc)。如何在用户不感知头文件变化的情况下更新 class Public 的具体实现

@solution: 将用户不关心的实现细节放到另一个 class Impl 类中并在 public.cpp 中实现完整定义，在 class Public 中
添加一个 Impl* impl 指针，所有公开的功能可以通过 impl 指针去调用 class Impl 中具体的功能函数，这样如果需要
发生成员变量/函数变化，只需要更新 Impl 的实现即可，
不用更改 public.h

@refs:
https://en.cppreference.com/w/cpp/language/pimpl
https://zhuanlan.zhihu.com/p/676910057

pImpl 优点:
1. 隐藏头实现细节: 公开的头文件只包含了用户需要的一些接口，如果没有 impl，公开的头文件就会暴漏很多实现细节(比如成员变量，依赖的其他头文件/三方库等)
2. 减少依赖: 头文件不依赖实现细节需要的库或组件等，依赖更简单
3. 提高封装性: 对外只提供了用户需要的功能，具体实现细节都放到了 impl 中，也方便了 impl 的变更和调整
4. 优化编译时间: 由于公开的头文件结构没有改变，其他依赖该头文件的源文件也不用重新编译; 如果 impl 发生变化，其他编译好的 lib 直接链接到 impl 的编译结果即可

pImpl 缺点:
1. 所需功能都需通过 impl 指针进行访问，牺牲了一点函数调用的性能
2. impl 指针需要使用动态分配内存

pImpl 坑:
1. 由于类中含有 impl 不完整类型，会出现一些编译问题
2. 对于 impl 指针是原始指针还是unique_ptr、shared_ptr，在(拷贝/移动) + (构造/赋值) 上会有一定区别

*/

#include <print>
#include <string>

#include "pointer_to_implementation_deps/book.h"
#include "pointer_to_implementation_deps/book_template_with_pimpl.h"
#include "pointer_to_implementation_deps/book_with_pimpl.h"
#include "utils/main_decorator.h"

/**
假设用户只关心 Book::print()
如下 bad_case::Book，如果 Book 中新添 std::string title_ 成员用于 print()，那么 book.h 的头文件肯定要发生变化，
(当然，也要更新 book.cpp 中 print() 的实现)，这就导致当前这个 main.cc 需要重新编译
如下 solution::Book，如果 Book 中还要新添其他字段用于 print，那么不用更改 book_with_pimpl.h，而只用更改
book_with_impl.cpp，这样当前这个 main.cc 就不用重新编译，而只用链接到更新后的 library book_with_pimpl

另外，对于带模板参数的公开类型，需要一些办法去避免在 pimpl 实现方式下的包含、使用模板参数、值，因为模板的实例化必须要
完整的定义，然后才能编译展开后的模板类。
如下 bad_case::BookWithTemplate，其构造、析构、print 等函数，都需要在 .h 实现，但在 .h 中调用 impl_->print()
将无法编译，因为此时并不知道 Impl 的完整定义。
解决办法就是使用 has_template_derived + no_template_base + no_template_base::Impl 三个类，将模板信息存于
has_template_derived，将一些实现也放到 derived 中，并通过 base 传到 impl 中。

个人认为在模板类使用 pImpl 不太优雅(derived -> base -> impl 的信息传递比较复杂，让某些功能实现变得复杂)
 */

struct MyType {
    std::string to_string() const {
        return std::format("[MyType: x = {}, y = {}]", x, y);
    }
    int x{0};
    double y{0};
};
template <>
std::string solution_for_template::to_string<MyType>(const MyType& t) {
    return t.to_string();
}
void run() {
    {
        bad_case::Book book;
        book.print();
    }
    {
        solution::Book book;
        book.print();
    }
    // 无法编译
    // {
    //     bad_case::BookWithTemplate<int> book;
    //     book.set_extra(3);
    //     book.print();
    // }
    {
        solution_for_template::BookWithTemplate<int> book;
        book.set_extra(3);
        book.print();
        book.set_extra(8);
        book.print();
    }
    {
        solution_for_template::BookWithTemplate<MyType> book;
        book.set_extra(MyType { 3, 9.0});
        book.print();
        MyType my_type { 4, 2.0};
        book.set_extra(my_type);
        book.print();
    }
}

int main() {
    utils::MainDecorator::Access();
    run();
    return 0;
}

