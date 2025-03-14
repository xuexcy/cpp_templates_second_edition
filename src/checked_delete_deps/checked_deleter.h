#pragma once

#include "object.h"
// struct Object;

template <class T>
inline void checked_delete(T* x) {
    // 注释掉 #include "object.h" 就会编译出错
    // 因为下方 sizeof(T) 在编译时可能 error: invalid application of 'sizeof' to an incomplete type 'Object'
    // 或者 return zero, 如果 return zero 那 char[-1] 也会编译错误
    using type_must_be_complete = char[sizeof(T)? 1 : -1];
    // (void) sizeof(type_must_be_complete);
    delete x;
}

template <class T>
inline void checked_array_delete(T* x) {
    using type_must_be_complete = char[sizeof(T)? 1 : -1];
    (void) sizeof(type_must_be_complete);
    delete [] x;
}

template <class T>
struct checked_deleter {
    void operator()(T* x) const {
        checked_delete(x);
    }
};
template <class T>
struct checked_array_deleter {
    void operator()(T* x) const {
        checked_array_delete(x);
    }
};

void checked_delete_object(Object* obj);
