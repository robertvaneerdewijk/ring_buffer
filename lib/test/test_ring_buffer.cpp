#include <gtest/gtest.h>

#include <ring_buffer.hpp>

TEST(CircularArithmeticSimple, Increment) {
    constexpr size_t size = 5;
    size_t cidx = 0;

    EXPECT_EQ(cidx, 0);
    for (size_t i = 0; i < 3; ++i) {
        cidx = detail::CircularArithmeticSimple::increment(cidx, size);
        EXPECT_EQ(static_cast<size_t>(cidx), 1);
        cidx = detail::CircularArithmeticSimple::increment(cidx, size);
        EXPECT_EQ(static_cast<size_t>(cidx), 2);
        cidx = detail::CircularArithmeticSimple::increment(cidx, size);
        EXPECT_EQ(static_cast<size_t>(cidx), 3);
        cidx = detail::CircularArithmeticSimple::increment(cidx, size);
        EXPECT_EQ(static_cast<size_t>(cidx), 4);
        cidx = detail::CircularArithmeticSimple::increment(cidx, size);
        EXPECT_EQ(static_cast<size_t>(cidx), 0);
    }
}
TEST(CircularArithmeticSimple, Decrement) {
    constexpr size_t size = 5;
    size_t cidx = 0;

    EXPECT_EQ(cidx, 0);
    for (size_t i = 0; i < 3; ++i) {
        cidx = detail::CircularArithmeticSimple::decrement(cidx, size);
        EXPECT_EQ(static_cast<size_t>(cidx), 4);
        cidx = detail::CircularArithmeticSimple::decrement(cidx, size);
        EXPECT_EQ(static_cast<size_t>(cidx), 3);
        cidx = detail::CircularArithmeticSimple::decrement(cidx, size);
        EXPECT_EQ(static_cast<size_t>(cidx), 2);
        cidx = detail::CircularArithmeticSimple::decrement(cidx, size);
        EXPECT_EQ(static_cast<size_t>(cidx), 1);
        cidx = detail::CircularArithmeticSimple::decrement(cidx, size);
        EXPECT_EQ(static_cast<size_t>(cidx), 0);
    }
}
TEST(CircularArithmeticSimple, Plus) {
    constexpr size_t size = 5;
    size_t cidx = 0;

    cidx = detail::CircularArithmeticSimple::plus(cidx, 0, size);
    EXPECT_EQ(cidx, 0);
    cidx = detail::CircularArithmeticSimple::plus(cidx, 1, size);
    EXPECT_EQ(cidx, 1);
    cidx = detail::CircularArithmeticSimple::plus(cidx, 2, size);
    EXPECT_EQ(cidx, 3);
    cidx = detail::CircularArithmeticSimple::plus(cidx, 3, size);
    EXPECT_EQ(cidx, 1);
    cidx = detail::CircularArithmeticSimple::plus(cidx, 4, size);
    EXPECT_EQ(cidx, 0);
    cidx = detail::CircularArithmeticSimple::plus(cidx, 5, size);
    EXPECT_EQ(cidx, 0);
    cidx = detail::CircularArithmeticSimple::plus(cidx, 6, size);
    EXPECT_EQ(cidx, 1);
    cidx = detail::CircularArithmeticSimple::plus(cidx, 7, size);
    EXPECT_EQ(cidx, 3);
    cidx = detail::CircularArithmeticSimple::plus(cidx, 8, size);
    EXPECT_EQ(cidx, 1);
    cidx = detail::CircularArithmeticSimple::plus(cidx, 9, size);
    EXPECT_EQ(cidx, 0);

    cidx = detail::CircularArithmeticSimple::plus(0, 4, size);
    EXPECT_EQ(cidx, 4);
    cidx = detail::CircularArithmeticSimple::plus(0, 5, size);
    EXPECT_EQ(cidx, 0);
    cidx = detail::CircularArithmeticSimple::plus(0, 6, size);
    EXPECT_EQ(cidx, 1);
    cidx = detail::CircularArithmeticSimple::plus(0, 7, size);
    EXPECT_EQ(cidx, 2);
}
TEST(CircularArithmeticSimple, Minus) {
    constexpr size_t size = 5;
    size_t cidx = 0;

    cidx = detail::CircularArithmeticSimple::minus(cidx, 0, size);
    EXPECT_EQ(cidx, 0);
    cidx = detail::CircularArithmeticSimple::minus(cidx, 1, size);
    EXPECT_EQ(cidx, 4);
    cidx = detail::CircularArithmeticSimple::minus(cidx, 2, size);
    EXPECT_EQ(cidx, 2);
    cidx = detail::CircularArithmeticSimple::minus(cidx, 3, size);
    EXPECT_EQ(cidx, 4);
    cidx = detail::CircularArithmeticSimple::minus(cidx, 4, size);
    EXPECT_EQ(cidx, 0);
    cidx = detail::CircularArithmeticSimple::minus(cidx, 5, size);
    EXPECT_EQ(cidx, 0);
    cidx = detail::CircularArithmeticSimple::minus(cidx, 6, size);
    EXPECT_EQ(cidx, 4);

    cidx = detail::CircularArithmeticSimple::minus(0, 4, size);
    EXPECT_EQ(cidx, 1);
    cidx = detail::CircularArithmeticSimple::minus(0, 5, size);
    EXPECT_EQ(cidx, 0);
    cidx = detail::CircularArithmeticSimple::minus(0, 6, size);
    EXPECT_EQ(cidx, 4);
    cidx = detail::CircularArithmeticSimple::minus(0, 7, size);
    EXPECT_EQ(cidx, 3);
}

