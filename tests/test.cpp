#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <fe/arena.h>
#include <fe/ring.h>
#include <fe/sym.h>
#include <fe/utf8.h>

using namespace std::literals;

TEST_CASE("Arena") {
    fe::Arena arena;
    std::vector<int, fe::Arena::Allocator<int>> v(arena.allocator<int>());
    for (int i = 0, e = 10000; i != e; ++i) v.emplace_back(i);
}

TEST_CASE("Ring") {
    fe::Ring<int, 1> ring1;
    ring1[0] = 0;
    CHECK(ring1.front() == 0);
    ring1.put(1);
    CHECK(ring1.front() == 1);

    fe::Ring<int, 2> ring2;
    ring2[0] = 0;
    ring2[1] = 1;
    CHECK(ring2.front() == 0);
    CHECK(ring2[0] == 0);
    CHECK(ring2[1] == 1);
    ring2.put(2);
    CHECK(ring2.front() == 1);
    CHECK(ring2[0] == 1);
    CHECK(ring2[1] == 2);
    ring2.put(3);
    CHECK(ring2.front() == 2);
    CHECK(ring2[0] == 2);
    CHECK(ring2[1] == 3);

    fe::Ring<int, 3> ring3;
    ring3[0] = 0;
    ring3[1] = 1;
    ring3[2] = 2;
    CHECK(ring3.front() == 0);
    CHECK(ring3[0] == 0);
    CHECK(ring3[1] == 1);
    CHECK(ring3[2] == 2);
    ring3.put(3);
    CHECK(ring3.front() == 1);
    CHECK(ring3[0] == 1);
    CHECK(ring3[1] == 2);
    CHECK(ring3[2] == 3);
    ring3.put(4);
    CHECK(ring3.front() == 2);
    CHECK(ring3[0] == 2);
    CHECK(ring3[1] == 3);
    CHECK(ring3[2] == 4);
    ring3.put(5);
    CHECK(ring3.front() == 3);
    CHECK(ring3[0] == 3);
    CHECK(ring3[1] == 4);
    CHECK(ring3[2] == 5);
}

TEST_CASE("Sym") {
    fe::SymPool syms;

    CHECK(syms.sym("a").view() == "a"s);
    CHECK(syms.sym("ab").view() == "ab"s);
    CHECK(syms.sym("abc").view() == "abc"s);
    CHECK(syms.sym("abcd").view() == "abcd"s);
    CHECK(syms.sym("abcde").view() == "abcde"s);
    CHECK(syms.sym("abcdef").view() == "abcdef"s);
    CHECK(syms.sym("abcdefg").view() == "abcdefg"s);
    CHECK(syms.sym("abcdefgh").view() == "abcdefgh"s);
    CHECK(syms.sym("abcdefghi").view() == "abcdefghi"s);
    CHECK(syms.sym("abcdefghij").view() == "abcdefghij"s);

    auto abc = syms.sym("abc");
    auto x   = syms.sym("");
    auto b   = syms.sym("b");
    auto bc  = syms.sym("bc");
    CHECK(b == 'b');
    CHECK(b != 'a');
    CHECK(b <= 'b');
    CHECK(b >= 'b');
    CHECK(b < 'c');
    CHECK(b > 'a');
    CHECK(bc < 'c');
    CHECK(bc > 'a');
    CHECK(bc > 'b');
    auto xyz = syms.sym("xyz");
    std::string zyx;
    for (auto i = xyz.rbegin(), e = xyz.rend(); i != e; ++i) zyx += *i;
    CHECK(zyx == "zyx");
    CHECK(xyz.front() == 'x');
    CHECK(xyz.back() == 'z');
    CHECK(xyz.size() == 3);
    auto empty = fe::Sym();
    CHECK(empty.empty());
    CHECK(empty.size() == 0);
    CHECK(!empty);
}

TEST_CASE("utf8") {
    std::ostringstream oss;
    fe::utf8::encode(oss, U'a');
    fe::utf8::encode(oss, U'£');
    fe::utf8::encode(oss, U'λ');
    fe::utf8::encode(oss, U'𐄂');
    fe::utf8::encode(oss, U'𐀮');
    CHECK(oss.str() == "a£λ𐄂𐀮");
}
