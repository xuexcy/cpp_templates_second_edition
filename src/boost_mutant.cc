/**
如果两个类中的成员的内存布局相同，则可以通过 reinterpret_cast 将其中一个类的对象指针/引用转换成另一个类的对象指针/引用
https://www.boost.org/doc/libs/1_76_0/boost/bimap/relation/detail/mutant.hpp

如下, pair 中的 first 和 second，在 Reverse<pair> 中对应为 second 和 first (其实就是将原 class 的成员换了一个名字)
 */
#include <print>
#include <utility>

#include "boost/bimap.hpp"

#include "utils/main_decorator.h"

template <class Pair>
struct Reverse {
    typedef typename Pair::first_type second_type;
    typedef typename Pair::second_type first_type;
    second_type second ;
    first_type first;
};

template <class Pair>
Reverse<Pair>& mutate(Pair& p) {
    return reinterpret_cast<Reverse<Pair>&>(p);
}


// https://www.boost.org/doc/libs/1_82_0/libs/bimap/doc/html/boost_bimap/one_minute_tutorial.html
void run_boost_bimap() {
    using bm_type = boost::bimap<int, std::string>;
    bm_type bm;
    bm.insert(bm_type::value_type(1, "one"));
    bm.insert(bm_type::value_type(2, "two"));
    std::println("relations view:");
    for (auto it = bm.begin(); it != bm.end(); ++it) {
        std::println("left: {}  <--> right: {}", it->left, it->right);
    }
    std::println();

    std::println("left view:");
    for (auto it = bm.left.begin(); it != bm.left.end(); ++it) {
        std::println("first(left): {} --> second(right): {}", it->first, it->second);
    }
    std::println();

    std::println("right view:");
    for (auto it = bm.right.begin(); it != bm.right.end(); ++it) {
        std::println("first(right): {} --> second(left): {}", it->first, it->second);
    }
    std::println();

    assert(bm.right.find("two")->second == 2);
    assert(bm.right.at("one") == 1);
    bm.right.erase("two");
    bm.right.insert(bm_type::right_value_type("four", 4));

    // soccer world cup
    using ResultsBiMap = boost::bimap<char, int>;
    using Position = ResultsBiMap::value_type;
    ResultsBiMap results;
    std::vector<char> vecs{'A', 'S', 'G', 'F'};
    for (auto i = 0; i < vecs.size(); ++i) {
        results.insert(Position(vecs[i], i + 1));
    }
    std::println("results size: {}", results.size());
    std::println("winner: {}", results.right.at(1));
    for (auto it = results.right.begin(); it != results.right.end(); ++it) {
        std::println("rank: {}, name: {}", it->first, it->second);
    }
    for (auto it = results.left.begin(); it != results.left.end(); ++it) {
        std::println("name: {}, rank: {}", it->first, it->second);
    }
}

int main() {
    utils::MainDecorator::Access();
    std::pair<double, int> p(1.34, 5);
    std::println("p.first={}, p.second={}", p.first, p.second);
    std::println("mutate(p.first) = {}", mutate(p).first);
    std::println("mutate(p.second) = {}", mutate(p).second);
    run_boost_bimap();
    return 0;
}

