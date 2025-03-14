/**
delete 一个不完整的 class，行为将是未定义的, 在编译时，编译器会提示 warning，但是代码还是可以编译通过,
我们期望直接编译失败而不是warning，因为编译成功后 delete 行为可能不符合预期

如下 bad_case 中, 使用 deleter.h 中的 delete_object，但是在 delete.h 中 Object 的定义是不完整的
此时代码可以编译通过，但是行为并不符合预期, delete obj 没有调用 class Object 的析构函数

@solution:
在 delete 检查 class 的 size，如果能够确定 size 则说明 class 的定义是完整的
如果 T 的定义不完整，那么不同的编译期可能会编译失败或者返回0, 则下面代码会在 sizeof(T) 或 char[-1] 时编译失败
using new_type = char[sizeof(T) ? 1 : -1];
delete obj;
*/

#include <print>

#include "cpp_utils/util.h"

#include "checked_delete_deps/checked_deleter.h"
#include "checked_delete_deps/deleter.h"
#include "checked_delete_deps/object.h"
#include "utils/main_decorator.h"

namespace bad_case {
/**
编译时会出现warning:
src/checked_delete_deps/deleter.cpp:9:5: warning: deleting pointer to incomplete type 'Object' is incompatible with C++2c and may cause undefined behavior [-Wdelete-incomplete]
运行时，输出如下，delete 并没有调用 Object 的析构函数
@stdout:
Start bad_case
start delete Object
End bad_case

 */
void run() {
    std::println("\nStart bad_case");
    Object* p = new Object;
    delete_object(p);
    std::println("End bad_case");
}
}  // namespace bad_case

namespace solution {
/**
checked_delete_object 中必须能看到完整的 Object 定义(即checked_deleter.h 中的 #include "object.h")
否则会编译失败，而不是执行成功并调用 Object::~Object()
@solution:
Start solution
start checked_delete Object
Object::~Object()
End solution
 */
void run() {
    std::println("\nStart solution");
    Object* obj = new Object;
    checked_delete_object(obj);
    std::println("End solution");
}

}  // namespace solution
int main() {
    utils::MainDecorator::Access();
    bad_case::run();
    solution::run();
}
