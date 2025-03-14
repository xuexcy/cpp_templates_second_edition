#include "checked_deleter.h"

#include <print>

void checked_delete_object(Object *obj) {
    std::println("start checked_delete Object");
    // compile error
    checked_delete<Object>(obj);
}
