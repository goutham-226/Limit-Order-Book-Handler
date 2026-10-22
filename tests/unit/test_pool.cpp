//tests/unit/test_pool.cpp

#include<gtest/gtest.h>
#include<lob/core/pool.hpp>

/*
 * test if an empty pool
 * has zero active slots.
**/
TEST(Pool, FreshPoolisEmpty)
{
    lob::Pool p;
    EXPECT_EQ(p.slots_in_use(),0);
}

/*
 * test if a deleted slot is
 * reused for a new slot.
**/
TEST(pool, DeletedSlotIsReusedFirst)
{
    lob::Pool p;
    const auto order_val = 10;
    const auto a  = p.add_order(order_val);
    const auto truth_val = p.delete_order(a);
   
    EXPECT_EQ(p.add_order(order_val),a);
}

/*
 * test if double deleting 
 * a slot returns false.
**/
TEST(pool, DoubleDeleteReturnsFalse)
{
    lob::Pool p;
    const auto a = p.add_order(10);
    const auto deleted = p.delete_order(a);
    EXPECT_EQ(p.delete_order(a),false);
}