TEST(SaturatingCounter, Basic) {
    EXPECT_EQ(detail::SaturatingCounter::increment(0, 10), 1);
    EXPECT_EQ(detail::SaturatingCounter::increment(1, 10), 2);
    EXPECT_EQ(detail::SaturatingCounter::increment(2, 10), 3);
    EXPECT_EQ(detail::SaturatingCounter::increment(3, 10), 4);

    {
        size_t i = 0;
        i = detail::SaturatingCounter::increment(i, 10);
        EXPECT_EQ(i, 1);
        i = detail::SaturatingCounter::increment(i, 10);
        EXPECT_EQ(i, 2);
        i = detail::SaturatingCounter::increment(i, 10);
        EXPECT_EQ(i, 3);
    }

    for (size_t k = 1; k < 99; ++k) {
        for (size_t j = 0; j < k; ++j) {
            ASSERT_LE(detail::SaturatingCounter::increment(j, k), k);
        }
    }
}

TEST(RingBuffer, Size) {
    for (size_t k = 0; k < 33; ++k) {
        RingBuffer<int> ringbuffer{k};

        ASSERT_EQ(ringbuffer.size(), k);
    }
}

TEST(RingBuffer, Count) {
    RingBuffer<int> ringbuffer{5};

    EXPECT_EQ(ringbuffer.count(), 0);
    ringbuffer.push_back(0);
    EXPECT_EQ(ringbuffer.count(), 1);
    ringbuffer.push_back(0);
    EXPECT_EQ(ringbuffer.count(), 2);
    ringbuffer.push_back(0);
    EXPECT_EQ(ringbuffer.count(), 3);
    ringbuffer.push_back(0);
    EXPECT_EQ(ringbuffer.count(), 4);
    ringbuffer.push_back(0);
    EXPECT_EQ(ringbuffer.count(), 5);
    for (size_t i = 0; i < 6; ++i) {
        ringbuffer.push_back(0);
        EXPECT_EQ(ringbuffer.count(), 5);
    }

    for (size_t i = ringbuffer.size(); i > 0; --i) {
        EXPECT_EQ(ringbuffer.count(), i);
        int x;
        ringbuffer.pop_front(&x);
    }
    EXPECT_EQ(ringbuffer.count(), 0);
}

TEST(RingBuffer, PopFront) {
    RingBuffer<size_t> ringbuffer{5};

    for (size_t i = 0; i < ringbuffer.size(); ++i) {
        ringbuffer.push_back(i);
    }

    for (size_t i = 0; i < ringbuffer.size(); ++i) {
        size_t x;
        ringbuffer.pop_front(&x);
        ASSERT_EQ(x, i);
    }

    for (size_t i = 0; i < 1234; ++i) {
        ringbuffer.push_back(i);
    }

    size_t t;
    size_t expect = 1234 - ringbuffer.size();
    while (ringbuffer.pop_front(&t)) {
        ASSERT_EQ(t, expect);
        ++expect;
    }
}

