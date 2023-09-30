#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <fe/ring.h>

#include <doctest/doctest.h>

TEST_CASE("ring1") {
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
