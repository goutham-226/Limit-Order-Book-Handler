//tests/unit/test_pool.cpp

#include<gtest/gtest.h>
#include<lob/core/pool.hpp>

TEST(Pool, FreshPoolisEmpty)
{
    lob::Pool p;
    EXPECT_EQ(p.slots_in_use(),0);
}