TEST(RingBuffer, PopBack) {
    RingBuffer<size_t> ringbuffer{5};

    for (size_t i = 0; i < 5; ++i) {
        ringbuffer.push_back(i);
    }

    for (int i = 4; i >= 0; --i) {
        size_t x;
        ringbuffer.pop_back(&x);
        ASSERT_EQ(x, static_cast<size_t>(i));
    }

    for (size_t i = 0; i < 1234; ++i) {
        ringbuffer.push_back(i);
    }

    size_t t;
    size_t expect = 1234 - 1;
    while (ringbuffer.pop_back(&t)) {
        ASSERT_EQ(t, expect);
        --expect;
    }
}

TEST(RingBuffer, Back) {
    RingBuffer<size_t> ringbuffer{5};

    for (size_t i = 0; i < 333; ++i) {
        ringbuffer.push_back(i);
        ASSERT_EQ(ringbuffer.back(), i);
    }
}

TEST(RingBuffer, BackN) {
    RingBuffer<size_t> ringbuffer{5};

    for (size_t i = 0; i < 333; ++i) {
        ringbuffer.push_back(i);
        ASSERT_EQ(ringbuffer.back(0), i);
    }

    for (size_t i = 0; i < 5; ++i) {
        ringbuffer.push_back(i);
        ASSERT_EQ(ringbuffer.back(0), i);
        ASSERT_EQ(ringbuffer.back(i), 0);
    }
}

TEST(RingBuffer, Front) {
    RingBuffer<size_t> ringbuffer{5};

    for (size_t i = 0; i < ringbuffer.size(); ++i) {
        ringbuffer.push_back(i);
    }

    for (size_t i = ringbuffer.size(); i < 333; ++i) {
        ASSERT_EQ(ringbuffer.front(), i - ringbuffer.size());
        ringbuffer.push_back(i);
    }
}

TEST(RingBuffer, FrontN) {
    RingBuffer<size_t> ringbuffer{5};

    for (size_t i = 0; i < ringbuffer.size(); ++i) {
        ringbuffer.push_back(i);
    }
    for (size_t k = 0; k < ringbuffer.size(); ++k) {
        ASSERT_EQ(ringbuffer.front(k), k);
    }

    for (size_t i = ringbuffer.size(); i < 333; ++i) {
        ASSERT_EQ(ringbuffer.front(), i - ringbuffer.size());
        for (size_t k = 0; k < ringbuffer.size(); ++k) {
            ASSERT_EQ(ringbuffer.front(k), i - ringbuffer.size() + k);
        }
        ringbuffer.push_back(i);
    }
}

TEST(RingBuffer, Empty) {
    RingBuffer<size_t> ringbuffer{5};

    ASSERT_TRUE(ringbuffer.empty());

    ringbuffer.push_back(0);
    ASSERT_FALSE(ringbuffer.empty());

    size_t x;

    ringbuffer.pop_front(&x);
    ASSERT_TRUE(ringbuffer.empty());

    for (size_t i = 0; i < 333; ++i) {
        ringbuffer.push_back(i);
        ASSERT_FALSE(ringbuffer.empty());
    }

    for (size_t i = 0; i < ringbuffer.size() - 1; ++i) {
        ringbuffer.pop_front(&x);
        ASSERT_FALSE(ringbuffer.empty());
    }
    ringbuffer.pop_front(&x);
    ASSERT_TRUE(ringbuffer.empty());
}

TEST(RingBuffer, Full) {
    RingBuffer<size_t> ringbuffer{5};

    ASSERT_FALSE(ringbuffer.full());

    for (size_t i = 0; i < ringbuffer.size() - 1; ++i) {
        ringbuffer.push_back(i);
        ASSERT_FALSE(ringbuffer.full());
    }

    for (size_t i = 0; i < 333; ++i) {
        ringbuffer.push_back(i);
        ASSERT_TRUE(ringbuffer.full());
    }

    for (size_t i = 0; i < ringbuffer.size(); ++i) {
        size_t x;
        ringbuffer.pop_front(&x);
        ASSERT_FALSE(ringbuffer.full());
    }
}

TEST(RingBuffer, Clear) {
    RingBuffer<size_t> ringbuffer{5};

    for (size_t k = 0; k < 33; ++k) {
        for (size_t i = 0; i < 13; ++i) {
            ringbuffer.push_back(i);
        }
        ringbuffer.clear();
        ASSERT_TRUE(ringbuffer.empty());
    }
}
