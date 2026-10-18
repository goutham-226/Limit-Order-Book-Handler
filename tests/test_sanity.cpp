#include<gtest/gtest.h>
#include<atomic>
#include<cstddef>

TEST(Rig, CompilerIsCXX23)
{
    EXPECT_GT(__cplusplus, 202002L);
}

TEST(Rig, AtomicSizeTIsLockFree)
{
    EXPECT_TRUE(std::atomic<std::size_t>::is_always_lock_free);
}
