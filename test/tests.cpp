
#include <gtest/gtest.h>

#include <lazycpp.hpp>

using namespace lazycpp;
using namespace lazycpp::detail;

static int build_int()
{
    return 3;
}

// Test the basic functionality of the non-lazy implementation of the LazyConcept
TEST(NotLazy, BasicTests)
{
    auto myLazyInt = NotLazy<int>(3);
    ASSERT_EQ(*myLazyInt, 3);
    ASSERT_TRUE(myLazyInt.isEvaluated());

    myLazyInt = 5;
    ASSERT_EQ(*myLazyInt, 5);
}

// Test the basic functionality of the variant-backed implementation of the LazyConcept
TEST(VariantLazy, LazyTests)
{
    auto myLazyInt = VariantLazy<int>(build_int);
    ASSERT_FALSE(myLazyInt.isEvaluated());
    ASSERT_EQ(*myLazyInt, build_int());
    ASSERT_TRUE(myLazyInt.isEvaluated());

    auto copy = myLazyInt;
    ASSERT_TRUE(copy.isEvaluated());

    copy = 5;
    ASSERT_EQ(*copy, 5);
}

TEST(VariantLazy, EnsureObjectIsEvaluatedOnce)
{
    int counter = 0;
    ASSERT_EQ(counter, 0);
    auto lazy = VariantLazy<int>([&counter]() { return ++counter; });

    ASSERT_EQ(counter, 0);

    // Evaluate the lazy variable and ensure the counter goes up to one
    ASSERT_EQ(lazy.get_value(), 1);
    ASSERT_EQ(counter, 1);

    // Retrieve the value again and ensure the counter remains the same
    ASSERT_EQ(lazy.get_value(), 1);
    ASSERT_EQ(counter, 1);
}

// Test the basic functionality of the SharedLazy type
TEST(LazyWrapper, Constructor)
{
    auto myNonLazyInt = SharedLazy<int>(std::make_shared<VariantLazy<int>>(build_int));

    auto copy1 = myNonLazyInt;
    ASSERT_FALSE(copy1.isEvaluated());
    ASSERT_FALSE(myNonLazyInt.isEvaluated());
    ASSERT_EQ(*myNonLazyInt, build_int());
    ASSERT_TRUE(myNonLazyInt.isEvaluated());
    ASSERT_TRUE(copy1.isEvaluated());

    auto copy2 = myNonLazyInt;
    ASSERT_TRUE(copy2.isEvaluated());
    ASSERT_EQ(*copy2, build_int());
    ASSERT_TRUE(copy2.isEvaluated());
}
