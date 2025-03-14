#include "deleter.h"

#include <print>

void delete_object(Object *obj) {
    std::println("start delete Object");
    // compiler warning:
    // Deleting pointer to incomplete type 'Object' is incompatible with C++2c and may cause undefined behavior (fix available)
    delete obj;
}
